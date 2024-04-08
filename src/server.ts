import * as http from "node:http";
import child_process from "node:child_process";
import fs from "node:fs";

function streamToString(stream) {
  const chunks = [];
  return new Promise((resolve, reject) => {
    stream.on("data", (chunk) => chunks.push(Buffer.from(chunk)));
    stream.on("error", (err) => reject(err));
    stream.on("end", () => resolve(Buffer.concat(chunks).toString("utf8")));
  });
}

const ids = [];
let next_id = 0;
function create_id() {
  if (ids.length === 0) return next_id++;
  return ids.pop();
}
function free_id(id) {
  ids.push(id);
}

http
  .createServer((request, response) => {
    const headers = {
      "Access-Control-Allow-Origin": "*" /* @dev First, read about security */,
      "Access-Control-Allow-Methods": "OPTIONS, POST, GET",
      "Access-Control-Max-Age": 2592000, // 30 days
      /** add other headers as per requirement */
    };

    if (request.method === "OPTIONS") {
      res.writeHead(204, headers);
      res.end();
      return;
    } else if (request.method === "POST") {
      const id = create_id();
      request
        .pipe(fs.createWriteStream(`compile/main${id}.cpp`))
        .on("finish", () => {
          response.writeHead(200, headers);
          if (request.url?.includes("/execute")) {
            const clang = child_process.spawn(
              "clang++",
              [`main${id}.cpp`, "-std=c++2a", "-O2", "-o", `main${id}`, "2>&1"],
              { shell: true, cwd: "compile" }
            );
            const clang_stdout = streamToString(clang.stdout);
            const clang_stderr = streamToString(clang.stderr);

            clang.on("close", async (code) => {
              if (code != 0) {
                response.end(
                  JSON.stringify({
                    code,
                    stdout: await clang_stdout,
                    stderr: await clang_stderr,
                    output: "",
                  })
                );
                free_id(id);
              } else {
                const exe = child_process.spawn(`./main${id}`, [], {
                  shell: true,
                  cwd: "compile",
                });
                const exe_stdout = streamToString(exe.stdout);
                const exe_stderr = streamToString(exe.stderr);

                exe.on("close", async (code) => {
                  response.end(
                    JSON.stringify({
                      code,
                      stdout: "",
                      stderr: await exe_stderr,
                      output: await exe_stdout,
                    })
                  );
                  console.log(await exe_stderr, await exe_stdout );
                  free_id(id);
                });
              }
            });
          } else {
            const clang = child_process.spawn(
              "clang++",
              [
                `main${id}.cpp`,
                "-std=c++2a",
                "-O2",
                "-S",
                "-mllvm",
                "--x86-asm-syntax=intel",
                "-fno-asynchronous-unwind-tables",
                "-fno-exceptions",
                "-fno-rtti",
                "-o",
                `main${id}.s`,
                "2>&1",
              ],
              { shell: true, cwd: "compile" }
            );
            const stdout = streamToString(clang.stdout);
            const stderr = streamToString(clang.stderr);

            clang.on("close", async (code) => {
              response.end(
                JSON.stringify({
                  code,
                  stdout: await stdout,
                  stderr: await stderr,
                  output:
                    code === 0
                      ? await streamToString(
                          fs.createReadStream(`compile/main${id}.s`)
                        )
                      : "",
                })
              );
              free_id(id);
            });
          }
        });
    }
  })
  .listen(1235);

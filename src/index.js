import Reveal from "reveal.js";
import RevealHighlight from "reveal.js/plugin/highlight/highlight";
import cpp from "highlight.js/lib/languages/cpp";
import { highlightElement } from "highlight.js";

let deck = new Reveal({
  plugins: [RevealHighlight],
  highlight: {
    beforeHighlight: (hljs) => hljs.registerLanguage("c++", cpp),
  },
  slideNumber: "c/t",
  margin: 0,
  width: 1920,
  height: 1080,
  center: false,
  hash: true,
});
deck.initialize();

for (const toCompile of document.getElementsByClassName("compile")) {
  const code = toCompile.children[0];
  const output = document.getElementById(toCompile.dataset.compile);
  let i = 0;
  const compile = () => {
    fetch(
      output.dataset.execute !== undefined
        ? "http://localhost:1235/execute"
        : "http://localhost:1235/",
      {
        method: "POST",
        body: code.innerText,
      }
    )
      .then((res) => res.text())
      .then((text) => {
        const data = JSON.parse(text);
        console.log(data);
        if (data.code === 0) {
          if (output.dataset.assembly !== undefined) {
            output.innerText = data.output
              .replace(/^\s*\..*$/gim, "")
              .replace(/\n+/g, "\n")
              .trim();
          } else if(output.dataset.execute !== undefined) {
            output.innerText = data.output.trim();
          } else {
            output.innerText = "exit code: 0";
          }
        } else {
          output.innerText = data.stdout + data.stderr;
        }
      });
  };
  code.addEventListener("input", () => {
    delete code.dataset.highlighted;
    output.innerText = "Compiling...";
    clearTimeout(i);
    i = setTimeout(compile, 500);
  });
  compile();
}

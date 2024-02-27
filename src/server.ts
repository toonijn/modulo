import * as http from "node:http";

http.createServer((request, response) => {
    console.log("New request!");
}).listen(1235);

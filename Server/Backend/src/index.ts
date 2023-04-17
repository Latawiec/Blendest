import express from 'express';
import { WebSocketServer } from 'ws';

const app: express.Application = express();
const port: number = 8080;

app.get('/file/:fileName(*)', (req, res)  => {
  console.log(req.params.fileName);
  const filename = req.params.fileName;
  console.log(`requested D:/Programming/Blendest/Server/Backend/Assets/${filename}`);
  res.sendFile(`D:/Programming/Blendest/Server/Backend/Assets/${filename}`);
});

const server = app.listen(port);

const wss = new WebSocketServer({ server: server });
wss.on('connection', function connection(ws) {
  console.log("Connection!");

ws.on('error', console.error);

ws.on('message', function message(data) {
  console.log('received: %s', data);
});

ws.on('close', function close(code, reason) {
  console.log("Closed: ", code, " ", reason);
})

ws.send('something');
ws.send('something else');

let i = 0;
setInterval(() => {
  ws.send(`Hi: ${i}`);
  i++;
}, 1000)
});


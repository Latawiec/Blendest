import express from 'express';
import fileUpload from 'express-fileupload';
import bodyParser from 'body-parser';
import * as WebSocket from 'websocket';

import { ConnectionManager } from '@net/ConnectionManager';

const app: express.Application = express();
const port = process.env.PORT || 8080;

app.use(fileUpload({
  debug: true
}));

const server = app.listen(port, () => {
  console.log("Blendest server running on port: " + port);
});

const wsServer = new WebSocket.server({
  httpServer: server
});

app.get('/file/:fileName(*)', (req, res)  => {
  console.log(req.params.fileName);
  const filename = req.params.fileName;
  console.log(`requested D:/Programming/Blendest/Server/Backend/Assets/${filename}`);
  res.sendFile(`D:/Programming/Blendest/Server/Backend/Assets/${filename}`);
});

app.post('/send', (req, res) => {

  console.log(JSON.stringify(req.headers));

  if (!req.files || Object.keys(req.files).length === 0) {
    console.log("No files were uploaded.");
    return res.status(400).send('No files were uploaded.');
  }

  console.log(req.files);

  return res.status(200).send("OK");
});

const clientManager = new ConnectionManager(wsServer);

clientManager.on('connectionCreated', (connection) =>{
  console.log("Connected: [%d] %s", connection.id, connection.address);
  connection.on('message', (data) => {
    console.log("message");
  })
  connection.on('close', (data) => {
    console.log("close");
  })
})

clientManager.on('connectionClosed', (connection) => {
  console.log("Disconnected: [%d] %s", connection.id, connection.address);
})
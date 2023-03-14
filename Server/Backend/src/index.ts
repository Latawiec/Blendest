import { WebSocketServer } from 'ws';

const wss = new WebSocketServer({ port: 8080 });

wss.on('connection', function connection(ws) {
    console.log("Connection!");

  ws.on('error', console.error);

  ws.on('message', function message(data) {
    console.log('received: %s', data);
  });

  ws.send('something');

  ws.send('something else');

  ws.close(1013, "YOOO");
});
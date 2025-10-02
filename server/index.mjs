import net from "net";
import { Buffer } from "buffer";
const ESP32_TCP_SOCKET_PORT = 10032;
const TCP_SOCKET_PORT = 10000;
const WEB_SOCKET_PORT = 11000;

let esp32Socket = null;
let tcpSocketClients = [];
let jsonState = false;

let buffer = Buffer.alloc(0);
let readPos = 0;
let remainJsonLen = 0;
let jsonBuffer = [];
const esp32TcpSocketServer = net.createServer((socket) => {
  console.log("TCP Socket client connected");

  //접속한 클라이언트 소켓을 배열에 추가
  esp32Socket = socket;

  //소켓이 작동중일때 메시지를 받은 경우

  socket.on("data", (data) => {
    // 아직 안 읽은 데이터가 buffer에 남아있음
    if (readPos > 0) {
      // 남은 데이터만 잘라서 새로운 버퍼로 만들기
      buffer = buffer.subarray(readPos);
      readPos = 0;
    }
    buffer = Buffer.concat([buffer, data]);
    // console.log(data);

    while (true) {
      if (jsonState && remainJsonLen === 0) {
        break;
      }
      const b = readByte();
      if (b === null) {
        break;
      }
      if (!jsonState) {
        if (b === "\n".charCodeAt(0)) {
          jsonState = true;
        } else {
          remainJsonLen *= 10;
          remainJsonLen += b - "0".charCodeAt(0);
        }
      } else {
        jsonBuffer.push(b);
        remainJsonLen -= 1;
      }
    }
    if (jsonState && remainJsonLen === 0) {
      jsonState = false;
      const jsonStr = Buffer.from(jsonBuffer).toString();
      const JSONobj = JSON.parse(jsonStr);
      jsonBuffer = [];
      console.log(JSONobj);
    }
    //로그 저장기능
    // const dataObj = JSON.parse(data.toString);

    //브로드 캐스트
    // tcpSocketClients.forEach((tcpSocketClient) => {
    //   tcpSocketClient.write(data);
    // });
  });
});
function readByte() {
  if (readPos >= buffer.length) return null;
  const byte = buffer[readPos];
  readPos++;

  return byte;
}
const tcpSocketServer = net.createServer((socket) => {
  console.log("TCP Socket client connected");

  //접속한 클라이언트 소켓을 배열에 추가
  tcpSocketClients.push(socket);

  //소켓이 작동중일때 메시지를 받은 경우
  socket.on("data", (data) => {
    console.log("Received from TCP socket client : ", data.toString());
  });
});

esp32TcpSocketServer.listen(ESP32_TCP_SOCKET_PORT, () =>
  console.log(
    `esp32 server is waiting for client connecting : ${ESP32_TCP_SOCKET_PORT}`
  )
);
tcpSocketServer.listen(TCP_SOCKET_PORT, () =>
  console.log(`server is waiting for client connecting : ${TCP_SOCKET_PORT}`)
);

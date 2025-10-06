import net from "net";
import { Buffer } from "buffer";
const ESP32_TCP_SOCKET_PORT = 10032;
const TCP_SOCKET_PORT = 10000;
const WEB_SOCKET_PORT = 11000;

let esp32Socket = null;
let tcpSocketClients = [];


const tcpSocketServer = net.createServer((socket) => {
  console.log("ESP32 클라이언트 접속:", socket.remoteAddress);

  // 예: 3초 후에 서버에서 클라이언트로 JSON 전송
  setTimeout(() => {
    const data = { type: "ping", msg: "Hello from Server" };
    const jsonStr = JSON.stringify(data);
    const length = Buffer.byteLength(jsonStr);

    const header = Buffer.alloc(4);
    header.writeUInt32BE(length, 0);

    socket.write(Buffer.concat([header, Buffer.from(jsonStr)]));
    console.log("서버 → ESP32 전송:", data);
  }, 3000);

  socket.on("data", (data) => {
    // 클라이언트 → 서버로 오는 응답도 받을 수 있음
    console.log("ESP32 → 서버 수신(raw):", data);
  });

  socket.on("close", () => {
    console.log("클라이언트 연결 종료");
  });
});

tcpSocketServer.listen(TCP_SOCKET_PORT, () =>
  console.log(`server is waiting for client connecting : ${TCP_SOCKET_PORT}`)
);

import net from "net";
import { Buffer } from "buffer";
const ESP32_TCP_SOCKET_PORT = 10032;
const TCP_SOCKET_PORT = 10000;
const WEB_SOCKET_PORT = 11000;

let esp32Socket = null;
let tcpSocketClients = [];

const esp32TcpSocketServer = net.createServer((socket) => {
  console.log("TCP Socket client connected");

  //접속한 클라이언트 소켓을 배열에 추가
  esp32Socket = socket;

  //소켓이 작동중일때 메시지를 받은 경우
  let buffer = Buffer.alloc(0);
  let expectedLength = null;
  socket.on("data", (data) => {
    // 아직 안 읽은 데이터가 buffer에 남아있음
    buffer = Buffer.concat([buffer, data]);
    while (true) {
      // 1) 길이 정보가 아직 없으면 4바이트 먼저 읽기
      if (expectedLength === null) {
        if (buffer.length < 4) break;

        expectedLength = buffer.readUInt32BE(0); // Big Endian으로 읽기
        buffer = buffer.slice(4);
      }

      // console.log(data);
      // 2) 본문이 다 왔는지 확인
      if (buffer.length < expectedLength) break;
      const jsonStr = buffer.subarray(0, expectedLength).toString();
      buffer = buffer.subarray(expectedLength);
      expectedLength = null;

      try {
        const obj = JSON.parse(jsonStr);
        console.log("수신된 JSON:", obj);
      } catch (err) {
        console.error("JSON 파싱 실패:", jsonStr);
      }
    }
    socket.on("close", () => {
      console.log("클라이언트 연결 종료");
      buffer = null;
      expectedLength = null;
    });
    //로그 저장기능
    // const dataObj = JSON.parse(data.toString);

    //브로드 캐스트
    // tcpSocketClients.forEach((tcpSocketClient) => {
    //   tcpSocketClient.write(data);
    // });
  });
});
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

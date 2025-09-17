import net from "net";

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
  socket.on("data", (data) => {
    console.log("Received from ESP32 TCP socket client : ", data.toString());

    //로그 저장기능
    // const dataObj = JSON.parse(data.toString);

    //브로드 캐스트
    tcpSocketClients.forEach((tcpSocketClient) => {
      tcpSocketClient.write(data);
    });
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

esp32TcpSocketServer.listen(ESP32_TCP_SOCKET_PORT,()=>console.log(`esp32 server is waiting for client connecting : ${ESP32_TCP_SOCKET_PORT}`));
tcpSocketServer.listen(TCP_SOCKET_PORT,()=>console.log(`server is waiting for client connecting : ${TCP_SOCKET_PORT}`));

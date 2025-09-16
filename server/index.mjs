import net from "net"

const ESP32_TCP_SOCKET_PORT = 10032;
const TCP_SOCKET_PORT = 10000;
const WEB_SOCKET_PORT = 11000;

let esp32Socket = null;
let tcpSocketClients = [];


const tcpSocketServer = net.createServer((socket) => {
    console.log("TCP Socket client connected");

    //접속한 클라이언트 소켓을 배열에 추가
    tcpSocketClients.push(socket);

    //소켓이 작동중일때 메시지를 받은 경우
    socket.on("data",(data)=>{
        console.log("Received from TCP socket client : ", data.toString());
        

    });
});
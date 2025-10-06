#include <WiFi.h>
#include <WiFiClient.h>

String ssid = "SK_WiFiGIGACBDC";
String password = "1903048634";
const uint16_t port = 10000;
const char *host = "192.168.35.189";

WiFiClient client;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("WiFi 연결중...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi 연결 완료");

  if (!client.connect(host, port)) {
    Serial.println("서버 연결 실패");
    return;
  }
  Serial.println("서버 연결 성공");
}

void loop() {
  // 서버 → ESP32 데이터 수신 처리
  if (client.available() >= 4) {
    uint32_t lenNet;
    client.readBytes((uint8_t*)&lenNet, 4);
    uint32_t length = ntohl(lenNet);

    String json;
    json.reserve(length);
    while (json.length() < length) {
      if (client.available()) {
        json += (char)client.read();
      }
    }

    Serial.print("서버로부터 수신된 JSON: ");
    Serial.println(json);

    // 필요시 서버에 응답도 가능
    String reply = "{\"status\":\"received\"}";
    uint32_t replyLenNet = htonl(reply.length());
    client.write((uint8_t*)&replyLenNet, 4);
    client.write((const uint8_t*)reply.c_str(), reply.length());
  }
}

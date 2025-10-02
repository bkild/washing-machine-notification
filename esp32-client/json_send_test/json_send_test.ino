#include <WiFi.h>
#include <ArduinoJson.h>

// ===========================
// Select camera model in board_config.h
// ===========================
#define CHUNK_SIZE 128
// ===========================
// Enter your WiFi credentials
// ===========================
// String ssid = "Hotspot7028";
// String password = "0622661079";
String ssid = "SK_WiFiGIGACBDC";
String password = "1903048634";
const uint16_t port = 10032;
const char *host = "192.168.35.189";
WiFiClient client;
void startCameraServer();
void setupLedFlash();


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // startCameraServer();

  Serial.print("connect success :");
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("Connection to host failed");
    delay(1000);
    return;
  }


  JsonDocument doc;
  // Add values in the document
  //데이터 전송은 무조건 문자열로 통일
  doc["device_id"] = "esp32_washer_01";
  doc["status"] = "running";  // running, completed, idle
  doc["time_left"] = "25";      // 남은 세탁 시간 (분 단위)
  doc["alert"] = "false";       // 세탁 완료 알림 여부


  client.print(measureJson(doc));
  client.print("\n");
  serializeJson(doc, client);
  serializeJson(doc, Serial);
  Serial.println();
  delay(1000);
}

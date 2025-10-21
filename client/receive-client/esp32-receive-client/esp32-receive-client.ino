#include "config.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include "seven_segment_led.h"
#include "little_star.h"
String ssid = WIFI_SSID;
String password = WIFI_PASSWORD;
const char *host = SERVER_IP;
const uint16_t port = SERVER_PORT;

WiFiClient client;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  seven_segment_init(1);
  little_star_init();
  little_star_start();
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Connecting");
  while (!client.connect(host, port)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected");
}
String status = "idle";
int time_left = 8888;
int alert = 0;
void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    Serial.println("Reconnecting...");
    if (client.connect(host, port)) {
      Serial.println("Reconnected");
      status = "idle";
      time_left = 8888;
      alert = 0;
      melodyState.playing=false;
    } else {
      delay(1000);
      return;
    }
  }

  if (client.available() >= 4) {
    uint32_t lenNet;
    client.readBytes((uint8_t *)&lenNet, 4);
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
    JsonDocument doc;
    deserializeJson(doc, json);
    if (doc.containsKey("type") && doc.containsKey("data")) {
      status = doc["data"]["status"].as<String>();
      time_left = doc["data"]["time_left"].as<int>();
      alert = doc["data"]["alert"].as<int>();
    }

    // 필요시 서버에 응답도 가능
    // String reply = "{\"status\":\"received\"}";
    // uint32_t replyLenNet = htonl(reply.length());
    // client.write((uint8_t *)&replyLenNet, 4);
    // client.write((const uint8_t *)reply.c_str(), reply.length());
  }
  
  if (status == "running") {
    show_min(time_left);
  } else if (status == "completed") {
    show_end();
  } else if (status == "error") {
    show_err();
  }
  // Serial.println(alert);
  if (alert) {
    if (!melodyState.playing) {
      little_star_start();
    }
    little_star_update();
  }
  show_off();
}

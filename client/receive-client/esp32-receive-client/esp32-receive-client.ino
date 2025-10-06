#include <WiFi.h>
String ssid = "SK_WiFiGIGACBDC";
String password = "1903048634";
const uint16_t port = 10032;
const char *host = "192.168.35.189";

WiFiClient client;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("Connection to host failed");
    delay(1000);
    return;
  }
  client.
  
}

#include <ArduinoJson.h>
#include <WiFi.h>
// String ssid = "Hotspot7028";
// String password = "0622661079";
String ssid = "SK_WiFiGIGACBDC";
String password = "1903048634";
const uint16_t port= 10032;
const char *host = "192.168.35.189";

WiFiClient client;

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {

    delay(500);
    Serial.println("...");
  }
  Serial.println("JSON parsing using ArduinoJson library on ESP32");
}

void loop()
{
  WiFiClient client;
  if (!client.connect(host, port))
  {
    Serial.println("Connection to host failed");
    delay(1000);
    return;
  }
  // put your main code here, to run repeatedly:
  Serial.println("Parsing start: ");
  char JSONMsg[] = "{\"SensorType\":\"Temperature\",\"Value\":10}";

  client.print(JSONMsg);
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, JSONMsg);
  if (error)
  { // Check for errors in parsing

    Serial.println("Parsing failed!");
    Serial.print("deserializeJson() returned ");
    Serial.println(error.c_str());
    delay(5000);

    return;
  }

  JsonObject obj = doc.as<JsonObject>();

  const char *sensorType = obj["SensorType"]; // Get sensor type value

  int value = obj["Value"]; // Get value of sensor measurement

  Serial.printf("Sensor type: %s\n", sensorType);
  Serial.printf("Sensor value: %d\n", value);
  Serial.println();

  delay(5000);
}

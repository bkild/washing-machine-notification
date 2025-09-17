#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("JSON parsing using ArduinoJson library on ESP32");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Parsing start: ");
  char JSONMsg[] = "{\"SensorType\":\"Temperature\",\"Value\":10}";


  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, JSONMsg);
  if (error) {  //Check for errors in parsing

    Serial.println("Parsing failed!");
    Serial.print("deserializeJson() returned ");
    Serial.println(error.c_str());
    delay(5000);

    return;
  }


  JsonObject obj = doc.as<JsonObject>();

  const char* sensorType = obj["SensorType"];  //Get sensor type value

  int value = obj["Value"];  //Get value of sensor measurement



  Serial.printf("Sensor type: %s\n", sensorType);
  Serial.printf("Sensor value: %d\n", value);



  Serial.println();

  delay(5000);
}

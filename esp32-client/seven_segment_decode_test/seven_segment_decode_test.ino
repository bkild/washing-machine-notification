#include "FS.h"
#include "SD_MMC.h"
#include "esp_heap_caps.h"
#include <TJpg_Decoder.h>
#include "seven_segment_led.h"



void setup() {
  Serial.begin(115200);
  seven_segment_init();
  if (!psramFound()) {
    Serial.println("PSRAM not found!");
    return;
  }

  if (!SD_MMC.begin("/sdcard", true)) {  // 1-bit 모드
    Serial.println("Card Mount Failed");
    return;
  }

  File file = SD_MMC.open("/sample3.jpg", "r");
  if (!file) {
    Serial.println("Failed to open file");
    return;
  }

  size_t fileSize = file.size();
  Serial.printf("File size: %u bytes\n", fileSize);

  // PSRAM에 uint8_t 배열 할당
  uint8_t *data = (uint8_t *)ps_malloc(fileSize);
  if (!data) {
    Serial.println("PSRAM malloc failed!");
    file.close();
    return;
  }

  size_t bytesRead = file.read(data, fileSize);
  file.close();

  Serial.printf("Read %u bytes into PSRAM\n", bytesRead);

  auto t = getTime(data, fileSize);
  if (t>=0) {
    Serial.printf("%03d\n", t);
  }else{
    Serial.printf("Error\n");

  }
  free(data);
}

void loop() {}

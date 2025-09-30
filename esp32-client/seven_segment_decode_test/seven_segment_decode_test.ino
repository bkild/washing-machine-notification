#include "FS.h"
#include "SD_MMC.h"
#include "esp_heap_caps.h"
#include <TJpg_Decoder.h>
#include "seven_segment_led.h"



void setup() {
  Serial.begin(115200);
  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tjpg_output);
  if (!psramFound()) {
    Serial.println("PSRAM not found!");
    return;
  }

  if (!SD_MMC.begin("/sdcard", true)) {  // 1-bit 모드
    Serial.println("Card Mount Failed");
    return;
  }

  File file = SD_MMC.open("/sample1.jpg", "r");
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

  memset(segment_led_state, 0, sizeof(segment_led_state));
  TJpgDec.drawJpg(0, 0, data, fileSize);
  Serial.println(off_led_power);
  for (int k = 0; k < 4; k++) {
    for (int idx = 0; idx < 8; idx++) {
      Serial.print(segment_led_power[k][idx]);
      Serial.print(" ");
    }
    Serial.print("\n");
  }
  for (int k = 0; k < 4; k++) {
    for (int idx = 0; idx < 8; idx++) {
      segment_led_state[k][idx] = segment_led_power[k][idx] >= 2 * off_led_power;
    }
  }
  print7segment(segment_led_state[0]);
  print7segment(segment_led_state[1]);
  print7segment(segment_led_state[2]);
  print7segment(segment_led_state[3]);
  // 필요하면 free(data); 호출
}

void loop() {}

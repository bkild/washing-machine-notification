#include "FS.h"
#include "SD_MMC.h"
#include "esp_heap_caps.h"
#include <TJpg_Decoder.h>
#include "seven_segment_led.h"

// JPEG 메모리 버퍼에서 디코딩 (전체 변환 아님)
bool segment_led_state[4][8];
bool tjpg_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  // (x,y)~(x+w,y+h) 블록 단위 픽셀이 bitmap으로 들어옴
  // 여기서 특정 좌표의 RGB565 값을 추출 가능
  // Serial.printf("%d %d %d %d", x, y, w, h);
  for (int k = 0; k < 4; k++) {
    for (int idx = 0; idx < 8; idx++) {
      if (k == 0 && idx != 1 && idx != 2) {
        continue;
      }
      int px = seg_pixel_pos[k][idx][0];
      int py = seg_pixel_pos[k][idx][1];
      int local_x = px - x;
      int local_y = py - y;
      if (0 <= local_x && local_x < w && 0 <= local_y && local_y < h) {
        uint16_t color = bitmap[local_y * w + local_x];
        uint8_t r = ((color >> 11) & 0x1F) << 3;
        uint8_t g = ((color >> 5) & 0x3F) << 2;
        uint8_t b = (color & 0x1F) << 3;
        segment_led_state[k][idx] = abs(r - TARGET_R) <= 40 && abs(g - TARGET_G) <= 40 && abs(b - TARGET_B) <= 40;
        Serial.printf("%d %d %d\n", abs(r - TARGET_R), abs(g - TARGET_G), abs(b - TARGET_B));
        Serial.printf("%d %d %d\n\n", r, g, b);
      }
    }
  }

  return true;  // 계속 디코딩
}
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
  print7segment(segment_led_state[0]);
  print7segment(segment_led_state[1]);
  print7segment(segment_led_state[2]);
  print7segment(segment_led_state[3]);
  // 필요하면 free(data); 호출
}

void loop() {}

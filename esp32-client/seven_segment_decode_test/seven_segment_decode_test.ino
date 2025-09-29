#include "FS.h"
#include "SD_MMC.h"
#include "esp_heap_caps.h"
#include <TJpg_Decoder.h>
#include "seven_segment_led.h"

// JPEG 메모리 버퍼에서 디코딩 (전체 변환 아님)
int off_led_power = 0;
int segment_led_power[4][8];
int segment_led_cnt[4][8];
bool segment_led_state[4][8];
int len_led_area = 1;
int ret_array[4];

bool calc_cross_area(int x, int y, int w, int h, int x2, int y2, int w2, int h2) {
  x2-=w2;
  y2-=h2;
  w2=2*w2+1;
  h2=2*h2+1;
  if (x2 < 0 || y2 < 0) {
    return false;
  }
  if (x2 + w2 - 1 < x || x + w - 1 < x2 || y2 + h2 - 1 < y || y + h - 1 < y2) {
    return false;
  }
  ret_array[0] = x;
  ret_array[1] = y;
  ret_array[2] = x + w - 1;
  ret_array[3] = y + h - 1;
  if (x < x2) {
    ret_array[0] = x2;
  }
  if (x2 + w2 - 1 < x + w - 1) {
    ret_array[2] = x2 + w2 - 1;
  }
  if (y < y2) {
    ret_array[1] = y2;
  }
  if (y2 + h2 - 1 < y + h - 1) {
    ret_array[3] = y2 + h2 - 1;
  }
  ret_array[0] -= x;
  ret_array[1] -= y;
  ret_array[2] -= x;
  ret_array[3] -= y;
  return true;
}
bool tjpg_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  // (x,y)~(x+w,y+h) 블록 단위 픽셀이 bitmap으로 들어옴
  // 여기서 특정 좌표의 RGB565 값을 추출 가능
  // Serial.printf("%d %d %d %d", x, y, w, h);
  if (calc_cross_area(x, y, w, h, off_led_pos[0], off_led_pos[1], len_led_area, len_led_area)) {
    for (int cur_x = ret_array[0]; cur_x <= ret_array[2]; cur_x++) {
      for (int cur_y = ret_array[1]; cur_y <= ret_array[3]; cur_y++) {
        uint16_t color = bitmap[cur_y * w + cur_x];
        uint8_t r = ((color >> 11) & 0x1F) << 3;
        uint8_t g = ((color >> 5) & 0x3F) << 2;
        uint8_t b = (color & 0x1F) << 3;
        off_led_power += r;
        off_led_power += g;
        off_led_power += b;
      }
    }
  }
  for (int k = 0; k < 4; k++) {
    for (int idx = 0; idx < 8; idx++) {
      if (k == 0 && idx != 1 && idx != 2) {
        continue;
      }
      int px = seg_pixel_pos[k][idx][0];
      int py = seg_pixel_pos[k][idx][1];
      if (calc_cross_area(x, y, w, h, seg_pixel_pos[k][idx][0], seg_pixel_pos[k][idx][1], len_led_area, len_led_area)) {
        for (int cur_x = ret_array[0]; cur_x <= ret_array[2]; cur_x++) {
          for (int cur_y = ret_array[1]; cur_y <= ret_array[3]; cur_y++) {
            uint16_t color = bitmap[cur_y * w + cur_x];
            uint8_t r = ((color >> 11) & 0x1F) << 3;
            uint8_t g = ((color >> 5) & 0x3F) << 2;
            uint8_t b = (color & 0x1F) << 3;
            segment_led_power[k][idx] += r;
            segment_led_power[k][idx] += g;
            segment_led_power[k][idx] += b;
            segment_led_cnt[k][idx]++;
          }
        }
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
      segment_led_state[k][idx]=segment_led_power[k][idx]>=2*off_led_power;
    }
  }
  print7segment(segment_led_state[0]);
  print7segment(segment_led_state[1]);
  print7segment(segment_led_state[2]);
  print7segment(segment_led_state[3]);
  // 필요하면 free(data); 호출
}

void loop() {}

#include "seven_segment_led.h";
void setup() {
  Serial.begin(115200);
  seven_segment_init(1);
}

int min_val = 60;
int tmpCnt = 0;
void loop() {
  if (min_val == 0) {
    show_err();
  } else {
    show_min(min_val);
    tmpCnt++;
    if (tmpCnt == 50) {
      min_val--;
      tmpCnt = 0;

      Serial.printf("%d\n", min_val);
    }
  }



  // cnt++;
}

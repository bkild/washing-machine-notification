#include "seven_segment_led.h";
int digit[] = { 18, 19, 22, 23 };
int segment[] = { 12, 13, 14, 25, 26, 27, 32, 33 };
void setup() {
  Serial.begin(115200);
  seven_segment_init();
  for (int i = 0; i < 8; i++) {
    pinMode(segment[i], OUTPUT);
  }

  for (int i = 0; i < 4; i++) {
    pinMode(digit[i], OUTPUT);
  }
}

int min_val = 60;
int tmpCnt = 0;
void loop() {
  if (min_val == 0) {
    show_end();
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
void show_min(int min_val) {
  for (int i = 3; i >= 0; i--) {
    char c = min_val % 10 + '0';
    show_char(i, c);
    delay(5);
    min_val /= 10;
  }
}
void show_end() {
  show_char(1, 'E');
  delay(5);
  show_char(2, 'N');
  delay(5);
  show_char(3, 'D');
  delay(5);
}
void show_char(int pos, char c) {
  for (int i = 0; i < 4; i++) {
    if (i == pos) {
      digitalWrite(digit[i], LOW);
    } else {
      digitalWrite(digit[i], HIGH);
    }
  }
  for (int i = 0; i < 8; i++) {
    byte segment_data = (char_to_seven_segment_idx[c] & (0x01 << i)) >> i;
    if (segment_data == 1) {
      digitalWrite(segment[7 - i], HIGH);
    } else {
      digitalWrite(segment[7 - i], LOW);
    }
  }
}
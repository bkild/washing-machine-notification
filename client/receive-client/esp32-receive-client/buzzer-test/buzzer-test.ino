#include "little_star.h"
void setup()
{
  little_star_init();
}

void loop()
{
  little_star_play();
  delay(5000); // 5초 대기 후 다시 반복 (세탁기 완료 알림이라면 이 부분을 제거하거나, delay(9999999)로 긴 대기를 줍니다)
}
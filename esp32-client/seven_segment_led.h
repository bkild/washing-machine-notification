#include <stdint.h>
#ifndef SEVEN_SEGMENT_LED
#define SEVEN_SEGMENT_LED
#include <stdio.h>
extern char seven_segment_idx_to_char[256];
extern short char_to_seven_segment_idx[256];
extern int off_led_pos[2];
extern int seg_pixel_pos[4][8][2];
extern void seven_segment_init();

// JPEG 메모리 버퍼에서 디코딩 (전체 변환 아님)
extern int off_led_power;
extern int segment_led_power[4][8];
extern int segment_led_cnt[4][8];
extern bool segment_led_state[4][8];
extern int len_led_area;
extern int ret_array[4];

bool calc_cross_area(int x, int y, int w, int h, int x2, int y2, int w2, int h2);
bool tjpg_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);
struct STIME{
  int hour;
  int min;
};
STIME getTime(uint8_t *data,uint32_t size);
void print7segment(char c);
void print7segment(bool is_on[]);

#endif
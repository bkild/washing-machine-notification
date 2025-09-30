extern "C++"
#include "seven_segment_led.h"
#include <Arduino.h>
#include <stdio.h>
  char seven_segment_idx_to_char[256];
short char_to_seven_segment_idx[256];
void seven_segment_idx_to_char_init();
int off_led_pos[2] = { 315, 320 };
int seg_pixel_pos[4][8][2] = {
	-1, -1, 327, 322, 326, 329, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	334, 318, 338, 322, 336, 330, 332, 333, 329, 330, 331, 321, 333, 326, -1, -1,
	349, 319, 353, 323, 351, 330, 347, 333, 345, 330, 345, 323, 348, 326, -1, -1,
	359, 320, 363, 323, 361, 331, 357, 334, 354, 330, 355, 323, 358, 327, -1, -1
};
int off_led_power = 0;
int segment_led_power[4][8];
int segment_led_cnt[4][8];
bool segment_led_state[4][8];
int len_led_area = 1;
int ret_array[4];
void seven_segment_idx_to_char_init() {
	//a,b,c,d,e,f,g,dp
	seven_segment_idx_to_char[0b1111'1100] = '0';
	seven_segment_idx_to_char[0b0110'0000] = '1';
	seven_segment_idx_to_char[0b1101'1010] = '2';

	seven_segment_idx_to_char[0b1111'0010] = '3';
	seven_segment_idx_to_char[0b0110'0110] = '4';
	seven_segment_idx_to_char[0b1011'0110] = '5';

	seven_segment_idx_to_char[0b1011'1110] = '6';
	seven_segment_idx_to_char[0b1110'0000] = '7';
	seven_segment_idx_to_char[0b1111'1110] = '8';

	seven_segment_idx_to_char[0b1111'0110] = '9';

	char_to_seven_segment_idx['0'] = 0b1111'1100;
	char_to_seven_segment_idx['1'] = 0b0110'0000;
	char_to_seven_segment_idx['2'] = 0b1101'1010;

	char_to_seven_segment_idx['3'] = 0b1111'0010;
	char_to_seven_segment_idx['4'] = 0b0110'0110;
	char_to_seven_segment_idx['5'] = 0b1011'0110;

	char_to_seven_segment_idx['6'] = 0b1011'1110;
	char_to_seven_segment_idx['7'] = 0b1110'0000;
	char_to_seven_segment_idx['8'] = 0b1111'1110;

	char_to_seven_segment_idx['9'] = 0b1111'0110;

	// _
	//| |
	//|_|
	//0b1111'1100
	//
	//  |
	//  |
	//0b0110'0000
	// _
	// _|
	//|_
	//0b1101'1010
	// _
	// _|
	// _|
	//0b1111'0010
	//
	//|_|
	//  |
	//0b0110'0110
	// _
	//|_
	// _|
	//0b1011'0110
	// _
	//|_
	//|_|
	//0b1011'1110
	// _
	//| |
	//  |
	//0b1110'0000
	// _
	//|_|
	//|_|
	//0b1111'1110
	// _
	//|_|
	//  |
	//0b1111'0110
}

bool calc_cross_area(int x, int y, int w, int h, int x2, int y2, int w2, int h2) {
	x2 -= w2;
	y2 -= h2;
	w2 = 2 * w2 + 1;
	h2 = 2 * h2 + 1;
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

void print7segment(char c) {
	// 0
	//561
	//432 7
	short idx = char_to_seven_segment_idx[c];
	bool is_on[8];
	for (size_t i = 0; i < 8; i++) {
		is_on[i] = (idx & (1 << (7 - i))) != 0;
	}
	printf(" %c \n", (is_on[0] ? '_' : ' '));
	printf("%c%c%c\n", (is_on[5] ? '|' : ' '), (is_on[6] ? '_' : ' '), (is_on[1] ? '|' : ' '));
	printf("%c%c%c %c\n", (is_on[4] ? '|' : ' '), (is_on[3] ? '_' : ' '), (is_on[2] ? '|' : ' '), (is_on[7] ? '.' : ' '));
}
void print7segment(bool is_on[]) {
	// 0
	//561
	//432 7
	Serial.printf(" %c \n", (is_on[0] ? '_' : ' '));
	Serial.printf("%c%c%c\n", (is_on[5] ? '|' : ' '), (is_on[6] ? '_' : ' '), (is_on[1] ? '|' : ' '));
	Serial.printf("%c%c%c %c\n", (is_on[4] ? '|' : ' '), (is_on[3] ? '_' : ' '), (is_on[2] ? '|' : ' '), (is_on[7] ? '.' : ' '));
}
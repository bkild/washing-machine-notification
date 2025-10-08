#include <stdint.h>

#include "seven_segment_led.h"
#include <Arduino.h>
#include <stdio.h>
#include <TJpg_Decoder.h>
#include <string.h>

char seven_segment_idx_to_char[256];
short char_to_seven_segment_idx[256];
int digit_pin[] = { 18, 19, 22, 23 };
int segment_pin[] = { 12, 13, 14, 25, 26, 27, 32, 33 };
void seven_segment_init() {
	TJpgDec.setJpgScale(1);
	TJpgDec.setCallback(tjpg_output);
	// a,b,c,d,e,f,g,dp
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
	seven_segment_idx_to_char[0b1001'1110] = 'E';
	seven_segment_idx_to_char[0b0010'1010] = 'N';
	seven_segment_idx_to_char[0b0111'1010] = 'D';
	seven_segment_idx_to_char[0b0000'1010] = 'r';

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
	char_to_seven_segment_idx['E'] = 0b1001'1110;
	char_to_seven_segment_idx['N'] = 0b0010'1010;
	char_to_seven_segment_idx['D'] = 0b0111'1010;
	char_to_seven_segment_idx['r'] = 0b0000'1010;

	// _
	//| |
	//|_|
	// 0b1111'1100
	//
	//  |
	//  |
	// 0b0110'0000
	// _
	// _|
	//|_
	// 0b1101'1010
	// _
	// _|
	// _|
	// 0b1111'0010
	//
	//|_|
	//  |
	// 0b0110'0110
	// _
	//|_
	// _|
	// 0b1011'0110
	// _
	//|_
	//|_|
	// 0b1011'1110
	// _
	//| |
	//  |
	// 0b1110'0000
	// _
	//|_|
	//|_|
	// 0b1111'1110
	// _
	//|_|
	//  |
	// 0b1111'0110
}
void seven_segment_init(int out) {
	seven_segment_init();
	if (out) {
		for (int i = 0; i < 8; i++) {
			pinMode(segment_pin[i], OUTPUT);
		}

		for (int i = 0; i < 4; i++) {
			pinMode(digit_pin[i], OUTPUT);
		}
	}
}
int off_led_pos[2] = { 200, 335 };
int seg_pixel_pos[4][8][2] = {
	-1, -1,
	236, 332,
	246, 405,
	-1, -1,
	-1, -1,
	-1, -1,
	-1, -1,
	-1, -1,

	257, 285,
	279, 311,
	290, 388,
	278, 430,
	258, 399,
	248, 328,
	268, 355,
	-1, -1,

	341, 227,
	375, 264,
	393, 362,
	372, 412,
	337, 376,
	322, 287,
	357, 317,
	-1, -1,

	429, 166,
	485, 203,
	514, 332,
	478, 399,
	427, 351,
	407, 242,
	452, 281,
	-1, -1
};
int off_led_power = 0;
int segment_led_power[4][8];
int segment_led_cnt[4][8];
bool segment_led_state[4][8];
int len_led_area = 1;
int ret_array[4];

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

uint8_t tm[4];
int getTime(uint8_t *data, uint32_t fileSize) {
	memset(segment_led_power, 0, sizeof(segment_led_power));
	off_led_power = 0;
	TJpgDec.drawJpg(0, 0, data, fileSize);
	// Serial.println(off_led_power);

	for (int k = 0; k < 4; k++) {
		for (int idx = 0; idx < 8; idx++) {
			// Serial.printf("%d ", segment_led_power[k][idx]);
			segment_led_state[k][idx] = segment_led_power[k][idx] >= 1.5 * off_led_power;
		}
		// Serial.println();
	}
	for (int k = 0; k < 4; k++) {
		int bit = 0;
		for (int idx = 0; idx < 8; idx++) {
			bit <<= 1;
			bit |= segment_led_state[k][idx];
		}
		tm[k] = seven_segment_idx_to_char[bit];
	}
	int min_val;
	if (tm[1] == 'E' && tm[2] == 'N' && tm[3] == 'D') {
		min_val = 0;
		return min_val;
	}
	// Serial.println(tm[0]);
	// Serial.println(tm[1]);
	// Serial.println(tm[2]);
	// Serial.println(tm[3]);

	if (tm[0] != 0 && tm[1] != 0) {
		min_val = ((tm[0] - '0') * 10 + (tm[1] - '0')) * 60;
	} else if (tm[0] == 0 && tm[1] != 0) {
		min_val = (tm[1] - '0') * 60;
	} else {
		min_val = 0;
	}

	if (tm[2] == 0 || tm[3] == 0) {
		min_val = -1;
	} else {
		min_val += (tm[2] - '0') * 10 + (tm[3] - '0');
	}
	// print7segment(segment_led_state[0]);
	// print7segment(segment_led_state[1]);
	// print7segment(segment_led_state[2]);
	// print7segment(segment_led_state[3]);
	return min_val;
}

void print7segment(char c) {
	// 0
	// 561
	// 432 7
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
	// 561
	// 432 7
	Serial.printf(" %c \n", (is_on[0] ? '_' : ' '));
	Serial.printf("%c%c%c\n", (is_on[5] ? '|' : ' '), (is_on[6] ? '_' : ' '), (is_on[1] ? '|' : ' '));
	Serial.printf("%c%c%c %c\n", (is_on[4] ? '|' : ' '), (is_on[3] ? '_' : ' '), (is_on[2] ? '|' : ' '), (is_on[7] ? '.' : ' '));
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
void show_err() {
	show_char(1, 'E');
	delay(5);
	show_char(2, 'r');
	delay(5);
	show_char(3, 'r');
	delay(5);
}
void show_off() {
	for (int i = 0; i < 4; i++) {
		digitalWrite(digit_pin[i], HIGH);
	}
}
void show_char(int pos, char c) {
	for (int i = 0; i < 4; i++) {
		if (i == pos) {
			digitalWrite(digit_pin[i], LOW);
		} else {
			digitalWrite(digit_pin[i], HIGH);
		}
	}
	for (int i = 0; i < 8; i++) {
		byte segment_data = (char_to_seven_segment_idx[c] & (0x01 << i)) >> i;
		if (segment_data == 1) {
			digitalWrite(segment_pin[7 - i], HIGH);
		} else {
			digitalWrite(segment_pin[7 - i], LOW);
		}
	}
}
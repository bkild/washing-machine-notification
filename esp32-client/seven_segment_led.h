#ifndef SEVEN_SEGMENT_LED
#define SEVEN_SEGMENT_LED
#include<stdio.h>
char seven_segment_idx_to_char[256];
short char_to_seven_segment_idx[256];
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


}

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
void print7segment(char c) {
	// 0
	//561
	//432 7
	short idx = char_to_seven_segment_idx[c];
	bool is_on[8];
	for (size_t i = 0; i < 8; i++)
	{
		is_on[i] = (idx & (1 << (7 - i))) != 0;
	}
	printf(" %c \n",(is_on[0] ?'_':' '));
	printf("%c%c%c\n", (is_on[5] ? '|' : ' '), (is_on[6] ? '_' : ' '), (is_on[1] ? '|' : ' '));
	printf("%c%c%c %c\n", (is_on[4] ? '|' : ' '), (is_on[3] ? '_' : ' '), (is_on[2] ? '|' : ' '), (is_on[7] ? '.' : ' '));
}

#endif
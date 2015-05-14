
#include "color.h"
#include <stdlib.h>
#include <stdio.h>

/* Bonus points for time wasted on C golf for improving the efficiency of this part */
int xterm_color_index(color_t c){
	int candidate = 0;
	int best_distance = 0x7fffffff;
	for(int i=0; i<256; i++){
		/* NOTE: this ignores the alpha channel */
		color_t k = colortable[i];
		int tmp = abs(c.r - k.r);
		tmp *= tmp;
		int distance = tmp;
		if(distance > best_distance)
			continue;
		tmp = abs(c.g - k.g);
		tmp *= tmp;
		distance += tmp;
		if(distance > best_distance)
			continue;
		tmp = abs(c.b - k.b);
		tmp *= tmp;
		distance += tmp;
		if(distance > best_distance)
			continue;
		best_distance = distance;
		candidate = i;
	}
	return candidate;
}

/* Generated by genpal.py */
color_t colortable[256] = {
	/* 16 default ANSI colors */
	{0x00, 0x00, 0x00, 0xff},
	{0xa8, 0x00, 0x00, 0xff},
	{0x00, 0xa8, 0x00, 0xff},
	{0xa8, 0x54, 0x00, 0xff},
	{0x00, 0x00, 0xa8, 0xff},
	{0xa8, 0x00, 0xa8, 0xff},
	{0x00, 0xa8, 0xa8, 0xff},
	{0xa8, 0xa8, 0xa8, 0xff},
	{0x54, 0x54, 0x54, 0xff},
	{0xfc, 0x54, 0x54, 0xff},
	{0x54, 0xfc, 0x54, 0xff},
	{0xfc, 0xfc, 0x54, 0xff},
	{0x54, 0x54, 0xfc, 0xff},
	{0xfc, 0x54, 0xfc, 0xff},
	{0x54, 0xfc, 0xfc, 0xff},
	{0xfc, 0xfc, 0xfc, 0xff},
	/* 6x6x6 color cube */
	{0x00, 0x00, 0x00, 0xff},
	{0x00, 0x00, 0x5f, 0xff},
	{0x00, 0x00, 0x87, 0xff},
	{0x00, 0x00, 0xaf, 0xff},
	{0x00, 0x00, 0xd7, 0xff},
	{0x00, 0x00, 0xff, 0xff},
	{0x00, 0x5f, 0x00, 0xff},
	{0x00, 0x5f, 0x5f, 0xff},
	{0x00, 0x5f, 0x87, 0xff},
	{0x00, 0x5f, 0xaf, 0xff},
	{0x00, 0x5f, 0xd7, 0xff},
	{0x00, 0x5f, 0xff, 0xff},
	{0x00, 0x87, 0x00, 0xff},
	{0x00, 0x87, 0x5f, 0xff},
	{0x00, 0x87, 0x87, 0xff},
	{0x00, 0x87, 0xaf, 0xff},
	{0x00, 0x87, 0xd7, 0xff},
	{0x00, 0x87, 0xff, 0xff},
	{0x00, 0xaf, 0x00, 0xff},
	{0x00, 0xaf, 0x5f, 0xff},
	{0x00, 0xaf, 0x87, 0xff},
	{0x00, 0xaf, 0xaf, 0xff},
	{0x00, 0xaf, 0xd7, 0xff},
	{0x00, 0xaf, 0xff, 0xff},
	{0x00, 0xd7, 0x00, 0xff},
	{0x00, 0xd7, 0x5f, 0xff},
	{0x00, 0xd7, 0x87, 0xff},
	{0x00, 0xd7, 0xaf, 0xff},
	{0x00, 0xd7, 0xd7, 0xff},
	{0x00, 0xd7, 0xff, 0xff},
	{0x00, 0xff, 0x00, 0xff},
	{0x00, 0xff, 0x5f, 0xff},
	{0x00, 0xff, 0x87, 0xff},
	{0x00, 0xff, 0xaf, 0xff},
	{0x00, 0xff, 0xd7, 0xff},
	{0x00, 0xff, 0xff, 0xff},
	{0x5f, 0x00, 0x00, 0xff},
	{0x5f, 0x00, 0x5f, 0xff},
	{0x5f, 0x00, 0x87, 0xff},
	{0x5f, 0x00, 0xaf, 0xff},
	{0x5f, 0x00, 0xd7, 0xff},
	{0x5f, 0x00, 0xff, 0xff},
	{0x5f, 0x5f, 0x00, 0xff},
	{0x5f, 0x5f, 0x5f, 0xff},
	{0x5f, 0x5f, 0x87, 0xff},
	{0x5f, 0x5f, 0xaf, 0xff},
	{0x5f, 0x5f, 0xd7, 0xff},
	{0x5f, 0x5f, 0xff, 0xff},
	{0x5f, 0x87, 0x00, 0xff},
	{0x5f, 0x87, 0x5f, 0xff},
	{0x5f, 0x87, 0x87, 0xff},
	{0x5f, 0x87, 0xaf, 0xff},
	{0x5f, 0x87, 0xd7, 0xff},
	{0x5f, 0x87, 0xff, 0xff},
	{0x5f, 0xaf, 0x00, 0xff},
	{0x5f, 0xaf, 0x5f, 0xff},
	{0x5f, 0xaf, 0x87, 0xff},
	{0x5f, 0xaf, 0xaf, 0xff},
	{0x5f, 0xaf, 0xd7, 0xff},
	{0x5f, 0xaf, 0xff, 0xff},
	{0x5f, 0xd7, 0x00, 0xff},
	{0x5f, 0xd7, 0x5f, 0xff},
	{0x5f, 0xd7, 0x87, 0xff},
	{0x5f, 0xd7, 0xaf, 0xff},
	{0x5f, 0xd7, 0xd7, 0xff},
	{0x5f, 0xd7, 0xff, 0xff},
	{0x5f, 0xff, 0x00, 0xff},
	{0x5f, 0xff, 0x5f, 0xff},
	{0x5f, 0xff, 0x87, 0xff},
	{0x5f, 0xff, 0xaf, 0xff},
	{0x5f, 0xff, 0xd7, 0xff},
	{0x5f, 0xff, 0xff, 0xff},
	{0x87, 0x00, 0x00, 0xff},
	{0x87, 0x00, 0x5f, 0xff},
	{0x87, 0x00, 0x87, 0xff},
	{0x87, 0x00, 0xaf, 0xff},
	{0x87, 0x00, 0xd7, 0xff},
	{0x87, 0x00, 0xff, 0xff},
	{0x87, 0x5f, 0x00, 0xff},
	{0x87, 0x5f, 0x5f, 0xff},
	{0x87, 0x5f, 0x87, 0xff},
	{0x87, 0x5f, 0xaf, 0xff},
	{0x87, 0x5f, 0xd7, 0xff},
	{0x87, 0x5f, 0xff, 0xff},
	{0x87, 0x87, 0x00, 0xff},
	{0x87, 0x87, 0x5f, 0xff},
	{0x87, 0x87, 0x87, 0xff},
	{0x87, 0x87, 0xaf, 0xff},
	{0x87, 0x87, 0xd7, 0xff},
	{0x87, 0x87, 0xff, 0xff},
	{0x87, 0xaf, 0x00, 0xff},
	{0x87, 0xaf, 0x5f, 0xff},
	{0x87, 0xaf, 0x87, 0xff},
	{0x87, 0xaf, 0xaf, 0xff},
	{0x87, 0xaf, 0xd7, 0xff},
	{0x87, 0xaf, 0xff, 0xff},
	{0x87, 0xd7, 0x00, 0xff},
	{0x87, 0xd7, 0x5f, 0xff},
	{0x87, 0xd7, 0x87, 0xff},
	{0x87, 0xd7, 0xaf, 0xff},
	{0x87, 0xd7, 0xd7, 0xff},
	{0x87, 0xd7, 0xff, 0xff},
	{0x87, 0xff, 0x00, 0xff},
	{0x87, 0xff, 0x5f, 0xff},
	{0x87, 0xff, 0x87, 0xff},
	{0x87, 0xff, 0xaf, 0xff},
	{0x87, 0xff, 0xd7, 0xff},
	{0x87, 0xff, 0xff, 0xff},
	{0xaf, 0x00, 0x00, 0xff},
	{0xaf, 0x00, 0x5f, 0xff},
	{0xaf, 0x00, 0x87, 0xff},
	{0xaf, 0x00, 0xaf, 0xff},
	{0xaf, 0x00, 0xd7, 0xff},
	{0xaf, 0x00, 0xff, 0xff},
	{0xaf, 0x5f, 0x00, 0xff},
	{0xaf, 0x5f, 0x5f, 0xff},
	{0xaf, 0x5f, 0x87, 0xff},
	{0xaf, 0x5f, 0xaf, 0xff},
	{0xaf, 0x5f, 0xd7, 0xff},
	{0xaf, 0x5f, 0xff, 0xff},
	{0xaf, 0x87, 0x00, 0xff},
	{0xaf, 0x87, 0x5f, 0xff},
	{0xaf, 0x87, 0x87, 0xff},
	{0xaf, 0x87, 0xaf, 0xff},
	{0xaf, 0x87, 0xd7, 0xff},
	{0xaf, 0x87, 0xff, 0xff},
	{0xaf, 0xaf, 0x00, 0xff},
	{0xaf, 0xaf, 0x5f, 0xff},
	{0xaf, 0xaf, 0x87, 0xff},
	{0xaf, 0xaf, 0xaf, 0xff},
	{0xaf, 0xaf, 0xd7, 0xff},
	{0xaf, 0xaf, 0xff, 0xff},
	{0xaf, 0xd7, 0x00, 0xff},
	{0xaf, 0xd7, 0x5f, 0xff},
	{0xaf, 0xd7, 0x87, 0xff},
	{0xaf, 0xd7, 0xaf, 0xff},
	{0xaf, 0xd7, 0xd7, 0xff},
	{0xaf, 0xd7, 0xff, 0xff},
	{0xaf, 0xff, 0x00, 0xff},
	{0xaf, 0xff, 0x5f, 0xff},
	{0xaf, 0xff, 0x87, 0xff},
	{0xaf, 0xff, 0xaf, 0xff},
	{0xaf, 0xff, 0xd7, 0xff},
	{0xaf, 0xff, 0xff, 0xff},
	{0xd7, 0x00, 0x00, 0xff},
	{0xd7, 0x00, 0x5f, 0xff},
	{0xd7, 0x00, 0x87, 0xff},
	{0xd7, 0x00, 0xaf, 0xff},
	{0xd7, 0x00, 0xd7, 0xff},
	{0xd7, 0x00, 0xff, 0xff},
	{0xd7, 0x5f, 0x00, 0xff},
	{0xd7, 0x5f, 0x5f, 0xff},
	{0xd7, 0x5f, 0x87, 0xff},
	{0xd7, 0x5f, 0xaf, 0xff},
	{0xd7, 0x5f, 0xd7, 0xff},
	{0xd7, 0x5f, 0xff, 0xff},
	{0xd7, 0x87, 0x00, 0xff},
	{0xd7, 0x87, 0x5f, 0xff},
	{0xd7, 0x87, 0x87, 0xff},
	{0xd7, 0x87, 0xaf, 0xff},
	{0xd7, 0x87, 0xd7, 0xff},
	{0xd7, 0x87, 0xff, 0xff},
	{0xd7, 0xaf, 0x00, 0xff},
	{0xd7, 0xaf, 0x5f, 0xff},
	{0xd7, 0xaf, 0x87, 0xff},
	{0xd7, 0xaf, 0xaf, 0xff},
	{0xd7, 0xaf, 0xd7, 0xff},
	{0xd7, 0xaf, 0xff, 0xff},
	{0xd7, 0xd7, 0x00, 0xff},
	{0xd7, 0xd7, 0x5f, 0xff},
	{0xd7, 0xd7, 0x87, 0xff},
	{0xd7, 0xd7, 0xaf, 0xff},
	{0xd7, 0xd7, 0xd7, 0xff},
	{0xd7, 0xd7, 0xff, 0xff},
	{0xd7, 0xff, 0x00, 0xff},
	{0xd7, 0xff, 0x5f, 0xff},
	{0xd7, 0xff, 0x87, 0xff},
	{0xd7, 0xff, 0xaf, 0xff},
	{0xd7, 0xff, 0xd7, 0xff},
	{0xd7, 0xff, 0xff, 0xff},
	{0xff, 0x00, 0x00, 0xff},
	{0xff, 0x00, 0x5f, 0xff},
	{0xff, 0x00, 0x87, 0xff},
	{0xff, 0x00, 0xaf, 0xff},
	{0xff, 0x00, 0xd7, 0xff},
	{0xff, 0x00, 0xff, 0xff},
	{0xff, 0x5f, 0x00, 0xff},
	{0xff, 0x5f, 0x5f, 0xff},
	{0xff, 0x5f, 0x87, 0xff},
	{0xff, 0x5f, 0xaf, 0xff},
	{0xff, 0x5f, 0xd7, 0xff},
	{0xff, 0x5f, 0xff, 0xff},
	{0xff, 0x87, 0x00, 0xff},
	{0xff, 0x87, 0x5f, 0xff},
	{0xff, 0x87, 0x87, 0xff},
	{0xff, 0x87, 0xaf, 0xff},
	{0xff, 0x87, 0xd7, 0xff},
	{0xff, 0x87, 0xff, 0xff},
	{0xff, 0xaf, 0x00, 0xff},
	{0xff, 0xaf, 0x5f, 0xff},
	{0xff, 0xaf, 0x87, 0xff},
	{0xff, 0xaf, 0xaf, 0xff},
	{0xff, 0xaf, 0xd7, 0xff},
	{0xff, 0xaf, 0xff, 0xff},
	{0xff, 0xd7, 0x00, 0xff},
	{0xff, 0xd7, 0x5f, 0xff},
	{0xff, 0xd7, 0x87, 0xff},
	{0xff, 0xd7, 0xaf, 0xff},
	{0xff, 0xd7, 0xd7, 0xff},
	{0xff, 0xd7, 0xff, 0xff},
	{0xff, 0xff, 0x00, 0xff},
	{0xff, 0xff, 0x5f, 0xff},
	{0xff, 0xff, 0x87, 0xff},
	{0xff, 0xff, 0xaf, 0xff},
	{0xff, 0xff, 0xd7, 0xff},
	{0xff, 0xff, 0xff, 0xff},
	/* 24-Grayscale slide */
	{0x00, 0x00, 0x00, 0xff},
	{0x12, 0x12, 0x12, 0xff},
	{0x1c, 0x1c, 0x1c, 0xff},
	{0x26, 0x26, 0x26, 0xff},
	{0x30, 0x30, 0x30, 0xff},
	{0x3a, 0x3a, 0x3a, 0xff},
	{0x44, 0x44, 0x44, 0xff},
	{0x4e, 0x4e, 0x4e, 0xff},
	{0x58, 0x58, 0x58, 0xff},
	{0x62, 0x62, 0x62, 0xff},
	{0x6c, 0x6c, 0x6c, 0xff},
	{0x76, 0x76, 0x76, 0xff},
	{0x80, 0x80, 0x80, 0xff},
	{0x8a, 0x8a, 0x8a, 0xff},
	{0x94, 0x94, 0x94, 0xff},
	{0x9e, 0x9e, 0x9e, 0xff},
	{0xa8, 0xa8, 0xa8, 0xff},
	{0xb2, 0xb2, 0xb2, 0xff},
	{0xbc, 0xbc, 0xbc, 0xff},
	{0xc6, 0xc6, 0xc6, 0xff},
	{0xd0, 0xd0, 0xd0, 0xff},
	{0xda, 0xda, 0xda, 0xff},
	{0xe4, 0xe4, 0xe4, 0xff},
	{0xee, 0xee, 0xee, 0xff}
};


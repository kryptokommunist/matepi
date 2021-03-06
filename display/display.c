// gcc -std=c99 -shared -Wl,-soname,display -o display.so -fPIC display.c -lwiringPi

#include <stdio.h>
#include <wiringPiSPI.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>

#define SPI_FREQUENCY   1000000
#define SPI_CHANNEL     1

#define CRATE_WIDTH		  4
#define CRATE_HEIGHT    5
#define CRATES_X		    8
#define CRATES_Y		    3
#define BUS_COUNT		    4
#define BYTES_PER_PIXEL	3 //3 Pixel + possibly 1 Gamma Value
#define CRATES_PER_BUS	6
#define BUS_ROWS		(CRATES_Y*CRATE_HEIGHT)
#define CRATE_COUNT		(CRATES_X*CRATES_Y)
#define CRATE_SIZE		(CRATE_WIDTH*CRATE_HEIGHT)
#define BUS_SIZE		(CRATES_PER_BUS*CRATE_SIZE*BYTES_PER_PIXEL)
#define BOTTLES               (CRATE_COUNT*CRATE_SIZE)
#define BUFF_SIZE                (CRATE_COUNT*CRATE_SIZE*BYTES_PER_PIXEL)
#define BUFF_SIZE_ALPHA                (CRATE_COUNT*CRATE_SIZE*4)

/* Old config
unsigned const char BOTTLE_MAP[CRATE_SIZE] = {
	   10, 15, 16, 11,  6,
	    5,  0,  1,  2,  7,
	   12, 17, 18, 13,  8,
	    3,  4,  9, 14, 19
};
*/

//new config
unsigned const char BOTTLE_MAP[CRATE_SIZE] = {
	   0, 4,  8, 12, 16,
	   17, 13,  9, 5, 1,
	   2, 6, 10, 14, 18,
	   19, 15, 11, 7, 3
};

/*
typedef struct {
	uint8_t r, g, b, a;
} color_t;

typedef struct {
	uint8_t r, g, b;
} rgb_t;
*/


void display(uint8_t data[BUFF_SIZE_ALPHA], float brightness, int alpha);

uint8_t applyGamma(uint8_t pixel, uint8_t gamma, float brightness) {

	return (uint8_t)roundf(powf((pixel/255.0F), gamma) * brightness * 255);

}

void setup() {

        if(wiringPiSPISetup(SPI_CHANNEL, SPI_FREQUENCY) == -1) {

                printf("Could not initialize SPI!\n");

        }

        printf("I just initialized SPI, bitch!\n");

}

/* Takes filename, return buffer containing image data. Length ist BUFF_SIZE*/
void display(uint8_t data[BUFF_SIZE_ALPHA], float brightness, int alpha)
{


if(alpha) {

	for(int i = 1;  i <= BUFF_SIZE_ALPHA; i++){

		if(i % 4 == 0) {

			uint8_t gamma = data[i - 1];

			int offset = roundf(i/4);

			data[i - 2 - offset - 1] = applyGamma(data[i - 3 - 1], gamma, brightness); //red
			data[i - 1 - offset - 1] = applyGamma(data[i - 2 - 1], gamma, brightness); //blue
			data[i - offset - 1] = applyGamma(data[i - 1 - 1], gamma, brightness); //green

		}

	}

}


unsigned char cratesData[CRATE_COUNT][CRATE_SIZE * 3];

for(int crate_x = 0; crate_x < CRATES_X; crate_x++){

	for(int crate_y = 0; crate_y < CRATES_Y; crate_y++){

		for(int bottle_x = 0; bottle_x < CRATE_WIDTH*3; bottle_x++){

			for(int bottle_y = 0; bottle_y < CRATE_HEIGHT; bottle_y++) {

				cratesData[crate_y * CRATES_X + crate_x][bottle_y * CRATE_WIDTH * 3 + bottle_x] = (unsigned char) data[crate_y * CRATE_SIZE*CRATES_X*3 + crate_x * CRATE_WIDTH*3 + bottle_y * CRATE_WIDTH * CRATES_X*3 + bottle_x];

			}

		}

	}

}

unsigned char buffer[BUFF_SIZE];

for(int i = 0; i < CRATE_COUNT; i++){

	for(int j = 0; j < CRATE_SIZE; j++) {

		for(int k = 0; k < 3; k++) {

		   buffer[i*CRATE_SIZE*3+j*3+k] = cratesData[i][BOTTLE_MAP[j]*3+k];

		}

}

}

wiringPiSPIDataRW(SPI_CHANNEL,buffer,BUFF_SIZE);

}

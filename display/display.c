

#include <stdio.h>
#include <wiringPiSPI.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#define SPI_FREQUENCY   1000000
#define SPI_CHANNEL     1

#define CRATE_WIDTH		  5
#define CRATE_HEIGHT    4
#define CRATES_X		    6
#define CRATES_Y		    4
#define BUS_COUNT		    4
#define BYTES_PER_PIXEL	3
#define CRATES_PER_BUS	6
#define BUS_ROWS		(CRATES_Y*CRATE_HEIGHT)
#define CRATE_COUNT		(CRATES_X*CRATES_Y)
#define CRATE_SIZE		(CRATE_WIDTH*CRATE_HEIGHT)
#define BUS_SIZE		(CRATES_PER_BUS*CRATE_SIZE*BYTES_PER_PIXEL)
#define BOTTLES               (CRATE_COUNT*CRATE_SIZE)
#define BUFF_SIZE                (CRATE_COUNT*CRATE_SIZE*BYTES_PER_PIXEL)

unsigned const char BOTTLE_MAP[CRATE_SIZE] = {
	   10, 15,  16, 11, 6, 
	   5, 0,  1, 2, 7, 
	   12, 17, 18, 13, 8, 
	   3, 4, 9, 14, 19
};

/*
typedef struct {
	uint8_t r, g, b, a;
} color_t;

typedef struct {
	uint8_t r, g, b;
} rgb_t;
*/

int spi_initialized = 0;

void display(uint8_t data[BUFF_SIZE], float brightness, int alpha);

uint8_t applyGamma(uint8_t pixel, uint8_t gamma, float brightness) {

	return (uint8_t)roundf(powf((pixel/255.0F), gamma) * brightness * 255);

}

/* Takes filename, return buffer containing image data. Length ist BUFF_SIZE*/
void display(uint8_t data[BUFF_SIZE], float brightness, int alpha)
{

if(!spi_initialized) { /* SPI should only be initialized once at the beginning! */

	if(wiringPiSPISetup(SPI_CHANNEL, SPI_FREQUENCY) == -1) {

	        printf("Could not initialize SPI!\n");

	}

	printf("I just initialized SPI, bitch!\n");

	spi_initialized = -1;
}


unsigned char cratesData[CRATE_COUNT][CRATE_SIZE * 3];

for(int crate_x = 0; crate_x < CRATES_X; crate_x++){

	for(int crate_y = 0; crate_y < CRATES_Y; crate_y++){

		for(int bottle_x = 0; bottle_x < CRATE_WIDTH*3; bottle_x++){

			for(int bottle_y = 0; bottle_y < CRATE_HEIGHT; bottle_y++) {

				if(alpha) {

					if(bottle_x % 3 == 2) {

						u_int8_t red = data[crate_y * CRATE_SIZE*CRATES_X*3 + crate_x * CRATE_WIDTH*3 + bottle_y * CRATE_WIDTH * CRATES_X*3 + bottle_x - 3];
						u_int8_t blue = data[crate_y * CRATE_SIZE*CRATES_X*3 + crate_x * CRATE_WIDTH*3 + bottle_y * CRATE_WIDTH * CRATES_X*3 + bottle_x - 2];
						u_int8_t green = data[crate_y * CRATE_SIZE*CRATES_X*3 + crate_x * CRATE_WIDTH*3 + bottle_y * CRATE_WIDTH * CRATES_X*3 + bottle_x - 1];
						u_int8_t gamma = data[crate_y * CRATE_SIZE*CRATES_X*3 + crate_x * CRATE_WIDTH*3 + bottle_y * CRATE_WIDTH * CRATES_X*3 + bottle_x];

						cratesData[crate_y * CRATES_X + crate_x][bottle_y * CRATE_WIDTH * 3 + bottle_x - 2] = applyGamma(red, gamma, brightness);
						cratesData[crate_y * CRATES_X + crate_x][bottle_y * CRATE_WIDTH * 3 + bottle_x - 1] = applyGamma(blue, gamma, brightness);
						cratesData[crate_y * CRATES_X + crate_x][bottle_y * CRATE_WIDTH * 3 + bottle_x] = applyGamma(green, gamma, brightness);



					}

				} else {

					cratesData[crate_y * CRATES_X + crate_x][bottle_y * CRATE_WIDTH * 3 + bottle_x] = (unsigned char) data[crate_y * CRATE_SIZE*CRATES_X*3 + crate_x * CRATE_WIDTH*3 + bottle_y * CRATE_WIDTH * CRATES_X*3 + bottle_x];

				}

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
sleep(0.1); // give time to display, without this the data somehow doesn't get pushed to the display...

}

#include <stdio.h>
#include <wiringPiSPI.h>
#include "lodepng.h"
#include <stdlib.h>
#include <unistd.h>

#define SPI_FREQUENCY   1000000
#define SPI_CHANNEL     1

#define CRATE_WIDTH     5
#define CRATE_HEIGHT    4
#define CRATES_X        6
#define CRATES_Y        4
#define BUS_COUNT       4
#define BYTES_PER_PIXEL 3
#define CRATES_PER_BUS  6
#define BUS_ROWS    (CRATES_Y*CRATE_HEIGHT)
#define CRATE_COUNT   (CRATES_X*CRATES_Y)
#define CRATE_SIZE    (CRATE_WIDTH*CRATE_HEIGHT)
#define BUS_SIZE    (CRATES_PER_BUS*CRATE_SIZE*BYTES_PER_PIXEL)
#define BUFF_SIZE                (CRATE_COUNT*CRATE_SIZE*BYTES_PER_PIXEL)

unsigned const char BOTTLE_MAP[CRATE_SIZE] = {
     10, 15,  16, 11, 6, 
     5, 0,  1, 2, 7, 
     12, 17, 18, 13, 8, 
     3, 4, 9, 14, 19
};


void decodeOneStep(const char* filename);


int main() {

if(wiringPiSPISetup(SPI_CHANNEL, SPI_FREQUENCY) == -1) {

        printf("Could not initialize SPI!\n");

}

unsigned char buff[BUFF_SIZE];

for(int i = 0; i < BUFF_SIZE ; i++){

buff[i] = 0xFF;

}

printf("Came to the end\n");

wiringPiSPIDataRW(SPI_CHANNEL,buff,BUFF_SIZE);

while(true) {
wiringPiSPIDataRW(SPI_CHANNEL,buff,BUFF_SIZE);
sleep(3);
decodeOneStep("nyancat.png");
sleep(3);
decodeOneStep("rgb.png");
sleep(3);
decodeOneStep("hack.png");
sleep(3);
decodeOneStep("circle.png");
sleep(3);
decodeOneStep("checker.png");
sleep(3);
decodeOneStep("green-checker.png");
sleep(3);
decodeOneStep("red-line.png");
sleep(3);
decodeOneStep("line.png");
sleep(3);
decodeOneStep("test.png");
sleep(3);
decodeOneStep("wiki.png");
sleep(2);
decodeOneStep("chinese.png");
sleep(2);
decodeOneStep("ok.png");
sleep(2);
}

}


void decodeOneStep(const char* filename)
{
  unsigned error;
  unsigned char *image;
  unsigned width, height;

  error = lodepng_decode32_file(&image, &width, &height, filename);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  unsigned char buffer[BUFF_SIZE]= { 0xFF };
  int buffcnt = 0;
  for(int i = 0; i < (CRATE_COUNT * CRATE_SIZE * 4); i++){

  if(i==0 ||i % 4 != 0) {buffer[buffcnt] = image[i]; buffcnt++; /* printf("%x\n", image[i]);*/}

   }

free(image);

unsigned char cratesData[CRATE_COUNT][CRATE_SIZE * 3];

for(int crate_x = 0; crate_x < CRATES_X; crate_x++){

for(int crate_y = 0; crate_y < CRATES_Y; crate_y++){

for(int bottle_x = 0; bottle_x < CRATE_WIDTH*3; bottle_x++){

for(int bottle_y = 0; bottle_y < CRATE_HEIGHT; bottle_y++) {

  cratesData[crate_y * CRATES_X + crate_x][bottle_y * CRATE_WIDTH * 3 + bottle_x] = buffer[crate_y * CRATE_SIZE*CRATES_X*3 + crate_x * CRATE_WIDTH*3 + bottle_y * CRATE_WIDTH * CRATES_X*3 + bottle_x];

}

}

}

}

for(int i = 0; i < CRATE_COUNT; i++){

for(int j = 0; j < CRATE_SIZE; j++) {

for(int k = 0; k < 3; k++) {

   buffer[i*CRATE_SIZE*3+j*3+k] = cratesData[i][BOTTLE_MAP[j]*3+k];
   //printf("%x",  cratesData[i][BOTTLE_MAP[j]*3+k] ); 

}

}

}

  /*use image here*/
  wiringPiSPIDataRW(SPI_CHANNEL,buffer,BUFF_SIZE);
  /*printf("Width %i\n", width);printf("Height %i\n", height);*/

}
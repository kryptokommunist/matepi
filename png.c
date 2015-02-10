#include <stdio.h>
#include <wiringPiSPI.h>
#include "lodepng.h"
#include <stdlib.h>
#include <unistd.h>
const int chann = 1;
const int frequ = 1000000;
const int leds = 480;
const int buffsize = leds * 3;
#define CRATE_WIDTH     5
#define CRATE_HEIGHT    4
#define CRATES_X        6
#define CRATES_Y        4
#define BUS_COUNT       4
#define BYTES_PER_PIXEL 3
#define CRATES_PER_BUS  6
#define BUS_ROWS                (CRATES_Y*CRATE_HEIGHT)
#define CRATE_COUNT             (CRATES_X*CRATES_Y)
#define CRATE_SIZE              (CRATE_WIDTH*CRATE_HEIGHT)
#define BUS_SIZE                (CRATES_PER_BUS*CRATE_SIZE*BYTES_PER_PIXEL)
#define BUFF_SIZE               (CRATE_COUNT*CRATE_SIZE*BYTES_PER_PIXEL)

unsigned const char BOTTLE_MAP[CRATE_SIZE] = {
           6, 7,  8, 15, 16, 
           5, 4,  9, 14, 17, 
           0, 3, 10, 13, 18, 
           1, 2, 11, 12, 19
};


void decodeOneStep(const char* filename);


int main() {

if(wiringPiSPISetup(chann, frequ) == -1) {

        printf("Could not initialize SPI!\n");

}

unsigned char buff[buffsize];

for(int i = 0; i < buffsize; i++){

buff[i] = 0xFF;

}

printf("Came to the end\n");

wiringPiSPIDataRW(chann,buff,buffsize);

while(true) {
wiringPiSPIDataRW(chann,buff,buffsize);
sleep(2);
decodeOneStep("nyancat.png");
sleep(2);
decodeOneStep("rgb.png");
sleep(2);
decodeOneStep("hack.png");
sleep(2);
decodeOneStep("circle.png");
sleep(2);
decodeOneStep("checker.png");
sleep(2);
decodeOneStep("green-checker.png");
sleep(2);
decodeOneStep("wiki.png");
sleep(2);
decodeOneStep("chinese.png");
sleep(2);
decodeOneStep("ok.png");
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
  wiringPiSPIDataRW(chann,buffer,BUFF_SIZE);
  printf("Width %i\n", width);printf("Height %i\n", height);

}




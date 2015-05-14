#include <stdio.h>
#include <wiringPiSPI.h>

const int chann = 1;
const int frequ = 1000000;
const int leds = 480;
const int buffsize = leds * 3;

int main() {

if(wiringPiSPISetup(chann, frequ) == -1) {

        printf("Could not initialize SPI!\n");

}

unsigned char buff[buffsize];

for(int i = 0; i < buffsize; i++){

buff[i] = 0x00;

}

printf("Came to the end\n");

int n = wiringPiSPIDataRW(chann,buff,buffsize);

}


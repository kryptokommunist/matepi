#include "display.c"
#include <unistd.h>

int main() {

	unsigned char crapbuffer[BUFF_SIZE] = *pngToCrap("circle.png");

while(true){

	
    wiringPiSPIDataRW(SPI_CHANNEL,crapbuffer,BUFF_SIZE);
    sleep(5);
}    

}
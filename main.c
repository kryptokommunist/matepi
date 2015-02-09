#include "display.c"
#include <unistd.h>

int main() {

while(true){

	unsigned char crapbuffer = pngToCrap("circle.png");
    wiringPiSPIDataRW(SPI_CHANNEL,crapbuffer,BUFF_SIZE);
    sleep(5);
}    

}
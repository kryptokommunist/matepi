#include "display.c"
#include <unistd.h>

int main() {

while(true)

	unsigned char buffer = pngToCrap("circle.png");
    wiringPiSPIDataRW(SPI_CHANNEL,buffer,BUFF_SIZE);
    sleep(5);
    

}
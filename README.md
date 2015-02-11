# matepi
Controlling an led screen consisting of mate crates (WS2801 chipset led pixels)

Compile png.c with: 

g++ -Wall -o png png.c lodepng.c -lwiringPi

To run gifplayer first compile display.c (mategif.py depends on this c-code (ctypes)) with:

gcc -std=c99 -shared -Wl,-soname,display -o display.so -fPIC display.c -lwiringPi

(WiringPi library needs to be installed!)

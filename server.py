import socket
import ctypes

display = ctypes.CDLL("/home/pi/matepi/matelight-gifplayer-master/display.so")

HOST = "192.168.2.157"
PORT = 1377
BUFFSIZE = 16*30*20*3

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

s.bind((HOST,PORT))

print "hunny, i'm listening..."

while 1:
  data = (ctypes.c_ubyte * BUFFSIZE) (*s.recv(BUFFSIZE))
  display.display(ctypes.cast(data, ctypes.POINTER(ctypes.c_ubyte)))
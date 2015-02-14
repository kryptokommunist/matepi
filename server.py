import socket
import struct
import ctypes
from ctypes import c_size_t, c_uint8, c_void_p, c_float, CDLL, Structure, POINTER
import colorsys
from itertools import product
import numpy
import time


CRATE_WIDTH = 5
CRATE_HEIGHT = 4
CRATES_X = 6
CRATES_Y = 4

DISPLAY_WIDTH = CRATES_X*CRATE_WIDTH
DISPLAY_HEIGHT = CRATES_Y*CRATE_HEIGHT
CRATE_SIZE = CRATE_WIDTH*CRATE_HEIGHT*3
FRAME_SIZE = DISPLAY_WIDTH*DISPLAY_HEIGHT

display = ctypes.CDLL("/home/pi/matepi/matelight-gifplayer-master/display.so")


class COLOR(Structure):
     _fields_ = [('r', c_uint8), ('g', c_uint8), ('b', c_uint8), ('a', c_uint8)]

class FRAMEBUFFER(Structure):
     _fields_ = [('data', POINTER(COLOR)), ('w', c_size_t), ('h', c_size_t)]

def sendframe(framedata):
    """ Send a frame to the display
    The argument contains a h * w array of 3-tuples of (r, g, b)-data or 4-tuples of (r, g, b, a)-data where the a
    channel is ignored.
    """
    # just use the first Mate Light available
    rgba = len(framedata) == DISPLAY_WIDTH*DISPLAY_HEIGHT*4
    global dbuf
    numpy.copy(dbuf[:480*(3+rgba)], numpy.frombuffer(framedata, dtype=numpy.uint8))
    display.display(dbuf.ctypes.data_as(POINTER(c_uint8)))		


HOST = "192.168.2.157"
PORT = 1337
BUFFSIZE = 16*30*20*3

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

s.bind((HOST,PORT))

print "hunny, i'm listening..."

while 1:
  print(s.recv(BUFFSIZE))
  data = s.recv(BUFFSIZE+4)
  sendframe(data)
  display.display(ctypes.cast(data, ctypes.POINTER(ctypes.c_ubyte)))
  print "received data"



import socket
import struct
import ctypes
from ctypes import * #from ctypes import c_size_t, c_uint8, c_void_p, c_float, CDLL, Structure, POINTER
import colorsys
from itertools import product
import numpy
import time
import threading


CRATE_WIDTH = 5
CRATE_HEIGHT = 4
CRATES_X = 6
CRATES_Y = 4

DISPLAY_WIDTH = CRATES_X*CRATE_WIDTH
DISPLAY_HEIGHT = CRATES_Y*CRATE_HEIGHT
CRATE_SIZE = CRATE_WIDTH*CRATE_HEIGHT*3
FRAME_SIZE = DISPLAY_WIDTH*DISPLAY_HEIGHT

display = ctypes.CDLL("/home/pi/matepi/matelight-gifplayer-master/display.so") # shared library in C for framebuffer output

# ---------------------------------------------------

class COLOR(Structure):
     _fields_ = [('r', c_uint8), ('g', c_uint8), ('b', c_uint8), ('a', c_uint8)]

class FRAMEBUFFER(Structure):
     _fields_ = [('data', POINTER(COLOR)), ('w', c_size_t), ('h', c_size_t)]

bdf = CDLL('./libbdf.so')
bdf.read_bdf_file.restype = c_void_p
bdf.framebuffer_render_text.restype = POINTER(FRAMEBUFFER)
bdf.framebuffer_render_text.argtypes= [c_char_p, c_void_p, c_void_p, c_size_t, c_size_t, c_size_t]

unifont = bdf.read_bdf_file('unifont.bdf')

def compute_text_bounds(text):
  assert unifont
  print text
  textbytes = text
  textw, texth = c_size_t(0), c_size_t(0)
  res = bdf.framebuffer_get_text_bounds(textbytes, unifont, byref(textw), byref(texth))
  if res:
    raise ValueError('Invalid text')
  return textw.value, texth.value

cbuf = create_string_buffer(FRAME_SIZE*sizeof(COLOR))
cbuflock = threading.Lock()

def render_text(text, offset):
  global cbuf
  cbuflock.acquire()
  textbytes = text
  res = bdf.framebuffer_render_text(textbytes, unifont, cbuf, DISPLAY_WIDTH, DISPLAY_HEIGHT, offset)
  if res:
    raise ValueError('Invalid text')
  cbuflock.release()
  return cbuf

printlock = threading.Lock()

def printframe(fb):
  printlock.acquire()
  #print('\0337\033[H', end='')
  #from ctypes import *print('Rendering frame @{}'.format(time()))
  bdf.console_render_buffer(fb, DISPLAY_WIDTH, DISPLAY_HEIGHT)
  #print('\033[0m\033[KCurrently rendering', current_entry.entrytype, 'from', current_entry.remote, ':', current_entry.text, '\0338', end='')
  printlock.release()

def log(*args):
  printlock.acquire()
  print(strftime('\x1B[93m[%m-%d %H:%M:%S]\x1B[0m'), ' '.join(str(arg) for arg in args), '\x1B[0m')
  sys.stdout.flush()
  printlock.release()

class TextRenderer:
  def __init__(self, text):
    self.text = text
    self.width, _ = compute_text_bounds(text)

  def __iter__(self):
    for i in range(-DISPLAY_WIDTH, self.width):
      #print('Rendering text @ pos {}'.format(i))
      yield render_text(self.text, i)

#  -------------------------------------------------------

dbuf = numpy.zeros(DISPLAY_WIDTH*DISPLAY_HEIGHT*3, dtype=numpy.uint8)

def sendframe(framedata):
    """ Send a frame to the display
    The argument contains a h * w array of 3-tuples of (r, g, b)-data or 4-tuples of (r, g, b, a)-data where the a
    channel is ignored.
    """
    # just use the first Mate Light available
    rgba = len(framedata) == DISPLAY_WIDTH*DISPLAY_HEIGHT*4

    if rgba == 1:
       framedata = numpy.frombuffer(framedata, dtype=numpy.uint8)
       framedata = numpy.delete(framedata, numpy.arange(0, framedata.size, 4))

    global dbuf
    numpy.copyto(dbuf[:480*(3+rgba)], numpy.frombuffer(framedata, dtype=numpy.uint8))

    display.display(dbuf.ctypes.data_as(POINTER(c_uint8)), rgba)		


HOST = "192.168.2.157"
PORT = 1337
BUFFSIZE = 16*30*20*3

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

s.bind((HOST,PORT))

print "hunny, i'm listening..."

while 1:
  #print(s.recv(BUFFSIZE))
  
  #data = s.recv(BUFFSIZE)
  #sendframe(data)

  #display.display(ctypes.cast(data, ctypes.POINTER(ctypes.c_uint8)))
  #print "received data"

  renderer = TextRenderer("fsjkfjasdkf Penis lOLO")

  for frame in renderer:
      sendframe(frame)
#     printframe(frame)
      time.sleep(0.1)



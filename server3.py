import socket
import struct
import ctypes
from ctypes import * #from ctypes import c_size_t, c_uint8, c_void_p, c_float, CDLL, Structure, POINTER
import colorsys
from itertools import product
import numpy
import time
import threading
import itertools
import queue
import sys

# Brightness of the LEDs in percent. 1.0 means 100%.
BRIGHTNESS = 1

UDP_TIMEOUT = 3.0

TEXT_RENDER_SLEEP = 0.3

CRATE_WIDTH = 5
CRATE_HEIGHT = 4
CRATES_X = 6
CRATES_Y = 4

DISPLAY_WIDTH = CRATES_X*CRATE_WIDTH
DISPLAY_HEIGHT = CRATES_Y*CRATE_HEIGHT
CRATE_SIZE = CRATE_WIDTH*CRATE_HEIGHT*3
FRAME_SIZE = DISPLAY_WIDTH*DISPLAY_HEIGHT

HOST = "192.168.2.157"
PORT = 1337
BUFFSIZE = FRAME_SIZE*3

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
  print(time.strftime('\x1B[93m[%m-%d %H:%M:%S]\x1B[0m'), ' '.join(str(arg) for arg in args), '\x1B[0m')
  sys.stdout.flush()
  printlock.release()

class TextRenderer:
  def __init__(self, text):
    self.text = text
    self.width, _ = compute_text_bounds(text)

  def __iter__(self):
    for i in range(-DISPLAY_WIDTH, self.width):
      #print('Rendering text @ pos {}'.format(i))
      time.sleep(TEXT_RENDER_SLEEP)
      yield render_text(self.text, i)

#  -------------------------------------------------------

dbuf = numpy.zeros(DISPLAY_WIDTH*DISPLAY_HEIGHT*4, dtype=numpy.uint8)

def sendframe(framedata):
    """ Send a frame to the display
    The argument contains a h * w array of 3-tuples of (r, g, b)-data or 4-tuples of (r, g, b, a)-data where the a
    channel is ignored.
    """
    # just use the first Mate Light available
    rgba = len(framedata) == DISPLAY_WIDTH*DISPLAY_HEIGHT*4
    global dbuf

    """if rgba == 1:
       framedata = numpy.frombuffer(framedata, dtype=numpy.uint8)
       framedata = numpy.delete(framedata, numpy.arange(0, framedata.size, 4))
       dbuf[:480*(3+rgba)] = numpy.copy(framedata)"""

    #print("rgba = " + str(rgba) + "| 480*(3+gam) = " + str(480*(3+gam)) + "\nlen(framedata) = " + str(len(framedata)))
   
    numpy.copyto(dbuf[:480*(3+rgba)], numpy.frombuffer(framedata, dtype=numpy.uint8))

    display.display(dbuf.ctypes.data_as(POINTER(c_uint8)), c_float(BRIGHTNESS), rgba)

class UDPServer:

  def __init__(self, port = 1337, ip= ''):
    self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    self.socket.bind((ip, port))
    self.thread = threading.Thread(target = self.udp_receive)
    self.thread.daemon = True
    self.start = self.thread.start
    self.frame_condition = threading.Condition()
    self.frame = None

  def frame_da(self):
    return self.frame is not None

  def __iter__(self):
    while True:
      with self.frame_condition:
        if not self.frame_condition.wait_for(self.frame_da, timeout=UDP_TIMEOUT):
          raise StopIteration()
        frame, self.frame = self.frame, None
        yield frame

  def udp_receive(self):
    while True:
      try:
        data = self.socket.recv(BUFFSIZE)
        if len(data) == BUFFSIZE+4:
          frame = data[:-4]
        elif len(data) == BUFFSIZE:
          frame = data
        else:
          raise ValueError('Invalid frame size: {}'.format(len(data)))
        with self.frame_condition:
          self.frame = frame
          self.frame_condition.notify()
      except Exception as e:
        print ("Error receiving UDP frame:", e)

render_queue = queue.LifoQueue(maxsize=50)

class TCPServer:
  """A TCP Server, that listens for text to display"""

  def __init__(self, port = 1337, ip=''):
    self.socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    self.socket.bind((ip, port))
    self.socket.listen(1)
    self.thread = threading.Thread(target = self.tcp_receive)
    self.thread.daemon = True
    self.start = self.thread.start

  def tcp_receive(self):
    global render_queue
    while 1:
      conn, addr = self.socket.accept()
      data = str(conn.recv(1024), 'UTF-8').strip().encode('utf-8')
      if len(data) > 140:
        conn.send(b'TOO MUCH INFORMATION!\n')
        continue
      log('\x1B[95mText from\x1B[0m {}: {}\x1B[0m'.format(addr, data))
      conn.close()
      render_queue.put(TextRenderer(data))


print("hunny, i'm listening...")

tcp_server = TCPServer(PORT, HOST)
tcp_server.start()

udp_server = UDPServer(PORT, HOST)
udp_server.start()

defaultlines = [ TextRenderer(l[:-1].replace('\\x1B', '\x1B').encode('utf-8')) for l in open('default.lines', encoding='utf-8').readlines() ]
#random.shuffle(defaultlines)
defaulttexts = itertools.chain(*defaultlines)

while 1:

  if udp_server.frame_da():
     renderer = udp_server
  elif not render_queue.empty():
     renderer = render_queue.get()
  else:
    try: 
      frame = next(defaulttexts)
    except StopIteration:
      defaultlines = [ TextRenderer(l[:-1].replace('\\x1B', '\x1B').encode('utf-8')) for l in open('default.lines', encoding='utf-8').readlines() ]
      defaulttexts = itertools.chain(*defaultlines)

    sendframe(frame)
    time.sleep(TEXT_RENDER_SLEEP)
    continue


  for frame in renderer:
     sendframe(frame)
     #printframe(frame)
     #time.sleep(0.1)



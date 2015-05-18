
import socket
import struct
import zlib
from time import time, strftime, sleep
from collections import namedtuple, deque
import itertools
import threading
import random
import os
import sys
import ctypes


CRATE_WIDTH = 5
CRATE_HEIGHT = 4
CRATES_X = 6
CRATES_Y = 4

DISPLAY_WIDTH = CRATES_X*CRATE_WIDTH
DISPLAY_HEIGHT = CRATES_Y*CRATE_HEIGHT
CRATE_SIZE = CRATE_WIDTH*CRATE_HEIGHT*3
FRAME_SIZE = DISPLAY_WIDTH*DISPLAY_HEIGHT

display = ctypes.CDLL("/home/pi/matepi/matelight-gifplayer-master/display.so") # shared library in C for framebuffer output

#!/usr/bin/env python


UDP_TIMEOUT = 3.0

class COLOR(Structure):
    _fields_ = [('r', c_uint8), ('g', c_uint8), ('b', c_uint8), ('a', c_uint8)]

class FRAMEBUFFER(Structure):
    _fields_ = [('data', POINTER(COLOR)), ('w', c_size_t), ('h', c_size_t)]

bdf = CDLL('./libbdf.so')
bdf.read_bdf_file.restype = c_void_p
bdf.framebuffer_render_text.restype = POINTER(FRAMEBUFFER)
bdf.framebuffer_render_text.argtypes= [c_char_p, c_void_p, c_void_p, c_size_t, c_size_t, c_size_t]

unifont = bdf.read_bdf_file('unifont.bdf')

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


def compute_text_bounds(text):
  assert unifont
  textbytes = bytes(str(text), 'UTF-8')
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
  textbytes = bytes(str(text), 'UTF-8')
  res = bdf.framebuffer_render_text(textbytes, unifont, cbuf, DISPLAY_WIDTH, DISPLAY_HEIGHT, offset)
  if res:
    raise ValueError('Invalid text')
  cbuflock.release()
  return cbuf

printlock = threading.Lock()

def printframe(fb):
  printlock.acquire()
  print('\0337\033[H', end)
  print('Rendering frame @{}'.format(time()))
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

class MateLightUDPServer:
  def __init__(self, port=1337, ip='192.168.2.157'):
    self.current_client = None
    self.last_timestamp = 0
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
        data, (addr, sport) = self.socket.recvfrom(FRAME_SIZE*3+4)
        timestamp = time()
        if timestamp - self.last_timestamp > UDP_TIMEOUT:
          self.current_client = addr
          log('\x1B[91mAccepting UDP data from\x1B[0m', addr)
        if addr == self.current_client:
          if len(data) == FRAME_SIZE*3+4:
            frame = data[:-4]
            crc1, = struct.unpack('!I', data[-4:])
            if crc1:
              crc2, = zlib.crc32(frame, 0),
              if crc1 != crc2:
                raise ValueError('Invalid frame CRC checksum: Expected {}, got {}'.format(crc2, crc1))
          elif len(data) == FRAME_SIZE*3:
            frame = data
          else:
            raise ValueError('Invalid frame size: {}'.format(len(data)))
          self.last_timestamp = timestamp
          with self.frame_condition:
            self.frame = frame
            self.frame_condition.notify()
      except Exception as e:
        log('Error receiving UDP frame:', e)

renderqueue = deque()

class MateLightTCPTextHandler(BaseRequestHandler):
  def handle(self):
    global render_deque
    data = str(self.request.recv(1024).strip(), 'UTF-8')
    addr = self.client_address[0]
    if len(data) > 140:
      self.request.sendall(b'TOO MUCH INFORMATION!\n')
      return
    log('\x1B[95mText from\x1B[0m {}: {}\x1B[0m'.format(addr, data))
    renderqueue.append(TextRenderer(data))
    self.request.sendall(b'KTHXBYE!\n')

TCPServer.allow_reuse_address = True
tserver = TCPServer(('192.168.2.157', 1337), MateLightTCPTextHandler)
t = threading.Thread(target=tserver.serve_forever)
t.daemon = True
t.start()

userver = MateLightUDPServer()
userver.start()

defaultlines = [ TextRenderer(l[:-1].replace('\\x1B', '\x1B')) for l in open('default.lines').readlines() ]
#random.shuffle(defaultlines)
defaulttexts = itertools.chain(*defaultlines)

if __name__ == '__main__':
  print('\033[?1049h'+'\n'*9)
  while True:
    if renderqueue:
      renderer = renderqueue.popleft()
    elif userver.frame_da():
      renderer = userver
    else:
      static_noise = time() % 300 < 60
      if False:
        foo = os.urandom(640)
        frame = bytes([v for c in zip(list(foo), list(foo), list(foo)) for v in c ])
        sleep(0.05)
      else:
        try:
          frame = next(defaulttexts)
        except StopIteration:
          defaultlines = [ TextRenderer(l[:-1].replace('\\x1B', '\x1B')) for l in open('default.lines').readlines() ]
          #random.shuffle(defaultlines)
          defaulttexts = itertools.chain(*defaultlines)
      sendframe(frame)
#     printframe(frame)
      continue
#   sleep(0.1)
    for frame in renderer:
      sendframe(frame)
#     printframe(frame)
#     sleep(0.1)

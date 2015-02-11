import socket
import time

HOST = "192.168.2.157"
PORT = 1377
data = "some text"

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 


while 1:

  s.sendto(data,(HOST,PORT))
  print("Sent data")
  time.sleep(1)
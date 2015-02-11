import socket

HOST = "192.168.2.157"
PORT = 1377

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

s.bind((HOST,PORT))

while 1:
  print s.recv(30)
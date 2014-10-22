#Group 4
#10/13/14
#Receiver from forwarder
# Takes the paket from forwarder and prints out the contens
import socket
import sys
import os
from struct import *

HOST = '0.0.0.0'

if len(sys.argv) != 2:
  print 'rec.py [Port number (10150+GID)]'
  sys.exit(2)

port =int(sys.argv[1])

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.setblocking(True)
s.bind((HOST,port))

while True:
  data, address = s.recvfrom(1012) #block until data received (1012 max bytes)

  if data:
    data = unpack('!hhII1000s', data)
    print "Sender IP", data[3]
    print "Sender Port", data[1]
    print "Message: ", data[4]
    print " "

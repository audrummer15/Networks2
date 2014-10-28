#Group 4
#10/13/14
#Receiver from forwarder
# Takes the paket from forwarder and prints out the contens
import socket, struct
import sys
import os
from struct import *

MAXBUFLEN = 1036
HOST = '0.0.0.0'

if len(sys.argv) != 2:
  print 'rec.py [Port number (10150+GID)]'
  sys.exit(2)

port =int(sys.argv[1])

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.setblocking(True)
s.bind((HOST,port))

while True:
  data, address = s.recvfrom(MAXBUFLEN) #block until data received (1036 max bytes)

  if data:
    data = unpack('!HHII1024s', data)
    print "Forwarder IP: ", address[0]
    print "Sender IP: ", socket.inet_ntoa(struct.pack("!I", data[3]))
    print "Message: ", data[4]
    print " "

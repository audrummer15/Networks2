# Echo client program
# Used to test lab12

import socket
import sys

from struct import *

PORT = 10014 #10010 + GID 

if len(sys.argv) <= 1:
	print 'Usage: stream_client.py <server_ip_address>'
	sys.exit()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((sys.argv[1], PORT))

op = '/'
var1 = 14
var2 = 0

packed = pack('cII', op, var1, var2)

s.send(packed)

data = s.recv(1024)

data = unpack('cIIIc', data)

print "Data: ", data[0], data[1], data[2], data[3], data[4]

s.close()

print 'Received', repr(data)
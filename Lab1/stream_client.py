# Echo client program
import socket
import sys

PORT = 10014 #10010 + GID 

if len(sys.argv) <= 1:
	print 'Usage: stream_client.py <server_ip_address>'
	sys.exit()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((sys.argv[1], PORT))

s.send('Hello, world')

data = s.recv(1024)

s.close()

print 'Received', repr(data)
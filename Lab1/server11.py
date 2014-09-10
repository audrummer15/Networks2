# Concurrent DGRAM Echo server program
import socket
import sys
import os

from collections import namedtuple

HOST = 'localhost'
PORT = 10014              # 10010 + GID

# A structure to handle packets in RFC form
Packet = namedtuple("Packet", "length seq timestamp data")

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((HOST, PORT))

while True:
	data, address = s.recvfrom(1038) #block until data received (1038 max bytes due to RFC)

	# When data is received, fork and let the child process the data and respond
	# This allows the parent to continue processing requests
	if os.fork() == 0: 
		if data:
			thisPacket = Packet(data[0:1], data[2:5], data[6:13], data[14:])
			sent = s.sendto(data, address)

			print "Data: ", thisPacket.data

			sys.exit()

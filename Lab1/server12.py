# Echo server program
#
# Recieve Message M
# Extract two unsigned integers (a and b) and a character (c) that can be + - x or /
# Perform the operation requested
# Send back the result with repeating the operands and operator
import socket
import sys
import os

from collections import namedtuple
from struct import *

HOST = '0.0.0.0'
PORT = 10014              # 10010 + GID

# A structure to handle packets in RFC form (Adam - not sure if needed anymore)
RequestPacket = namedtuple("RequestPacket", "operator operand1 operand2")
ResponsePacket = namedtuple("ResponsePacket", "operator operand1 operand2 answer isValid")

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))

while 1:
	s.listen(1)
	conn, addr = s.accept()

	print 'Connected by', addr

	# Receive Data and (if it's there) unpack in the form of "char, unsigned int, unsigned int"
	datarecv = conn.recv(1024)

	#If the data isn't there, close the connection and continue listening
	if not datarecv:
		conn.close()
		continue

	data = unpack('cII', datarecv)

	print 'Data: ', data

	# Construct a request packet for easier referencing (Adam - not sure if needed)
	req = RequestPacket(data[0], data[1], data[2])
	ansIsValid = '1'

	if req.operator == '+':
		result = int(req.operand1) + int(req.operand2)
	elif req.operator == '-':
		result = int(req.operand1) - int(req.operand2)
	elif req.operator == 'x':
		result = int(req.operand1) * int(req.operand2)
	elif req.operator == '/':
		if req.operand2 != 0:
			result = int(req.operand1) / int(req.operand2)
		else:
			result = 0
			ansIsValid = '0'
	else:
		result = 0
		ansIsValid = '0'

	# Pack the data back in and send it back to the origin
	resp = pack('cIIIc', req.operator, req.operand1, req.operand2, result, ansIsValid)
	conn.send(resp)

	conn.close()
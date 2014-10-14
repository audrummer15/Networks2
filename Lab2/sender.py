#Ben Gustafson
#10/13/14
#UDP Sender to forwarder
#Takes Client, Forwarder, and sender info on commmand line and sends the message given to forwarder

import socket, struct
import sys
import os
from struct import *


def send_message(sock, message, message_length, forward_ip, forward_port, client_ip, client_port, listening_port, source_ip):
	try:
		#make message
		print "making"
		print forward_ip, " ", forward_port, " ", client_ip, " ", client_port, " ", source_ip, " ", message
		message_to_send = pack('!hhII1000s', client_port, listening_port,struct.unpack("!L", socket.inet_aton(client_ip))[0], struct.unpack("!L", socket.inet_aton(source_ip))[0], message)

		# Send data
		print 'Sending: ', message
		print "Full message: ", message_to_send
		#Forwarder:
		#server_address = (forward_ip,forward_port)
		#Send to Client:
		server_address = (client_ip,client_port)
		sent = sock.sendto(message_to_send, server_address)
		print sent
		return True
	except Exception as inst:
		print inst
		pass
		return False

#Check command line arguments
if len(sys.argv) != 6:
	print 'send.py [Forwarder Name] [Forwarder Port # (1010+GID) ] [Client Name] [Client Port # (10150+GID)] [Source IP]'
	sys.exit(2)

forward_name = sys.argv[1]
forward_port = int(sys.argv[2])
client_name = sys.argv[3]
client_port = int(sys.argv[4])
source_ip = sys.argv[5]

print sys.argv[1], " ", sys.argv[2], " ", sys.argv[3], " ", sys.argv[4], " ", sys.argv[5]
print forward_name, " ", forward_port, " ", client_name, " ", client_port, " ", source_ip
continue_to_send = True

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while continue_to_send:

	message = raw_input('Enter your message: ')

	if message == "Exit" or message == "exit":
		continue_to_send = False

	else :
		message_sent = send_message(sock, message, len(message), forward_name, forward_port, client_name, client_port, 10054, source_ip)
		if message_sent:
			print 'Message Sent Successfully.'
		else :
			print 'Message Failed to Send.'

#Ending Script
print 'Closing socket! Goodbye'
sock.close()

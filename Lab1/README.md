Networks2
=========
- COMP 6320/6326/5320
- Assignment 1, Group 4
- Adam Brown
- Ben Gustafson

Overview: 
-------------

We have two different servers written in python and three clients written in c. 

Part A and B below describe how to use our servers and clients.

LAB 11: 
-------------

#####Server11#####

This server is a simple echo server sending back exactly what it receives. 

Open up the terminal navigate to this project and run:
	
	$ python server11.py

The server will be waiting for a data gram on the port 10014

#####Client11b#####

This is a simple data gram client, prompting the user for input and then sending that to the echo server and printing the response. 

To run the client run:

	$ make lab11bmakefile

Then

	$ ./client11b.o {hostname} {port}

You will be prompted for the message to send and once you press 'enter' your message will be sent. 

You will see the data show up on the server and the response from the server on the client. A round trip time will also be printed. 

#####Client11c#####

This is another simple datagram client sending 100,000 data grams to the echo server and printing the response. 

To run the client run:

	$ make lab11cmakefile

Then

	$ ./client11c.o {hostname} {port}


You will see the data show up on the server and the client print out the packets sent from the server. 

LAB 12: 
-------------

#####Server12#####

This server is a math wiz. Send it an equation and it will return the correct answer. 

Open up the terminal navigate to this project and run:
	
	$ python server12.py

The server will be waiting for a TCP connection on the port 10014

#####Client12#####

This is a simple TCP client taking in an equation and sending it to the server. 

To run the client run:

	$ make lab12makefile

Then

	$ ./client12.o {hostname} {port} {a} {b} {operand}

In order to start your program you must submit an equation to be evaluated. A and B have to be positive and only addition(+), subtraction(-), multiplication(x) and division(/) is supported. 

You will see the response from the server print out. 


Report: 
-------------
All of our code works on our local (mac) machines. 

We had a very hard time moving the lab12 to the tux machines. We are still currently unable to guarantee the server and the client will interact appropriately on the tux machines. It appears there are two different byte orders on the mac kernel and the linux kernel. Meaning the adjustments made on the the tux machines interfered with the edits make on the Mac. We were unable to copy the structs of a packet into a buffer to be sent, decoded correctly, modified, placed in a buffer and sent back to be decoded again. It appears at each encoding and decoding byte orders changed causing problems for the calculations. With more time all the bugs could be found but it is fairly buggy currently, but again it was working just fine on our Mac's.




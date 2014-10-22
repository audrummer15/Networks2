Networks2
=========
- COMP 6320/6326/5320
- Assignment 2, Group 4
- Adam Brown, amb0027@auburn.edu
- Ben Gustafson, brg0005@auburn.edu

Overview:
-------------

The goal is to send a packet through a forwarder to the actual destination.


LAB 2:
-------------

We have a sender (python), forwarder (C), receiver (python).

The beginning of each packet is formated as below:

<---- 16 bits ----> < ---- 16 bits ---->
|---------------------------------------|
|   Target Port #  |   Source Port #    |
|---------------------------------------|
|						Target IP Address						|
|---------------------------------------|
|						Source IP Address           |
|---------------------------------------|

The message follows, with a maximum of 1kb.


##Run Sender:

We have to enter in the information for the forwarder and client inorder for the packet to end up in the right place.

Type:

	$ python snd.py [Forwarder IP] [Forwarder Port # (10010+GID) ] [Client IP] [Client Port # (10150+GID)] [Source IP]

	$ python snd.py 127.0.0.1 10014 127.0.0.1 10154 127.0.0.1

Enter you message.

After you are done just type 'exit' in the "Enter your message:" and the program will close


##Run Forwarder

First we have to compile fwd.c

	 $ gcc fwd.c -o fwd.o

Then run fwd.o

	$ ./fwd.o [Listenting port number]
	$ ./fwd.o 10014

The packet being forwarded will be printed out and forwarded to the correct destination.

##Run Receiver

Type:

	$ python rec.py [Listening Port # (10150+GID)]
	$ python rec.py 10154

Report:
-------------

We had an interesting time with decoding the bytes in C.
Some of the bytes were being automatically upgraded to integers causing the IP addressed to be incorrect.
A cast to (unsigned) (unsigned char) fixed the issue.




long time; /* know C? */
Unprecedented performance: Nothing ever ran this slow before.
Any sufficiently advanced bug is indistinguishable from a feature.
Real Programmers confuse Halloween and Christmas, because dec 25 == oct 31.
The best way to accelerate an IBM is at 9.8 m/s/s.
recursion (re - cur' - zhun) n. 1. (see recursion)

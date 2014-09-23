/*
** client11c.c -- a datagram "client" sending 100,000 packets
**					Tracks the round trip time and reports the highest, lowest and average
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

#define MAXMESSAGE 1024
#define PACKET_LENGTH_SIZE 2
#define PACKET_SEQUENCE_NUMBER_SIZE 4
#define PACKET_TIMESTAMP_SIZE 8
#define MAXDATASIZE 1038 // max number of bytes we can get at once 
#define PACKETSSENT 100000

struct Packet
{
          uint16_t length;
          uint32_t sequence_number;
          uint64_t timestamp;
          char message[MAXMESSAGE];
};

void build_packet(struct Packet *pack, uint32_t sequence_number_in, char *message[]);
void build_packet_from_socket(struct Packet *pack, char recived_data[], int data_length);
void print_packet(struct Packet *pack, char type[]);
void build_string_from_packet(struct Packet *pack,char *buffer_out[]);


int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    if (argc != 3) {
        fprintf(stderr,"Please use: ./client11c.o hostname port#\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }
 	
 	int mypid = fork();
 	
 	if( 0 == mypid )
 	{
	 	//Child
	 	
	 	int next_sequence_number=1, packets_recieved =0;
	 	char buf[MAXDATASIZE];
	 	unsigned long roundtrip_high=0, roundtrip_low=0, roundtrip_avg=0;
	 	while (next_sequence_number <= PACKETSSENT)
	 	{
	 	
	 	    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	 	    	perror("Child recieving error:");
	 	    	exit(1);
	 	    }
	 	    
	 	    buf[numbytes] = '\0';
	 		struct Packet packet_recieved;
	 		build_packet_from_socket(&packet_recieved,buf,numbytes);
	 		print_packet(&packet_recieved, "Recieved");
	 		if(next_sequence_number == packet_recieved.sequence_number)
	 		{
		 		unsigned long roundtrip_time= (time(NULL) * 1000)- packet_recieved.timestamp;
		 		if (next_sequence_number ==1)
		 		{
		 			roundtrip_low =roundtrip_time;
		 		}
		 		if (roundtrip_time > roundtrip_high)
		 			roundtrip_high = roundtrip_time;
		 		else if (roundtrip_time < roundtrip_low)
		 			roundtrip_low = roundtrip_time;
		 		roundtrip_avg += roundtrip_time;
		 		
		 		packets_recieved ++;
		 		next_sequence_number++;
		 	}
		 	else
		 	{
		 		if (packet_recieved.sequence_number > next_sequence_number)
		 		{
		 			printf("Did not recieve packet(s) with the sequence number between: %d and %d",next_sequence_number,packet_recieved.sequence_number);
		 			next_sequence_number=packet_recieved.sequence_number+1;
		 		}
		 		else
		 		{
		 			printf("Recieve packet with the sequence number: %d late",packet_recieved.sequence_number);
		 			unsigned long roundtrip_time= (time(NULL) * 1000)- packet_recieved.timestamp;
		 			
		 			if (roundtrip_time > roundtrip_high)
		 				roundtrip_high = roundtrip_time;
		 			else if (roundtrip_time < roundtrip_low)
		 				roundtrip_low = roundtrip_time;
		 			roundtrip_avg += roundtrip_time;
		 			packets_recieved ++;
		 		}
		 	}
	 	}
	 	//round trip times
	 	printf("Round trip time average: %lu ms\n", (roundtrip_avg/packets_recieved));
	 	printf("Round trip time max: %lu ms\n", roundtrip_high);
	 	printf("Round trip time min: %lu ms\n", roundtrip_low);
	 	
 	}
 	else
 	{
 	    //Parent
 		int count = 1;
 		char *count_buffer;
 		count_buffer = (char *) malloc (20 + 1);
 		while (count <= PACKETSSENT)
 		{
	 		struct Packet packet_sent;
	 		sprintf(count_buffer, "%d", count);
	 		build_packet(&packet_sent,count,&count_buffer);
	 		//print_packet(&packet_sent, "Sent");
	 		char **buffer;
	 		build_string_from_packet(&packet_sent,buffer);
	 		
	 		if ((numbytes = sendto(sockfd, buffer, (packet_sent.length + 3), 0,p->ai_addr, p->ai_addrlen)) == -1) {
	 		    perror("Parent sending error: sendto");
	 		    exit(1);
	 		}
	 		
 			count++;
 			usleep(100000);
 		}
 		
 	}	
	
    freeaddrinfo(servinfo);
        
    close(sockfd);

    return 0;
}

void build_packet(struct Packet *pack, uint32_t sequence_number_in, char *message[])
{
	pack->sequence_number = sequence_number_in;
	pack->timestamp  = time(NULL) * 1000;
	strcpy(pack->message, *message); 
	pack->length = strlen(*message) + PACKET_LENGTH_SIZE+PACKET_TIMESTAMP_SIZE+PACKET_SEQUENCE_NUMBER_SIZE;
}
void build_packet_from_socket(struct Packet *pack, char recieved_data[], int data_length)
{
	memset(pack,0,data_length);
	memcpy(pack, recieved_data, data_length);
}
void build_string_from_packet(struct Packet *pack,char *buffer_out[])
{
	*buffer_out = (char*) malloc(pack->length+3);
	memcpy(buffer_out, pack, (pack->length+3));
}
void print_packet(struct Packet *pack, char type[])
{
	printf("Packet %s ======= \n",type);
	printf("  Length: %d \n", pack->length);
	printf("  Sequence Number: %d \n", pack->sequence_number);
	printf("  Timestamp: %llu \n", pack->timestamp);
	printf("  Message: %s \n\n", pack->message);
}

/*
** talker.c -- a datagram "client" demo
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

#define PORT "10014" // 10010 + GID
#define MAXMESSAGE 1024
#define PACKET_LENGTH_SIZE 2
#define PACKET_SEQUENCE_NUMBER_SIZE 4
#define PACKET_TIMESTAMP_SIZE 8
#define MAXDATASIZE 1038 // max number of bytes we can get at once 

struct Packet
{
          uint16_t length;
          uint32_t sequence_number;
          uint64_t timestamp;
          char message[MAXMESSAGE];
};

void build_packet(struct Packet *pack, uint32_t sequence_number_in, char *message[]);
void build_packet_from_socket(struct Packet *pack, char recived_data[], int data_length);
void print_packet(struct Packet *pack);
void build_string_from_packet(struct Packet *pack,char *buffer_out[]);


int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    if (argc != 3) {
        fprintf(stderr,"Please use: ./client11b.o hostname port#\n");
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
 	
 	printf("Enter a message to send to server: ");
    // Read from command line //
    int bytes_read;
    unsigned long message_size = MAXMESSAGE;
    char *string_in;
    string_in = (char *) malloc (MAXMESSAGE + 1);
   	bytes_read = getline(&string_in, &message_size, stdin);
   	string_in[bytes_read-1] = '\0'; //Removed newline

    struct Packet packet_sent;
    build_packet(&packet_sent,1,&string_in);
    print_packet(&packet_sent);
    char **buffer;
    build_string_from_packet(&packet_sent,buffer);
    
    if ((numbytes = sendto(sockfd, buffer, (packet_sent.length + 3), 0,p->ai_addr, p->ai_addrlen)) == -1) {
        perror("client to server: sendto");
        exit(1);
    }
    
    char buf[MAXDATASIZE];
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    	perror("recv");
    	exit(1);
    }
    
    buf[numbytes] = '\0';
	struct Packet packet_recieved;
	build_packet_from_socket(&packet_recieved,buf,numbytes);
	print_packet(&packet_recieved);
	
    freeaddrinfo(servinfo);
    unsigned long time_now =(time(NULL)* 1000);
    printf("Now: %lu , Timestamp: %llu \n ", time_now, packet_recieved.timestamp);
	unsigned long roundtriptime= time_now- packet_recieved.timestamp;
    printf("Round trip time: %lu ms\n", roundtriptime);
    
    close(sockfd);

    return 0;
}

void build_packet(struct Packet *pack, uint32_t sequence_number_in, char *message[])
{
	pack->sequence_number = sequence_number_in;
	pack->timestamp  = time(NULL) * 1000;
	strcpy(pack->message, *message); 
    printf( "String Length: %ld", strlen(*message));
	pack->length = strlen(*message) + PACKET_LENGTH_SIZE+PACKET_TIMESTAMP_SIZE+PACKET_SEQUENCE_NUMBER_SIZE;
}
void build_packet_from_socket(struct Packet *pack, char recieved_data[], int data_length)
{
	memset(pack,0,data_length);
	memcpy(pack, recieved_data, data_length);
}
void build_string_from_packet(struct Packet *pack, char *buffer_out[])
{
	*buffer_out = (char*) malloc(pack->length+3);
	memcpy(buffer_out, pack, (pack->length+3));
}
void print_packet(struct Packet *pack)
{
	printf("Packet ======= \n");
	printf("  Length: %hu \n", pack->length);
	printf("  Sequence Number: %u \n", pack->sequence_number);
	printf("  Timestamp: %llu \n", pack->timestamp);
	printf("  Message: %s \n\n", pack->message);
}

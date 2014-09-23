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

#define SERVERPORT "10014"    // the port users will be connecting to
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
void build_packet_from_socket(struct Packet *pack, char *recived_data[], int data_length);
void print_packet(struct Packet *pack);



int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    if (argc != 3) {
        fprintf(stderr,"usage: talker hostname message\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
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
    
    int length = sizeof(packet_sent);
    if ((numbytes = sendto(sockfd, &packet_sent, sizeof(packet_sent), 0,p->ai_addr, p->ai_addrlen)) == -1) {
        perror("client to server: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);

    printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
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
void build_packet_from_socket(struct Packet *pack, char *recived_data[], int data_length)
{
	memcpy((void*)pack->length, recived_data, PACKET_LENGTH_SIZE);
	memcpy((void*)pack->timestamp, recived_data+PACKET_LENGTH_SIZE,PACKET_LENGTH_SIZE);
	memcpy((void*)pack->sequence_number, recived_data+PACKET_LENGTH_SIZE+PACKET_LENGTH_SIZE,PACKET_SEQUENCE_NUMBER_SIZE);
	memcpy((void*)pack->message, recived_data +PACKET_LENGTH_SIZE+PACKET_LENGTH_SIZE+PACKET_SEQUENCE_NUMBER_SIZE,data_length -(PACKET_LENGTH_SIZE+PACKET_TIMESTAMP_SIZE+PACKET_SEQUENCE_NUMBER_SIZE));
}
void print_packet(struct Packet *pack)
{
	printf("Packet ======= \n");
	printf("  Length: %d \n", pack->length);
	printf("  Sequence Number: %d \n", pack->sequence_number);
	printf("  Timestamp: %llu \n", pack->timestamp);
	printf("  Message: %s \n\n", pack->message);
}

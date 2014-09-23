/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <arpa/inet.h>

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

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
    printf("Connecting to: %s\n", s);
    
    
    int mypid = fork();
    
    if( 0 == mypid )
    {
	//Child
	
	int count = 1;
	while (count < 100000)
	{
		if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
		}
		
		 buf[numbytes] = '\0';
		
		 printf("client: received: '%s'\n",buf);
		
	//   struct Packet packet_recieved;
	//   build_packet_from_socket(&packet_recieved, buf, numbytes);
	//   print_packet(&packet_recieved);
	//   uint64_t roundtrip = (time(NULL) * 1000) - packet_sent.timestamp;
	//   printf("Round Trip Time: %llu, roundtrip);
		count++;
	}    
	    	
    }
    else
    {
        //Parent
        int count = 1;
        char *count_buffer;
        count_buffer = (char *) malloc (20 + 1);
        while (count < 100000)
        {
	        struct Packet packet_sent;
	        char str[15];
	        sprintf(count_buffer, "%d", count);
	        build_packet(&packet_sent,count,&count_buffer);
	        print_packet(&packet_sent);
	        
	        int length = sizeof(packet_sent);
	        if (sendto(sockfd,&packet_sent,sizeof(packet_sent),0,p->ai_addr,p->ai_addrlen)==-1) {
	            printf("Client: Send Error:%s",strerror(errno));
	        }
	        
	        free(&packet_sent);
	        count++;
	    } 
	        
        freeaddrinfo(servinfo); // all done with this structure
    }
    
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
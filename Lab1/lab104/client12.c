/*
** client12.c -- Adam Brown and Ben Gustafson
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

#include <arpa/inet.h>

#define PORT 10014 // 10010 + GID
#define REQUEST_SIZE 9
#define RESPONSE_SIZE 14
#define MAXDATASIZE 14 // max number of bytes we can get at once 

struct Request
{
          char op_code;
          uint32_t a;
          uint32_t b;
}__attribute__((__packed__));

struct Response
{
          char op_code;
          uint32_t a;
          uint32_t b;
          uint32_t answer;
          char valid;
}__attribute__((__packed__));

void print_request_packet(struct Request *request_packet);
void print_response_packet(struct Response *response_packet);
void creat_buffer_to_send(struct Request *request_packet, char *buffer_out[]);
void creat_repsonse_packet(struct Response *response_packet, char data_recieved[]);

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

    if (argc != 6) {
        fprintf(stderr,"Please use: client12.c hostname port# uint uint command\n");
        exit(1);
    }
	
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
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
    
    struct Request request_sent;
    request_sent.a = atoi(argv[3]);
    request_sent.b = atoi(argv[4]);
    request_sent.op_code = *argv[5];
    /*char **send_buffer;
   	send_buffer = (char**) malloc(REQUEST_SIZE);
 	memset(send_buffer, 0, REQUEST_SIZE);
	creat_buffer_to_send(&request_sent, send_buffer);*/
 	print_request_packet(&request_sent);/*

	//char* bufbuf[sizeof(request_sent)];
	//memcpy(&bufbuf, &request_sent, sizeof(request_sent));	
 	//printf("Buffer: %s\n", &send_buffer);
	printf("Buffer: ");
	int i=0;
	for( i=0; i < REQUEST_SIZE; i++ )
		printf("%d ", send_buffer[i]);	
	printf("\n");*/

	send(sockfd, (void*)&request_sent, REQUEST_SIZE, 0);

    
    freeaddrinfo(servinfo); // all done with this structure
    
	
    if ((numbytes = recv(sockfd, buf, RESPONSE_SIZE, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    //buf[numbytes] = '\0';

	
    struct Response response_packet;
    creat_repsonse_packet(&response_packet, buf);
    print_response_packet(&response_packet);


    close(sockfd);

    return 0;
}
void print_request_packet(struct Request *request_packet)
{
	printf("Request Packet ======= \n");
	printf("  Code: %c \n", request_packet->op_code);
	printf("  A: %u \n", request_packet->a);
	printf("  B: %u \n", request_packet->b);
}
void print_response_packet(struct Response *response_packet)
{
	printf("Response Packet ======= \n");
	printf("  Code: %c \n", response_packet->op_code);
	printf("  A: %u \n", response_packet->a);
	printf("  B: %u \n", response_packet->b);
	printf("  Answer: %u \n", response_packet->answer);
	printf("  Valid: %c \n", response_packet->valid);
	
   /* if( (char)response_packet->valid == '1' )
        printf("Is Valid");
    else
        printf("Not Valid"); */
}
void creat_buffer_to_send(struct Request *request_packet, char *buffer_out[])
{
	unsigned char temp[4];
  	memcpy(&temp[0], &request_packet->a, 4);

	int i=0;
	for( i=0; i<4; i++ )
		printf("a: %d", temp[i]);

	//char* temp = (char*)request_packet->a;
	//char* temp2 = (char*)request_packet->b;
	memcpy(&buffer_out[0], &request_packet[0], 1);
	memcpy(&buffer_out[1], &temp[0], 4);
//	memcpy(&buffer_out[5], &temp2[0], 4);
}
void creat_repsonse_packet(struct Response *response_packet, char data_recieved[])
{
	memcpy(response_packet, data_recieved, RESPONSE_SIZE);
	memcpy(&response_packet->answer, &data_recieved[9], 4);
	response_packet->answer = ntohl(response_packet->answer);
	response_packet->valid = data_recieved[13];
}

/* Forwarding Agent for Group 4, Lab 2 */

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

#define MAXBUFLEN 1038

/*********************************************
    1. Take a port number Fx as argument
    2. Read incoming datagrams on Fx
    3. Extract Id and Pd
    4. Forward the datagram AS IS to Id on Pd

    Packet formation
    ----------------
    Pd - Target Port (16 bits)
    Ps - Source Port (16 bits)
    Id - Target Destination (32 bits)
    Is - Source Destination (32 bits)
***********************************************/

enum IPSelector {
	TARGET_IP,
	SOURCE_IP
};

enum PortSelector {
	TARGET_PORT,
	SOURCE_PORT
};

void *get_in_addr(struct sockaddr *sa);
char *getAddressFromM(char* message, enum IPSelector IPAddressSelector);
char *getPortFromM(char* message, enum PortSelector PortNumberSelector);
void forwardMessage(char* message, int messageLength);

int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    if( argc < 2 ) {
    	printf("Usage - fwd.o [Listening Port Number]\n");
    	return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("listener: waiting to recvfrom...\n");
	
	addr_len = sizeof their_addr;

    while(1) {
	    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
	        perror("recvfrom");
	        exit(1);
	    }

	    printf( "Forwarding to: %s:%s\n", getAddressFromM(buf, TARGET_IP), getPortFromM(buf, TARGET_PORT) );

	    forwardMessage(buf, numbytes);
	}

    close(sockfd);

    return 0;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

char *getAddressFromM(char* message, enum IPSelector IPAddressSelector) {
	int i=0, k=0, iOffset = 0;
    char *selectedHostname = (char*)malloc( sizeof(char) * 15 );
	char *ipFragment = (char*) malloc( sizeof(char) * 3 );

	if( IPAddressSelector == TARGET_IP )
		iOffset = 4;
	else if( IPAddressSelector == SOURCE_IP)
		iOffset = 8;

    for( i=0; i <= 3; i++ ) {    
    	memset(ipFragment, 0, sizeof(char) * 3);	
    	sprintf(ipFragment, "%d", message[4 + i]);
    	
    	int j;
    	for( j=0; j<3; j++) {
	    	if( ipFragment[j] != '\0' ) {
	    		selectedHostname[k++] = ipFragment[j];
	    	}
	    }

	    if( i<3 ) selectedHostname[k++] = '.';

    }

    free(ipFragment);

    return selectedHostname;
}

char *getPortFromM(char* message, enum PortSelector PortNumberSelector) {
	int i=0, k=0, iOffset = 0;
	int selectedPort = 0;
	char *cSelectedPort = (char*)malloc(sizeof(char) * 5);

	if( PortNumberSelector == TARGET_PORT )
		iOffset = 0;
	else if( PortNumberSelector == SOURCE_PORT)
		iOffset = 2;

	memcpy(&selectedPort, &message[iOffset], sizeof(int));
	
	sprintf(cSelectedPort, "%d", ntohs(selectedPort));

    return cSelectedPort;
}

void forwardMessage(char* message, int messageLength) {
	struct addrinfo hintsClient;
	memset(&hintsClient,0,sizeof(hintsClient));
	hintsClient.ai_family=AF_UNSPEC;
	hintsClient.ai_socktype=SOCK_DGRAM;
	hintsClient.ai_protocol=0;
	hintsClient.ai_flags=AI_ADDRCONFIG;
	struct addrinfo* res=0;
	
	int err=getaddrinfo( getAddressFromM(message, TARGET_IP), getPortFromM(message, TARGET_PORT), &hintsClient, &res );
	if (err!=0) {
	    perror("failed to resolve remote socket address");
	}

	int fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	if (fd==-1) {
	    perror("Forwarding bind");
	}

	if (sendto(fd,message,messageLength,0, res->ai_addr, res->ai_addrlen) == -1) {
	    perror("Forwarding send");
	}

	close(fd);

	return;
}
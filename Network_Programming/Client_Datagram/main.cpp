#include <stdio.h>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVERPORT "4950"

int main(int argc, char **argv)
{
	//set up the type of addresses we want returned.
	addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = 0;
	
	char ip[] = "192.168.1.200";
	
	//get server info
	addrinfo* serverInfo;
	int gaiError = getaddrinfo(ip, SERVERPORT, &hints, &serverInfo);
	if(gaiError != 0)
	{
		printf("getaddrinfo error: %s\n", gai_strerror(gaiError));
		exit(1);
	}
	
	//loop through the results and bind to the first we can
	addrinfo* pAddrInfos = serverInfo;
	int socketfd;
	while(pAddrInfos != nullptr)
	{
		socketfd = socket(pAddrInfos->ai_family, pAddrInfos->ai_socktype, pAddrInfos->ai_protocol);
		if(socketfd == -1)
		{
			perror("talker: socket");
			pAddrInfos = pAddrInfos->ai_next;
			continue;
		}
		
		break;
	}
	
	if(pAddrInfos == nullptr)
	{
		printf("talker: failed to create socket");
		exit(1);
	}
	
	int numBytes;
	char msg[] = "Hello World! Datagram Style";
	
	numBytes = sendto(socketfd, msg, sizeof(msg), 0, pAddrInfos->ai_addr, pAddrInfos->ai_addrlen);
	if(numBytes == -1)
	{
		perror("talker: sendto");
		freeaddrinfo(serverInfo);
		exit(1);
	}
	

	//the struct is no longer needed
	freeaddrinfo(serverInfo);
	
	printf("talker sent %d bytes to %s\n", numBytes, ip);
	
	close(socketfd);
	
	return 0;
}

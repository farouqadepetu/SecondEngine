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

#define MYPORT "4950"
#define MAXBUFLEN 100

int main(int argc, char **argv)
{
	//set up the type of addresses we want returned.
	addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE; //want to use in the bind function
	
	//get server info
	addrinfo* serverInfo; 
	int gaiError = getaddrinfo(nullptr, MYPORT, &hints, &serverInfo);
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
			perror("listener: socket");
			pAddrInfos = pAddrInfos->ai_next;
			continue;
		}
		
		int bindError = bind(socketfd, pAddrInfos->ai_addr, pAddrInfos->ai_addrlen);
		if(bindError == -1)
		{
			close(socketfd);
			perror("listener: bind");
			pAddrInfos = pAddrInfos->ai_next;
			continue;
		}
		
		break;
	}
	
	if(pAddrInfos == nullptr)
	{
		printf("listener: failed to bind");
		exit(1);
	}
	
	//the struct is no longer needed
	freeaddrinfo(serverInfo);
	
	socklen_t addrLen;
	int numBytes;
	char buf[MAXBUFLEN];
	sockaddr_storage theirAddr;
	
	printf("listener: waiting to recfrom..\n");
	addrLen = sizeof(sockaddr_storage);
	numBytes = recvfrom(socketfd, buf, MAXBUFLEN - 1, 0, (sockaddr*)&theirAddr, &addrLen);
	if(numBytes == -1)
	{
		perror("recvfrom");
		exit(1);
	}
	
	buf[numBytes] = '\0';
	char str[INET6_ADDRSTRLEN];
	void* addr;
	sockaddr_in6* ipv6Addr = (struct sockaddr_in6*)&theirAddr;
	addr = &(ipv6Addr->sin6_addr);
	inet_ntop(theirAddr.ss_family, addr, str, sizeof(str));
	printf("listener: got packet from %s\n", str);
	printf("listener: packet is %d bytes long\n", numBytes);
	printf("listener: packet contains %s\n", buf);
	
	close(socketfd);
	
	return 0;
}

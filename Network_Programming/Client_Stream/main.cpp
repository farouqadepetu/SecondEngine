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

#define PORT "3490"
#define MAXDATASIZE 100 //max number of bytes we can recieve at once

int main(int argc, char **argv)
{
	//set up the type of addresses we want returned.
	addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	//get server info
	addrinfo* serverInfo;
	int gaiError = getaddrinfo("192.168.1.200", PORT, &hints, &serverInfo);
	if(gaiError != 0)
	{
		printf("getaddrinfo error: %s\n", gai_strerror(gaiError));
		exit(1);
	}
	
	//loop through the results and bind to the first we can
	addrinfo* pAddrInfos = serverInfo;
	int socketfd;
	void* addr;
	char ipAddrStr[INET6_ADDRSTRLEN];
	while(pAddrInfos != nullptr)
	{
		socketfd = socket(pAddrInfos->ai_family, pAddrInfos->ai_socktype, pAddrInfos->ai_protocol);
		if(socketfd == -1)
		{
			perror("client: socket");
			pAddrInfos = pAddrInfos->ai_next;
			continue;
		}
		
		if(pAddrInfos->ai_family == AF_INET)
		{
			sockaddr_in* ipv4Addr = (struct sockaddr_in*)pAddrInfos->ai_addr;
			addr = &(ipv4Addr->sin_addr);
		}
		else if(pAddrInfos->ai_family == AF_INET6)
		{
			sockaddr_in6* ipv6Addr = (struct sockaddr_in6*)pAddrInfos->ai_addr;
			addr = &(ipv6Addr->sin6_addr);
		}
		
		inet_ntop(pAddrInfos->ai_family, addr,  ipAddrStr, INET6_ADDRSTRLEN);
		printf("client: attempting connection to %s\n", ipAddrStr);
		
		int connectError = connect(socketfd, pAddrInfos->ai_addr, pAddrInfos->ai_addrlen);
		if(connectError == -1)
		{
			perror("client: connect");
			pAddrInfos = pAddrInfos->ai_next;
			continue;
		}
		
		printf("client: connection to %s successful\n", ipAddrStr);
		break;
	}
	
	if(pAddrInfos == nullptr)
	{
		printf("connect: failed to connect");
		exit(1);
	}
	
	//the struct is no longer needed
	freeaddrinfo(serverInfo);
	
	char buf[MAXDATASIZE];
	int numbytes = recv(socketfd, buf, MAXDATASIZE - 1, 0);
	if(numbytes == -1)
	{
		perror("recv");
		exit(1);
	}
	
	buf[numbytes] = '\0';
	
	printf("client : recieved %s\n", buf);
	
	close(socketfd);
	
	return 0;
}

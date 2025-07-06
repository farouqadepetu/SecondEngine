#include <stdio.h>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>

#define PORT "3490"
#define MAX_DATA_SIZE 100 //max number of bytes we can recieve at once
#define MAX_EVENTS 5

int main(int argc, char **argv)
{
	//set up the type of addresses we want returned.
	addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = 0;
	
	//get server info
	addrinfo* serverInfo;
	int gaiError = getaddrinfo("192.168.1.204", PORT, &hints, &serverInfo);
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
	
	//set the socket to non-blocking
	int fnctlError = fcntl(socketfd, F_SETFL, O_NONBLOCK);
	if(fnctlError == -1)
	{
		perror("fcntl");
		exit(1);
	}
	
	//the struct is no longer needed
	freeaddrinfo(serverInfo);
	
	//create epoll instance
	int epoll_fd = epoll_create1(0);
	if(epoll_fd == -1)
	{
		perror("epoll_create1");
		exit(1);
	}
	
	//connectedPoll
	epoll_event connectedPoll;
	connectedPoll.data.fd = socketfd;
	connectedPoll.events = EPOLLIN; ////an event is raised when their data to recv
	
	int epollCtlError = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socketfd, &connectedPoll);
	if(epollCtlError == -1)
	{
		perror("epoll_ctl");
		exit(1);
	}
	
	epoll_event events[MAX_EVENTS];
	bool running = true;
	int j = 0;
	while(running)
	{
		//block until an event happens
		int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if(event_count == -1)
		{
			perror("event_count");
			exit(1);
		}
		
		printf("client: an event happened. Checking...\n");
		epoll_event modifiedPoll;
		for(int i = 0; i < event_count; ++i)
		{
			printf("event = %d\n", events[i].events);
			if(events[i].events & EPOLLIN)
			{
				//ready to read data
				int numbytes;
				if(j == 0)
				{
					char data[MAX_DATA_SIZE];
					numbytes = recv(socketfd, data, MAX_DATA_SIZE - 1, 0);
					if(numbytes == -1)
					{
						perror("recv");
						exit(1);
					}
					data[numbytes] = '\0';
					printf("client : recieved string %s\n", data);
					++j;
				}
				else if (j == 1)
				{
					uint32_t data;
					numbytes = recv(socketfd, &data, sizeof(uint32_t), 0);
					if(numbytes == -1)
					{
						perror("recv");
						exit(1);
					}
					uint32_t numU = ntohl(data);
					printf("client : recieved uint %d\n", numU);
					++j;
				}
				else if(j == 2)
				{
					uint32_t data;
					numbytes = recv(socketfd, &data, sizeof(uint32_t), 0);
					if(numbytes == -1)
					{
						perror("recv");
						exit(1);
					}
					int numI = (int)ntohl(data);
					printf("client : recieved int %d\n", numI);
					++j;
				}
				else if(j == 3)
				{
					uint32_t data;
					numbytes = recv(socketfd, &data, sizeof(uint32_t), 0);
					if(numbytes == -1)
					{
						perror("recv");
						exit(1);
					}
					data = ntohl(data);
					float numF;
					memcpy(&numF, &data, 4);
					printf("client : recieved float %f\n", numF);
					++j;
				}
				else if(j == 4)
				{
					uint64_t data;
					numbytes = recv(socketfd, &data, sizeof(uint64_t), 0);
					if(numbytes == -1)
					{
						perror("recv");
						exit(1);
					}
					data = be64toh(data);
					double numD;
					memcpy(&numD, &data, 8);
					printf("client : recieved double %lf\n", numD);
					++j;
				}
				
				modifiedPoll.data.fd = socketfd;
				modifiedPoll.events = EPOLLOUT | EPOLLET; //an event is raised when you can send data w.o blocking
				epollCtlError = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, socketfd, &modifiedPoll);
				if(epollCtlError == -1)
				{
					perror("epoll_ctl");
					exit(1);
				}
			}
			else if(events[i].events & EPOLLOUT)
			{
				printf("client : sending ack\n");
				int sendError = send(socketfd, "ACK\n", 4, 0);
				if(sendError == -1)
				{
					perror("send");
				}
				
				modifiedPoll.data.fd = socketfd;
				modifiedPoll.events = EPOLLIN;
				epollCtlError = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, socketfd, &modifiedPoll);
				if(epollCtlError == -1)
				{
					perror("epoll_ctl");
					exit(1);
				}
				
				if(j == 5)
					running = false;
			}
		}
	}
	
	close(socketfd);
	
	return 0;
}

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
#include "../Chat_Server/Network.h"

#define PORT "3490"
#define MAX_DATA_SIZE 100 //max number of bytes we can recieve at once
#define MAX_EVENTS 5

int main(int argc, char **argv)
{
	AddressInfo info;
	Addresses addresses;
	info.family = FAMILY_UNSPEC;
	info.type = SOCKET_TYPE_STREAM;
	info.prot = PROTOCOL_TCP;
	info.flags = ADDRESS_INFO_FLAG_NONE | ADDRESS_INFO_FLAG_NUMERIC_HOST;
	info.name = "192.168.1.204";
	info.port = PORT;
	
	int error = GetAddresses(&info, &addresses);
	if(error == -1)
	{
		perror("GetAddresses");
		exit(1);
	}
	PrintAddresses(&addresses);
	char buffer[ADDRESS_STRLEN];
	GetAddress(&addresses, buffer);
	printf("1st address = %s\n", buffer);
	
	Socket socket;
	Addresses outAddress;
	error = CreateSocket(&socket, &addresses, &outAddress);
	if(error == -1)
	{
		perror("CreateSocket");
		exit(1);
	}
	
	error = Connect(&socket, &outAddress);
	if(error == -1)
	{
		perror("Connect");
		exit(1);
	}
	FreeAddresses(&addresses);
	
	SocketEvent socketEvent;
	SocketEventInfo eventInfo;
	eventInfo.socketfd = socket.socketfd;
	eventInfo.events = EVENT_READ;
	error = CreateSocketEvent(&eventInfo, &socketEvent);
	if(error == -1)
	{
		perror("CreateSocketEvent");
		exit(1);
	}
	
	error = SetToNonBlock(&socket);
	if(error == -1)
	{
		perror("SetToNonBlock");
		exit(1);
	}
	
	bool running = true;
	while(running)
	{
		WaitForEvent(&socketEvent);
		printf("client: an event happened. Checking...\n");
		if(socketEvent.event & EVENT_READ)
		{
			char data[MAX_DATA_SIZE];
			int numBytes = Recieve(&socket, data, MAX_DATA_SIZE - 1);
			if(numBytes == -1)
			{
				perror("Recieve");
				exit(1);
			}
			if(numBytes == 0)
			{
				//connection was closed
				running = false;
				continue;
			}
			
			data[numBytes] = '\0';
			printf("client: Recieved %s\n", data);
			
			eventInfo.socketfd = socket.socketfd;
			eventInfo.events = EVENT_WRITE | EVENT_EDGE_TRIGGERED;
			error = ModifySocketEvent(&eventInfo, &socketEvent);
			if(error == -1)
			{
				perror("ModifySocketEvent");
				exit(1);
			}
			
		}
		if(socketEvent.event & EVENT_WRITE)
		{
			char buffer[]= "ACK\n";
			error = Send(&socket, buffer, strlen(buffer));
			if(error == -1)
			{
				perror("Send");
				exit(1);
			}
			
			eventInfo.socketfd = socket.socketfd;
			eventInfo.events = EVENT_READ | EVENT_EDGE_TRIGGERED;
			error = ModifySocketEvent(&eventInfo, &socketEvent);
			if(error == -1)
			{
				perror("ModifySocketEvent");
				exit(1);
			}
			
			printf("Closing connection\n");
			running = false;
		}
	}
	
	CloseSocketEvent(&socketEvent);
	CloseSocket(&socket);
	
	
	//set the socket to non-blocking
	/*int fnctlError = fcntl(socketfd, F_SETFL, O_NONBLOCK);
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
	
	close(socketfd);*/
	
	return 0;
}

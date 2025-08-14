#include <stdio.h>
#include <cstdlib>

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <syscall.h>
#include <string.h>
#include "../Chat_Server/Thread.h"
#include "../Chat_Server/Network.h"


#define MYPORT "3490"
#define BACKLOG 10
#define MAX_DATA_SIZE 100 //max number of bytes we can recieve at once
#define MAX_EVENTS 5

struct HandleConnectionData
{
	Socket connectionSocket;
};

void HandleConnection(void* ptr)
{
	HandleConnectionData* data;
	data = (HandleConnectionData*)ptr;
	
	SocketEvent socketEvent;
	SocketEventInfo eventInfo;
	eventInfo.socketfd = data->connectionSocket.socketfd;
	eventInfo.events = EVENT_WRITE | EVENT_EDGE_TRIGGERED;
	int error = CreateSocketEvent(&eventInfo, &socketEvent);
	if(error == -1)
	{
		perror("CreateSocketEvent");
		ExitThread();
		return;
	}
	
	error = SetToNonBlock(&data->connectionSocket);
	if(error == -1)
	{
		perror("SetToNonBlock");
		ExitThread();
		return;
	}

	while(true)
	{
		//block until an event happens
		error = WaitForEvent(&socketEvent);
		if(error == -1)
		{
			perror("WaitForEvent");
			ExitThread();
			return;
		}
		
		printf("server:thread %d an event happened! Checking..\n", gettid());
		if(socketEvent.event & EVENT_WRITE)
		{
			char msg[] = "Hello World!";
			printf("Sending string Hello World!\n");
			error = Send(&data->connectionSocket, msg, strlen(msg));
			if(error == -1)
			{
				perror("Send");
				ExitThread();
				return;
			}
			
			eventInfo.socketfd = data->connectionSocket.socketfd;
			eventInfo.events = EVENT_READ;
			error = ModifySocketEvent(&eventInfo, &socketEvent);
			if(error == -1)
			{
				perror("SetToNonBlock");
				ExitThread();
				return;
			}
		}
		if(socketEvent.event & EVENT_READ)
		{
			//Handle errors
			char buffer[MAX_DATA_SIZE];
			int numBytes = Recieve(&data->connectionSocket, buffer, MAX_DATA_SIZE - 1);
			if(numBytes == -1)
			{
				perror("Recieve");
				ExitThread();
			}
			if(numBytes == 0)
			{
				//connection was closed
				CloseSocketEvent(&socketEvent);
				CloseSocket(&data->connectionSocket);
				free(data);
				ExitThread();
				return;
			}
			
			buffer[numBytes] = '\0';
			printf("server: Recieved %s", buffer);
			
			eventInfo.socketfd = data->connectionSocket.socketfd;
			eventInfo.events = EVENT_WRITE | EVENT_EDGE_TRIGGERED;
			error = ModifySocketEvent(&eventInfo, &socketEvent);
			if(error == -1)
			{
				perror("ModifySocketEvent");
				ExitThread();
				return;
			}
		}
		
		/*for(int i = 0; i < event_count; ++i)
		{
			printf("event = %d\n", events[i].events);
			
			if(events[i].data.fd == data->listenerSocket)
				continue;
				
			if(events[i].events & EPOLLOUT)
			{
				char msg[] = "Hello World!";
				uint32_t numU = 349;
				int numI = -5039292;
				float numF = 50.983f;
				double numD = 987.3212;
				
				printf("Sending string Hello World!\n");
				int sendError = send(data->connectionSocket, msg, sizeof(msg), 0);
				if(sendError == -1)
				{
					perror("send");
				}
				
				printf("Sending uint32_t 349\n");
				uint32_t dataU = htonl(numU);
				sendError = send(data->connectionSocket, &dataU, sizeof(uint32_t), 0);
				if(sendError == -1)
				{
					perror("send");
				}
				
				printf("Sending int -5039292\n");
				int dataI = htonl(numI);
				sendError = send(data->connectionSocket, &dataI, sizeof(int), 0);
				if(sendError == -1)
				{
					perror("send");
				}
				
				printf("Sending float 50.983f\n");
				uint32_t tempF;
				memcpy(&tempF, &numF, sizeof(float));
				uint32_t dataF = htonl(tempF);
				sendError = send(data->connectionSocket, &dataF, sizeof(uint32_t), 0);
				if(sendError == -1)
				{
					perror("send");
				}
				
				printf("Sending double 987.3212\n");
				uint64_t tempD;
				memcpy(&tempD, &numD, sizeof(double));
				uint64_t dataD = htobe64(tempD);
				sendError = send(data->connectionSocket, &dataD, sizeof(uint64_t), 0);
				if(sendError == -1)
				{
					perror("send");
				}
				
				printf("server:thread %d sent data\n", gettid());
				
				printf("server:thread waiting for ack\n");
				
				connectedPoll.data.fd = data->connectionSocket;
				connectedPoll.events = EPOLLIN;
				
				int epollCtlError = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, data->connectionSocket, &connectedPoll);
				if(epollCtlError == -1)
				{
					perror("epoll_ctl");
					pthread_exit(nullptr);
				}
			}
			else if(events[i].events & EPOLLIN)
			{
				char buf[MAX_DATA_SIZE];
				int numbytes = recv(data->connectionSocket, buf, 4, 0);
				if(numbytes == -1)
				{
					//error
					perror("recv");
					pthread_exit(nullptr);
				}
				else if(numbytes == 0)
				{
					//client closed connection
					printf("server:thread %d client closed connection, exiting\n", gettid());
					close(data->connectionSocket);
					close(epoll_fd);
					free(data);
					ExitThread();
					return;
				}
				else
				{
					//data was recieved
					buf[numbytes] = '\0';
					printf("server:thread %d recieved %s\n", gettid(), buf);
				}
			}
		}*/
	}
}

int main(int argc, char **argv)
{
	AddressInfo info;
	Addresses addresses;
	info.family = FAMILY_UNSPEC;
	info.type = SOCKET_TYPE_STREAM;
	info.prot = PROTOCOL_TCP;
	info.flags = ADDRESS_INFO_FLAG_PASSIVE;
	info.name = nullptr;
	info.port = MYPORT;
	
	int error = GetAddresses(&info, &addresses);
	if(error == -1)
	{
		perror("GetAddresses");
		exit(1);
	}
	
	Socket listeningSocket;
	Addresses outAddress;
	error = CreateSocket(&listeningSocket, &addresses, &outAddress);
	if(error == -1)
	{
		perror("CreateSocket");
		exit(1);
	}
	
	error = Bind(&listeningSocket, &outAddress);
	if(error == -1)
	{
		perror("Bind");
		exit(1);
	}
	
	FreeAddresses(&addresses);
	
	error = Listen(&listeningSocket, BACKLOG);
	if(error == -1)
	{
		perror("Bind");
		exit(1);
	}
	
	error = SetToNonBlock(&listeningSocket);
	if(error == -1)
	{
		perror("SetToNonBlock");
		exit(1);
	}
	
	SocketEvent socketEvent;
	SocketEventInfo eventInfo;
	eventInfo.socketfd = listeningSocket.socketfd;
	eventInfo.events = EVENT_READ;
	error = CreateSocketEvent(&eventInfo, &socketEvent);
	if(error == -1)
	{
		perror("CreateSocketEvent");
		exit(1);
	}
	
	InitThreadSystem();
	printf("server: join thread created\n");
	
	printf("Waiting for connections...\n");
	
	//epoll_event events[MAX_EVENTS];
	while(true)
	{
		//block until an event happens
		error = WaitForEvent(&socketEvent);
		if(error == -1)
		{
			perror("WaitForEvent");
			exit(1);
		}
		
		
		printf("server: an event happened. Checking...\n");
		Socket connectionSocket;
		error = Accept(&listeningSocket, &connectionSocket);
		if(error == -1)
		{
			perror("Accept");
			exit(1);
		}
		
		Thread connectionThread;
		HandleConnectionData* data = (HandleConnectionData* )calloc(1, sizeof(HandleConnectionData));
		data->connectionSocket = connectionSocket;
		ThreadInfo info;
		info.pFunc = HandleConnection;
		info.pData = data;
		int result = CreateThread(&info, &connectionThread);
		if(result != 0)
		{
			printf("Erorr creating a connectionThread, exiting program");
			exit(1);
		}
		printf("server: connection thread created\n");
		
		/*for(int i = 0; i < event_count; ++i)
		{
			printf("server: event = %d\n", events[i].events);
			if(events[i].data.fd == listenerSocket)
			{
				//accept new connection
				int connectionSocket = NewConnection(&listenerSocket);
				if(connectionSocket == -1)
				{
					exit(1);
				}
				
				Thread connectionThread;
				HandleConnectionData* data = (HandleConnectionData* )calloc(1, sizeof(HandleConnectionData));
				data->listenerSocket = listenerSocket;
				data->connectionSocket = connectionSocket;
				ThreadInfo info;
				info.pFunc = HandleConnection;
				info.pData = data;
				int result = CreateThread(&info, &connectionThread);
				if(result != 0)
				{
					printf("Erorr creating a connectionThread, exiting program");
					exit(1);
				}
				printf("server: connection thread created\n");
			}
		}*/
	}
	
	ExitThreadSystem();
	CloseSocketEvent(&socketEvent);
	CloseSocket(&listeningSocket);
	
	return 0;
}

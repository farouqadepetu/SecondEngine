#include "../Network/Network.h"
#include "../Chat/ChatPacket.h"
#include <stdio.h>
#include <stdlib.h>

#define MYPORT "3490"
#define BACKLOG 10


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
	
	//Create a socket from the first address we can
	Socket listeningSocket;
	Address outAddress;
	for(uint32_t i = 0; i < addresses.numAddresses; ++i)
	{
		GetAddress(&addresses, &outAddress, i);
		error = CreateSocket(&listeningSocket, &outAddress);
		if(error == 0)
			break;
	}
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
	eventInfo.events = EVENT_RECEIVE;
	error = CreateSocketEvent(&eventInfo, &socketEvent);
	if(error == -1)
	{
		perror("CreateSocketEvent");
		exit(1);
	}
	
	printf("Chat Server is up and running\n");
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
		if(socketEvent.event & EVENT_RECEIVE)
		{
			error = Accept(&listeningSocket, &connectionSocket);
			if(error == -1)
			{
				perror("Accept");
				exit(1);
			}
		}
	
		//PUT IN DIFFERENT THREAD
		SocketEvent socketEvent2;
		eventInfo.socketfd = connectionSocket.socketfd;
		eventInfo.events = EVENT_RECEIVE;
		int error = CreateSocketEvent(&eventInfo, &socketEvent2);
		if(error == -1)
		{
			perror("CreateSocketEvent");
			exit(1);
		}
		
		error = SetToNonBlock(&connectionSocket);
		if(error == -1)
		{
			perror("SetToNonBlock");
			exit(1);
		}

		while(true)
		{
			//block until an event happens
			error = WaitForEvent(&socketEvent2);
			if(error == -1)
			{
				perror("WaitForEvent");
				exit(1);
			}
			
			if(socketEvent.event & EVENT_RECEIVE)
			{
				ChatPacket packet;
				RecieveChatPacket(&connectionSocket, &packet);
				CloseSocketEvent(&socketEvent);
				CloseSocketEvent(&socketEvent2);
				CloseSocket(&connectionSocket);
				CloseSocket(&listeningSocket);
				return 0;
			}
		}
	}
		
	return 0;
}

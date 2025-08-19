#include "../Network/Network.h"
#include "../Chat/ChatPacket.h"
#include "../Thread/Thread.h"
#include "../../ThirdParty/stb_ds.h"
#include <stdio.h>
#include <stdlib.h>

#define MYPORT "12349"
#define BACKLOG 10
#define MAX_NUM_CONNECTIONS 16

Socket* pConnectionSockets = nullptr;

struct ThreadData
{
	Socket connectionSocket;
};

void HandleConnection(void* ptr)
{
	ThreadData* data = (ThreadData*)ptr;
	
	arrpush(pConnectionSockets, data->connectionSocket);
	
	SocketEvent socketEvent;
	SocketEventInfo eventInfo;
	eventInfo.socketfd = data->connectionSocket.socketfd;
	eventInfo.events = EVENT_RECEIVE;
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

	ChatPacket packet;
	while(true)
	{
		//block until an event happens
		error = WaitForEvent(&socketEvent);
		if(error == -1)
		{
			perror("WaitForEvent: inside a connection thread");
			break;
		}
		
		if(socketEvent.event & EVENT_RECEIVE)
		{
			int error = ReceiveChatPacket(&data->connectionSocket, &packet);
			if(error == -1)
			{
				perror("ReceiveChatPacket");
				break;
			}
			if(error == 2)
			{
				//connection was closed
				for(uint32_t i = 0; i < arrlenu(pConnectionSockets); ++i)
				{
					if(pConnectionSockets[i].socketfd == data->connectionSocket.socketfd)
					{
						arrdel(pConnectionSockets, i);
						break;
					}
				}
				break;
			}
			printf("%s: %s\n", packet.name.str, packet.msg.str);
			
			//Send to other clients
			for(uint32_t i = 0; i < arrlenu(pConnectionSockets); ++i)
			{
				if(pConnectionSockets[i].socketfd != data->connectionSocket.socketfd)
				{
					error = SendChatPacket(&pConnectionSockets[i], &packet);
					if(error == -1)
					{
						perror("SendChatPacket: Erorr sending to other clients");
						break;
					}
				}
			}
		}
	}
	
	free(data);
	CloseSocketEvent(&socketEvent);
	CloseSocket(&data->connectionSocket);
	FreeChatString(&packet.name);
	FreeChatString(&packet.msg);
	ExitThread();
	return;
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
	
	InitThreadSystem();
	
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
			
			InitThreadSystem();
	
			Thread connectionThread;
			ThreadInfo tInfo{};
			ThreadData* data = (ThreadData*)calloc(1, sizeof(ThreadData));
			data->connectionSocket = connectionSocket;
			tInfo.pFunc = HandleConnection;
			tInfo.pData = data;
			int result = CreateThread(&tInfo, &connectionThread);
			if(result != 0)
			{
				printf("Erorr creating a connectionThread, exiting program");
				CloseSocketEvent(&socketEvent);
				CloseSocket(&listeningSocket);
				exit(1);
			}
		}
	}
	
	arrfree(pConnectionSockets);
	ExitThreadSystem();
	return 0;
}

#define STB_DS_IMPLEMENTATION
#include "../../ThirdParty/stb_ds.h"

#include "../Network/Network.h"
#include "../Chat/ChatPacket.h"
#include "../Thread/Thread.h"
#include <stdio.h>
#include <stdlib.h>


#define MYPORT "12349"
#define BACKLOG 10
#define MAX_NUM_CONNECTIONS 16

Socket* pConnectionSockets = nullptr;

struct ThreadData
{
	SocketEvent* pSocketEvent;
	Socket listeningSocket;
};

void ChatThread(void* ptr)
{
	ThreadData* data = (ThreadData*)ptr;
	ChatPacket packet;
	while(true)
	{
		//block until an event happens
		int error = WaitForEvent(data->pSocketEvent);
		if(error == -1)
		{
			perror("WaitForEvent: inside chat thread");
			break;
		}
		
		for(uint32_t i = 0; i < data->pSocketEvent->numFds; ++i)
		{
			Socket currentSocket = GetSocket(data->pSocketEvent, i);
			if(currentSocket.socketfd == data->listeningSocket.socketfd)
			{
				continue;
			}
			else
			{
				uint32_t event = CheckEvent(data->pSocketEvent, i);
				if(event & EVENT_RECEIVE)
				{
					error = ReceiveChatPacket(&currentSocket, &packet);
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
							if(pConnectionSockets[i].socketfd == currentSocket.socketfd)
							{
								error = RemoveSocket(data->pSocketEvent, &currentSocket);
								if(error == -1)
								{
									perror("RemoveSocket");
									break;
								}
								arrdel(pConnectionSockets, i);
								break;
							}
						}
						break;
					}
					else //recieved packet successfully
					{
						//Send to other clients
						for(uint32_t i = 0; i < arrlenu(pConnectionSockets); ++i)
						{
							if(pConnectionSockets[i].socketfd != currentSocket.socketfd)
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
					Clear(&packet.name);
					Clear(&packet.msg);
				}
			}
		}
	}
	free(data);
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
		perror("Listen");
		exit(1);
	}
	
	error = SetToNonBlock(&listeningSocket);
	if(error == -1)
	{
		perror("SetToNonBlock");
		exit(1);
	}
	
	SocketEvent socketEvent;
	error = CreateSocketEvent(&socketEvent);
	if(error == -1)
	{
		perror("CreateSocketEvent");
		exit(1);
	}
	
	error = AddSocket(&socketEvent, &listeningSocket, EVENT_RECEIVE);
	if(error == -1)
	{
		perror("AddSocket");
		exit(1);
	}
	
	InitThreadSystem();
	
	Thread chatThread;
	ThreadInfo tInfo{};
	ThreadData* data = (ThreadData*)calloc(1, sizeof(ThreadData));
	data->pSocketEvent = &socketEvent;
	data->listeningSocket = listeningSocket;
	tInfo.pFunc = ChatThread;
	tInfo.pData = data;
	int result = CreateThread(&tInfo, &chatThread);
	if(result != 0)
	{
		printf("Erorr creating a chatThread, exiting program");
		CloseSocketEvent(&socketEvent);
		CloseSocket(&listeningSocket);
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
		for(uint32_t i = 0; i < socketEvent.numFds; ++i)
		{
			Socket socket = GetSocket(&socketEvent, i);
			if(socket.socketfd == listeningSocket.socketfd)
			{
				uint32_t event = CheckEvent(&socketEvent, i);
				if(event & EVENT_RECEIVE)
				{
					error = Accept(&listeningSocket, &connectionSocket);
					if(error == -1)
					{
						perror("Accept");
						exit(1);
					}
					arrpush(pConnectionSockets, connectionSocket);
					error = AddSocket(&socketEvent, &connectionSocket, EVENT_RECEIVE);
					if(error == -1)
					{
						perror("AddSocket");
						exit(1);
					}
				}
			}
		}
	}
	
	arrfree(pConnectionSockets);
	ExitThreadSystem();
	return 0;
}

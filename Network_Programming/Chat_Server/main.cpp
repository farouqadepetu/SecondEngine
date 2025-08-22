#define STB_DS_IMPLEMENTATION
#include "../../ThirdParty/stb_ds.h"

#include "../Network/Network.h"
#include "../Chat/ChatPacket.h"
#include "../Thread/Thread.h"
#include <stdio.h>
#include <stdlib.h>


#define MYPORT "12349"
#define BACKLOG 10

Socket* pConnectionSockets = nullptr;
SocketEvent gChatThreadSE;

void ChatThread(void* ptr)
{
	ChatPacket packet;
	while(true)
	{
		int n = WaitForEvent(&gChatThreadSE);
		if(n == -1)
		{
			perror("WaitForEvent");
			exit(1);
		}
		printf("server: chatThread: an event happened. Checking...\n");
		
		for(uint32_t i = 0; i < n; ++i)
		{
			Socket currentSocket = GetSocket(&gChatThreadSE, i);
			uint32_t event = CheckEvent(&gChatThreadSE, i);
			
			if(event & EVENT_RECEIVE)
			{
				int numBytesReceived = ReceiveChatPacket(&currentSocket, &packet);
				if(numBytesReceived == -1)
				{
					perror("ReceiveChatPacket");
					break;
				}
				if(numBytesReceived == -2)
				{
					//connection was closed
					for(uint32_t i = 0; i < arrlenu(pConnectionSockets); ++i)
					{
						if(pConnectionSockets[i].socketfd == currentSocket.socketfd)
						{
							int error = RemoveSocket(&gChatThreadSE, &currentSocket);
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
							int numBytesSent = SendChatPacket(&pConnectionSockets[i], &packet);
							if(numBytesSent == -1)
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
	
	SocketEvent listeningSE;
	error = CreateSocketEvent(&listeningSE);
	if(error == -1)
	{
		perror("CreateSocketEvent: listeningSE");
		exit(1);
	}
	
	error = CreateSocketEvent(&gChatThreadSE);
	if(error == -1)
	{
		perror("CreateSocketEvent: chatThreadSE");
		exit(1);
	}
	
	error = AddSocket(&listeningSE, &listeningSocket, EVENT_RECEIVE);
	if(error == -1)
	{
		perror("AddSocket");
		exit(1);
	}
	
	InitThreadSystem();
	
	Thread chatThread;
	ThreadInfo tInfo{};
	tInfo.pFunc = ChatThread;
	tInfo.pData = nullptr;
	int result = CreateThread(&tInfo, &chatThread);
	if(result != 0)
	{
		printf("Erorr creating a chatThread, exiting program\n");
		CloseSocketEvent(&listeningSE);
		CloseSocket(&listeningSocket);
		exit(1);
	}
	
	printf("Chat Server is up and running\n");
	printf("Waiting for connections...\n");
	
	while(true)
	{
		int n = WaitForEvent(&listeningSE);
		if(n == -1)
		{
			perror("WaitForEvent");
			exit(1);
		}
		printf("server: mainThread: an event happened. Checking...\n");
		
		for(uint32_t i = 0; i < n; ++i)
		{
			Socket currentSocket = GetSocket(&listeningSE, i);
			uint32_t event = CheckEvent(&listeningSE, i);
			if(event & EVENT_RECEIVE)
			{
				Socket connectionSocket;
				error = Accept(&listeningSocket, &connectionSocket);
				if(error == -1)
				{
					perror("Accept");
					exit(1);
				}
				arrpush(pConnectionSockets, connectionSocket);
				error = AddSocket(&gChatThreadSE, &connectionSocket, EVENT_RECEIVE | EVENT_EDGE_TRIGGERED);
				if(error == -1)
				{
					perror("AddSocket");
					exit(1);
				}
			}
		}
	}
	
	arrfree(pConnectionSockets);
	ExitThreadSystem();
	return 0;
}

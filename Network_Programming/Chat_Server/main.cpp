#define STB_DS_IMPLEMENTATION
#include "../../ThirdParty/stb_ds.h"

#include "../Network/Network.h"
#include "../Chat/ChatPacket.h"
#include "../Thread/Thread.h"
#include <stdio.h>
#include <stdlib.h>


#define MYPORT "12349"
#define BACKLOG 10

struct Chatter
{
	Socket key;
	ChatPacket value;
};
Chatter* pChatter = nullptr;

SocketEvent gChatThreadSE;

void ChatThread(void* ptr)
{
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
				ChatPacket* pPacket = &hmget(pChatter, currentSocket);;
				int numBytesReceived = ReceiveChatPacket(&currentSocket, pPacket);
				if(numBytesReceived == -1)
				{
					perror("ReceiveChatPacket");
					break;
				}
				else if(numBytesReceived == -2)
				{
					//connection was closed
					//Send to other clients that the user is leaving the chat
					pPacket->msgType = LEAVING;
					for(uint32_t j = 0; j < hmlenu(pChatter); ++j)
					{
						Socket* pSocket = &pChatter[j].key;
						int numBytesSent = SendChatPacket(pSocket, pPacket);
						if(numBytesSent == -1)
						{
							perror("SendChatPacket: Erorr sending to other clients");
							break;
						}
					}
					//Remove user
					int error = RemoveSocket(&gChatThreadSE, &currentSocket);
					if(error == -1)
					{
						perror("RemoveSocket");
						break;
					}
					hmdel(pChatter, currentSocket);
					break;
				}
				else //recieved packet successfully
				{
					
					//hmput(pChatter, currentSocket, packet);
					//Send to other clients
					for(uint32_t j = 0; j < hmlenu(pChatter); ++j)
					{
						if(pChatter[j].key.socketfd != currentSocket.socketfd)
						{
							Socket* pSocket = &pChatter[j].key;
							int numBytesSent = SendChatPacket(pSocket, pPacket);
							if(numBytesSent == -1)
							{
								perror("SendChatPacket: Erorr sending to other clients");
								break;
							}
							Clear(&pPacket->msg);
						}
					}
				}
			}
		}
	}

	for(uint32_t i = 0; i < hmlenu(pChatter); ++i)
	{
		FreeChatString(&pChatter[i].value.name);
		FreeChatString(&pChatter[i].value.msg);
	}
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
				hmput(pChatter, connectionSocket, ChatPacket{});
				error = AddSocket(&gChatThreadSE, &connectionSocket, EVENT_RECEIVE | EVENT_EDGE_TRIGGERED);
				if(error == -1)
				{
					perror("AddSocket");
					exit(1);
				}
			}
		}
	}
	
	hmfree(pChatter);
	ExitThreadSystem();
	return 0;
}

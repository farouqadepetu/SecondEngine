#include "../Chat/ChatPacket.h"
#include "../Chat/ChatString.h"
#include "../Network/Network.h"
#include "../Thread/Thread.h"
#include <stdlib.h>

#define PORT "12349"

void ReadInput(ChatString* pChatStr)
{
	char ch = '\0';
	while(true)
	{
		ch = fgetc(stdin);
		
		if(ch == '\n' || ch == EOF || ch == '\r')
			break;
			
		AddChar(pChatStr, ch);
	}
}

struct ThreadData
{
	Socket connectionSocket;
};

void ReceieveFromServer(void* ptr)
{
	ThreadData* data;
	data = (ThreadData*)ptr;
	
	int error = SetToNonBlock(&data->connectionSocket);
	if(error == -1)
	{
		perror("SetToNonBlock");
		CloseSocket(&data->connectionSocket);
		free(data);
		ExitThread();
		return;
	}
	
	SocketEvent socketEvent;
	error = CreateSocketEvent(&socketEvent);
	if(error == -1)
	{
		perror("CreateSocketEvent");
		CloseSocket(&data->connectionSocket);
		free(data);
		ExitThread();
		return;
	}
	
	error = AddSocket(&socketEvent, &data->connectionSocket, EVENT_RECEIVE);
	if(error == -1)
	{
		perror("CreateSocketEvent");
		CloseSocketEvent(&socketEvent);
		CloseSocket(&data->connectionSocket);
		free(data);
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
			perror("WaitForEvent");
			CloseSocketEvent(&socketEvent);
			CloseSocket(&data->connectionSocket);
			free(data);
			ExitThread();
			return;
		}
		
		for(uint32_t i = 0; i < socketEvent.numFds; ++i)
		{
			uint32_t event = CheckEvent(&socketEvent, i);
			if(event & EVENT_RECEIVE)
			{
				int error = ReceiveChatPacket(&data->connectionSocket, &packet);
				if(error == -1)
				{
					perror("Recieve");
					CloseSocketEvent(&socketEvent);
					CloseSocket(&data->connectionSocket);
					free(data);
					ExitThread();
					return;
				}
				if(error == 2)
				{
					//connection was closed
					CloseSocketEvent(&socketEvent);
					CloseSocket(&data->connectionSocket);
					free(data);
					ExitThread();
					return;
				}
				
				//Someone joined the chat
				if(Empty(&packet.msg))
					printf("%s joined the chat\n", packet.name.str);
				else //someone sent a message
					printf("%s: %s\n", packet.name.str, packet.msg.str);
					
				Clear(&packet.name);
				Clear(&packet.msg);
			}
		}
	}
}

int main(int argc, char **argv)
{
	ChatPacket packet;
	
	AllocateChatString(&packet.name);
	printf("Enter your name for this session: ");
	ReadInput(&packet.name);
	
	printf("Name = %s\n", packet.name.str);
	
	AllocateChatString(&packet.msg);
	
	AddressInfo info;
	Addresses addresses;
	info.family = FAMILY_UNSPEC;
	info.type = SOCKET_TYPE_STREAM;
	info.prot = PROTOCOL_TCP;
	info.flags = ADDRESS_INFO_FLAG_NONE;
	info.name = "192.168.1.204";
	info.port = PORT;
	
	int error = GetAddresses(&info, &addresses);
	if(error == -1)
	{
		perror("GetAddresses");
		exit(1);
	}
	printf("Number of addresses = %d\n", addresses.numAddresses);
	
	//Create a socket from the first address we can
	Socket socket;
	Address outAddress;
	int socketCreated = -1;
	for(uint32_t i = 0; i < addresses.numAddresses; ++i)
	{
		char buffer[ADDRESS_STRLEN];
		GetAddress(&addresses, &outAddress, i);
		GetAddress(&outAddress, buffer);
		printf("Address %d = %s\n", i, buffer);
		socketCreated = CreateSocket(&socket, &outAddress);
		if(socketCreated == 0)
			break;
	}
	if(socketCreated == -1)
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
	
	printf("You have joined the chat\n");
	
	//Send name to server
	error = SendChatPacket(&socket, &packet);
	if(error == -1)
	{
		perror("SendChatPacket: name");
		exit(1);
	}
	
	InitThreadSystem();
	
	Thread receiveThread;
	ThreadInfo tInfo{};
	ThreadData* data = (ThreadData*)calloc(1, sizeof(ThreadData));
	data->connectionSocket = socket;
	tInfo.pFunc = ReceieveFromServer;
	tInfo.pData = data;
	int result = CreateThread(&tInfo, &receiveThread);
	if(result != 0)
	{
		printf("Erorr creating a receieve thread, exiting program\n");
		exit(1);
	}
	
	while(true)
	{
		printf("Enter a message: ");
		ReadInput(&packet.msg);
		
		if(packet.msg.str[0] == '\n' || packet.msg.str[0] == '\r')
		{
			Clear(&packet.msg);
			continue;
		}
		
		printf("Message = %s\n", packet.msg.str);
		
		error = SendChatPacket(&socket, &packet);
		if(error == -1)
		{
			perror("SendChatPacket: while");
			exit(1);
		}
		
		Clear(&packet.msg);
	}
	
	FreeChatString(&packet.msg);
	FreeChatString(&packet.name);
	
	CloseSocket(&socket);
	ExitThreadSystem();
	
	return 0;
}
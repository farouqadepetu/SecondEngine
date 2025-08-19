#include "../Chat/ChatPacket.h"
#include "../Chat/ChatString.h"
#include "../Network/Network.h"
#include "../Thread/Thread.h"
#include <stdlib.h>

#define PORT "12854"

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
	
	SocketEvent tSocketEvent;
	SocketEventInfo eventInfo;
	eventInfo.socketfd = data->connectionSocket.socketfd;
	eventInfo.events = EVENT_RECEIVE;
	int error = CreateSocketEvent(&eventInfo, &tSocketEvent);
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
		error = WaitForEvent(&tSocketEvent);
		if(error == -1)
		{
			perror("WaitForEvent");
			ExitThread();
			return;
		}
		
		if(tSocketEvent.event & EVENT_RECEIVE)
		{
			int error = ReceiveChatPacket(&data->connectionSocket, &packet);
			if(error == -1)
			{
				perror("Recieve");
				ExitThread();
				return;
			}
			if(error == 2)
			{
				//connection was closed
				CloseSocketEvent(&tSocketEvent);
				CloseSocket(&data->connectionSocket);
				free(data);
				ExitThread();
				return;
			}
			
			printf("%s: %s", packet.name.str, packet.msg.str);
			Clear(&packet.name);
			Clear(&packet.msg);
		}
	}
}

int main(int argc, char **argv)
{
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
	
	error = SetToNonBlock(&socket);
	if(error == -1)
	{
		perror("SetToNonBlock");
		exit(1);
	}
	
	SocketEvent socketEvent;
	SocketEventInfo eventInfo;
	eventInfo.socketfd = socket.socketfd;
	eventInfo.events = EVENT_RECEIVE;
	error = CreateSocketEvent(&eventInfo, &socketEvent);
	if(error == -1)
	{
		perror("CreateSocketEvent");
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
		printf("Erorr creating a receieveThread, exiting program");
		exit(1);
	}
	
	ChatPacket packet;
	
	AllocateChatString(&packet.name);
	printf("Enter your name for this session: ");
	ReadInput(&packet.name);
	
	printf("Name = %s\n", packet.name.str);
	AllocateChatString(&packet.msg);
	
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
		
		SendChatPacket(&socket, &packet);
		
		Clear(&packet.msg);
	}
	
	FreeChatString(&packet.msg);
	FreeChatString(&packet.name);
	
	CloseSocketEvent(&socketEvent);
	CloseSocket(&socket);
	ExitThreadSystem();
	
	return 0;
}
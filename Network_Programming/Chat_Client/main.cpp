#include "../Chat/ChatPacket.h"
#include "../Chat/ChatString.h"
#include "../Network/Network.h"
#include <stdlib.h>

#define PORT "3490"

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
	
	ChatPacket packet;
	
	AllocateChatString(&packet.name);
	printf("Enter your name for this session: ");
	ReadInput(&packet.name);
	
	printf("Name = %s\n", packet.name.str);
	
	AllocateChatString(&packet.msg);
	printf("Enter a message: ");
	ReadInput(&packet.msg);
	
	printf("Message = %s\n", packet.msg.str);
	
	SendChatPacket(&socket, &packet);
	
	FreeChatString(&packet.msg);
	FreeChatString(&packet.name);
	
	CloseSocketEvent(&socketEvent);
	CloseSocket(&socket);
	
	return 0;
}
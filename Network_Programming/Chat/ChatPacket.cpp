#include "ChatPacket.h"
#include <stdlib.h>

void SendChatPacket(Socket* pSocket, ChatPacket* pPacket)
{
	//add 1 to account for null-terminating char
	uint32_t nameSize = pPacket->name.size + 1;
	printf("Size of name = %d\n", nameSize);
	
	uint32_t msgSize = pPacket->msg.size + 1;
	printf("Size of msg = %d\n", msgSize);
	
	//size of packet + size of name + name + size of msg + msg
	uint32_t packetSize = sizeof(uint32_t) + sizeof(uint32_t) + nameSize + sizeof(uint32_t) + msgSize;
	printf("Size of packet being sent = %d\n", packetSize);
	
	uint8_t* buffer = (uint8_t*)calloc(packetSize, sizeof(uint8_t));
	
	uint32_t offset = 0;
	
	//copy the number of bytes of the packet into the buffer
	memcpy(buffer, &packetSize, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	
	//copy the size of the user's name in buffer
	memcpy(buffer + offset, &nameSize, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	
	//copy the user's name into the buffer
	memcpy(buffer + offset, &pPacket->name.str, nameSize);
	offset += nameSize;
	
	//copy the size of the user's msg in buffer
	memcpy(buffer + offset, &msgSize, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	
	//copy the user's msg into the buffer
	memcpy(buffer + offset, &pPacket->msg.str, msgSize);
	
	//SEND PACKET
	int numBytesSent = Send(pSocket, buffer, packetSize);
	if(numBytesSent == -1)
	{
		perror("SendChatPacket");
		exit(1);
	}
	
	free(buffer);
}

void RecieveChatPacket(Socket* pSocket, ChatPacket* pPacket)
{
	//Get packet length
	int packetLength = 0;
	int numBytesRead = Recieve(pSocket, &packetLength, 4);
	if(numBytesRead == -1)
	{
		perror("RecieveChatPacket");
		exit(1);
	}
	if(numBytesRead == 0)
	{
		//Need to handle client closing connection
		return;
	}
	
	uint8_t* buffer = (uint8_t*)calloc(packetLength, sizeof(uint8_t));
	numBytesRead = Recieve(pSocket, buffer, packetLength - 4);
	if(numBytesRead == -1)
	{
		perror("RecieveChatPacket");
		exit(1);
	}
	if(numBytesRead != packetLength - 4)
	{
		printf("Number of bytes read does not equal to packet length\n");
		exit(1);
	}
	
	uint32_t offset = 0;
	
	//Get size of name
	memcpy(&pPacket->name.size, buffer, sizeof(uint32_t));
	printf("Name Size = %d\n", pPacket->name.size);
	offset += sizeof(uint32_t);
	
	//Get name
	memcpy(&pPacket->name.str, buffer + offset, pPacket->name.size);
	printf("Name = %s\n", pPacket->name.str);
	offset += pPacket->name.size;
	
	//Get size of msg
	memcpy(&pPacket->msg.size, buffer, sizeof(uint32_t));
	printf("Msg Size = %d\n", pPacket->msg.size);
	offset += sizeof(uint32_t);
	
	//Get msg
	memcpy(&pPacket->msg.str, buffer + offset, pPacket->msg.size);
	printf("Msg = %s\n", pPacket->msg.str);
	
}
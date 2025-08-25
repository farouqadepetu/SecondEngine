#include "ChatPacket.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int SendChatPacket(Socket* pSocket, ChatPacket* pPacket)
{
	//add 1 to account for null-terminating char
	uint32_t nameSize = (pPacket->name.size > 0) ? pPacket->name.size + 1 : 0;
	//printf("Size of name = %d\n", nameSize);
	
	uint32_t msgSize = (pPacket->msg.size > 0) ? pPacket->msg.size + 1 : 0;
	//printf("Size of msg = %d\n", msgSize);
	
	//size of packet + size of nameSize + nameSize + size of msgSize + msg Size + size of msgType
	uint32_t packetSize = sizeof(uint32_t) + sizeof(uint32_t) + nameSize + sizeof(uint32_t) + msgSize + sizeof(uint32_t);
	//printf("Size of packet being sent = %d\n", packetSize);
	
	uint8_t* buffer = (uint8_t*)calloc(packetSize, sizeof(uint8_t));
	
	uint32_t offset = 0;
	
	//copy the number of bytes of the packet into the buffer
	memcpy(buffer, &packetSize, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	
	//copy the size of the user's name in buffer
	memcpy(buffer + offset, &nameSize, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	
	//copy the user's name into the buffer
	memcpy(buffer + offset, pPacket->name.str, nameSize);
	offset += nameSize;
	
	//copy the size of the user's msg in buffer
	memcpy(buffer + offset, &msgSize, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	
	//copy the user's msg into the buffer
	memcpy(buffer + offset, pPacket->msg.str, msgSize);
	offset += msgSize;
	
	printf("Msg Type b4 sending = %d\n", pPacket->msgType);
	//copy the msg type into the buffer
	memcpy(buffer + offset, &pPacket->msgType, sizeof(uint32_t));
	
	//SEND PACKET
	int numBytesSent = Send(pSocket, buffer, packetSize);
	if(numBytesSent == -1)
	{
		return -1;
	}
	if(numBytesSent != packetSize)
	{
		return -1;
	}
	
	free(buffer);
	
	return 0;
}

int ReceiveChatPacket(Socket* pSocket, ChatPacket* pPacket)
{
	//Get packet length
	int packetLength = 0;
	int numBytesRead = Recieve(pSocket, &packetLength, 4);
	if(numBytesRead == -1)
	{
		perror("In RecieveChatPacket()");
		exit(1);
	}
	if(numBytesRead == 0)
	{
		return -2;
	}
	//printf("Packet Length = %d\n", packetLength);
	
	uint8_t* buffer = (uint8_t*)calloc(packetLength, sizeof(uint8_t));
	numBytesRead = Recieve(pSocket, buffer, packetLength - 4);
	if(numBytesRead == -1)
	{
		return -1;
	}
	if(numBytesRead != packetLength - 4)
	{
		return -1;
	}
	if(numBytesRead == 0)
	{
		return -2;
	}
	
	uint32_t offset = 0;
	
	//Get size of name
	uint32_t nameSize = 0;
	memcpy(&nameSize, buffer, sizeof(uint32_t));
	//printf("Name Size = %d\n", nameSize);
	offset += sizeof(uint32_t);
	
	//Get name
	AllocateChatString(&pPacket->name);
	AddString(&pPacket->name, (char*)(buffer + offset), nameSize);
	//printf("Name = %s\n", pPacket->name.str);
	offset += nameSize;
	
	//Get size of msg
	uint32_t msgSize = 0;
	memcpy(&msgSize, buffer + offset, sizeof(uint32_t));
	//printf("Msg Size = %d\n", msgSize);
	offset += sizeof(uint32_t);
	
	//Get msg
	AllocateChatString(&pPacket->msg);
	AddString(&pPacket->msg, (char*)(buffer + offset), msgSize);
	//printf("Msg = %s\n", pPacket->msg.str);
	offset += sizeof(msgSize);
	
	//Get msgType
	memcpy(&pPacket->msgType, buffer + offset, sizeof(uint32_t));
	printf("Msg Type = %d\n", pPacket->msgType);
	
	free(buffer);
	
	return 0;
}
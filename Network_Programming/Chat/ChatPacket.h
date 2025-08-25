#ifndef _CHATPACKET_H_
#define _CHATPACKET_H_

#include "../Chat/ChatString.h"
#include "../Network/Network.h"

enum MessageType
{
	JOINING = 1,
	LEAVING,
	REGULAR
};

struct ChatPacket
{
	ChatString name;
	ChatString msg;
	uint32_t msgType;
};

//Returns -1 if error, 0 otherwise
int SendChatPacket(Socket* pSocket, ChatPacket* pPacket);

//Returns -1 if error, -2 if connection was closed, 0 for success
int ReceiveChatPacket(Socket* pSocket, ChatPacket* pPacket);

#endif
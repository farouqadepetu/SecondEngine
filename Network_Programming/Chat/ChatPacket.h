#ifndef _CHATPACKET_H_
#define _CHATPACKET_H_

#include "../Chat/ChatString.h"
#include "../Network/Network.h"

struct ChatPacket
{
	ChatString name;
	ChatString msg;
};

void SendChatPacket(Socket* pSocket, ChatPacket* pPacket);
void ReceiveChatPacket(Socket* pSocket, ChatPacket* pPacket);

#endif
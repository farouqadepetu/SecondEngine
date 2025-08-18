#ifndef _CHATSTRING_H_
#define _CHATSTRING_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

struct ChatString
{
	uint32_t capacity; //how many chars str can hold
	uint32_t size; //the number of chars in str
	char* str;
	
};

void AllocateChatString(ChatString* pChatStr);
void FreeChatString(ChatString* pChatStr);
void AddChar(ChatString* pChatStr, char ch);

#endif
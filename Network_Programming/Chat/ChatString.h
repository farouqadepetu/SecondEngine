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

//Allocates a string with a default capacity of 32 bytes
void AllocateChatString(ChatString* pChatStr);

//Allocates a string with capacity bytes
void AllocateChatString(ChatString* pChatStr, uint32_t capacity);

//Frees the strong
void FreeChatString(ChatString* pChatStr);

//Appends a chararacter to the end of of the string
void AddChar(ChatString* pChatStr, char ch);

//Appends str to the end of the string
//str needs to be null-terminated
void AddString(ChatString* pChatStr, char* str);

//Removes all chars
void Clear(ChatString* pChatStr);


#endif
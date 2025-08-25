#ifndef _CHATSTRING_H_
#define _CHATSTRING_H_

#include <stdint.h>

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
void AddString(ChatString* pChatStr, char* str, uint32_t size);

//Removes all chars
void Clear(ChatString* pChatStr);

//Returns true if string is empty, false otherwise.
bool Empty(const ChatString* pChatStr);

//Returns the char at index i
char GetChar(const ChatString* pChatStr, uint32_t i);

#endif
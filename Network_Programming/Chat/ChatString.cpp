#include "ChatString.h"
#include <stdlib.h>

void AllocateChatString(ChatString* pChatStr)
{
	pChatStr->capacity = 32;
	pChatStr->size = 0;
	
	//last char should be the null-terminating char so add one more byte
	pChatStr->str = (char*)calloc(pChatStr->capacity + 1, sizeof(char));
}

void AllocateChatString(ChatString* pChatStr, uint32_t capacity)
{
	pChatStr->capacity = capacity;
	pChatStr->size = 0;
	
	//last char should be the null-terminating char so add one more byte
	pChatStr->str = (char*)calloc(pChatStr->capacity + 1, sizeof(char));
}

void ReallocateChatString(ChatString* pChatStr)
{
	printf("Reallocating, the current capacity is %d\n", pChatStr->capacity);
	pChatStr->capacity *= 2;
	printf("Reallocating, the new capacity is %d\n", pChatStr->capacity);
	
	//last char should be the null-terminating char so add one more byte
	char* newStr = (char*)calloc(pChatStr->capacity + 1, sizeof(char));
	
	for(uint32_t i = 0; i < pChatStr->size; ++i)
	{
		newStr[i] = pChatStr->str[i];
	}
	
	free(pChatStr->str);
	pChatStr->str = newStr;
}

void FreeChatString(ChatString* pChatStr)
{
	free(pChatStr->str);
	pChatStr->str = nullptr;
	pChatStr->capacity = 0;
	pChatStr->size = 0;
}

void AddChar(ChatString* pChatStr, char ch)
{
	if(pChatStr->size >= pChatStr->capacity)
		ReallocateChatString(pChatStr);
		
	pChatStr->str[pChatStr->size] = ch;
	++pChatStr->size;
	pChatStr->str[pChatStr->size] = '\0';
}

void AddString(ChatString* pChatStr, char* str)
{
	uint32_t strSize = strlen(str);
	
	for(uint32_t i = 0; i < strSize; ++i)
	{
		AddChar(pChatStr, str[i]);
	}
}

void Clear(ChatString* pChatStr)
{
	for(uint32_t i = 0; i < pChatStr->size; ++i)
	{
		pChatStr->str[i] = 0;
	}
	pChatStr->size = 0;
}

bool Empty(const ChatString* pChatStr)
{
	if(pChatStr->size == 0)
		return true;
		
	return false;
}

char GetChar(const ChatString* pChatStr, uint32_t i)
{
	if(i >=pChatStr->size)
	{
		printf("GetChar() i is out-of-bounds, exiting program!\n");
		exit(1);
	}
	
	return pChatStr->str[i];
}
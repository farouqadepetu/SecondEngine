#include "SEFileSystem.h"
#include <Windows.h>
#include <stdlib.h>
#include <cstdio>


void ReadFile(const char* filename, char** buffer, uint32_t* fileSize, FileType type)
{
	FILE* file = nullptr;

	switch (type)
	{
	case TEXT:
		fopen_s(&file, filename, "r");
		break;
	case BINARY:
		fopen_s(&file, filename, "rb");
		break;
	}

	if (!file)
	{
		char msg[] = "Failed to open file ";
		strcat_s(msg, filename);
		strcat_s(msg, ". Exiting prorgam");
		MessageBoxA(nullptr, msg, "File open error.", MB_OK);
		exit(3);
	}

	fseek(file, 0, SEEK_END);

	*fileSize = ftell(file);

	*buffer = (char*)calloc(*fileSize, sizeof(char));

	fseek(file, 0, SEEK_SET);

	fread(*buffer, sizeof(char), *fileSize, file);

	fclose(file);
}

void FreeFileBuffer(char* buffer)
{
	free(buffer);
}
#include "SEFileSystem.h"
#include <Windows.h>
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
		char msg[256] = "Failed to open file ";
		strcat_s(msg, filename);
		strcat_s(msg, ". Exiting prorgam.");
		MessageBoxA(nullptr, msg, "File open error.", MB_OK);
		exit(-1);
	}

	fseek(file, 0, SEEK_END);

	uint32_t size = ftell(file);

	char* buf = (char*)calloc(size, sizeof(char));

	fseek(file, 0, SEEK_SET);

	int result = fread(buf, sizeof(char), size, file);
	if (result != size)
	{
		char msg[256] = "Failed to read file ";
		strcat_s(msg, filename);
		strcat_s(msg, ". Exiting prorgam.");
		MessageBoxA(nullptr, msg, "File read error.", MB_OK);
	}

	fclose(file);

	*buffer = buf;
	*fileSize = size;
}

void GetCurrentPath(char* dir)
{
	char currentDirectory[MAX_FILE_PATH]{};
	GetModuleFileNameA(nullptr, currentDirectory, MAX_FILE_PATH);

	//Find last slash
	char* slash = strrchr(currentDirectory, '\\');
	uint32_t length = 0;
	length = slash - currentDirectory + 2;

	strncpy_s(dir, length, currentDirectory, _TRUNCATE);
}
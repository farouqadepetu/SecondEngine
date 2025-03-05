#pragma once

#include <cstdint>

#define MAX_FILE_PATH 256

enum FileType
{
	TEXT,
	BINARY
};

void ReadFile(const char* filename, char** buffer, uint32_t* fileSize, FileType type);
void GetCurrentPath(char* dir);
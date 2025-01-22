#pragma once

#include <cstdint>

enum FileType
{
	TEXT,
	BINARY
};

void ReadFile(const char* filename, char** buffer, uint32_t* fileSize, FileType type);

void FreeFileBuffer(char* buffer);
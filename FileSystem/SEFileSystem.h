#pragma once

#include <cstdint>

#define MAX_FILE_PATH 256

enum FileType
{
	TEXT,
	BINARY
};

struct SEFile
{
	FILE* file = nullptr;
	char* buffer = nullptr; //a stb_ds array
	uint32_t size;
	FileType type;
};

void ReadFile(const char* filename, SEFile* outFile, FileType type);
void FreeSEFile(SEFile* file);
void GetCurrentPath(char* dir);
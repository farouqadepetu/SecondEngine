#include <Windows.h>
#include <cstdio>

#include "..\ThirdParty\stb_ds.h"
#include "SEFileSystem.h"


void ReadFile(const char* filename, SEFile* outFile, FileType type)
{
	SEFile file{};

	switch (type)
	{
	case TEXT:
		fopen_s(&file.file, filename, "r");
		file.type = TEXT;
		break;
	case BINARY:
		fopen_s(&file.file, filename, "rb");
		file.type = BINARY;
		break;
	}

	char errorMsg[256]{};
	if (!file.file)
	{
		strcat_s(errorMsg, "Failed to open file ");
		strcat_s(errorMsg, filename);
		strcat_s(errorMsg, ". Exiting prorgam.");
		MessageBoxA(nullptr, errorMsg, "File open error.", MB_OK);
		exit(-1);
	}

	uint32_t size = 0;
	char* buf = nullptr;
	if (type == TEXT)
	{
		char ch = fgetc(file.file);
		while (ch != EOF)
		{
			arrpush(buf, ch);
			ch = fgetc(file.file);
		}

		arrpush(buf, '\0');
		size = arrlenu(buf);
	}
	else //type == BINARY
	{
		//only use fseek for binary files for getting size of the file
		fseek(file.file, 0, SEEK_END);

		size = ftell(file.file);

		buf = (char*)calloc(size, sizeof(char));

		fseek(file.file, 0, SEEK_SET);

		int result = fread(buf, sizeof(char), size, file.file);
		if (result != size)
		{
			strcat_s(errorMsg, "Failed to read file ");
			strcat_s(errorMsg, filename);
			strcat_s(errorMsg, ". Exiting prorgam.");
			MessageBoxA(nullptr, errorMsg, "File read error.", MB_OK);
			exit(-1);
		}
	}

	fclose(file.file);
	
	file.buffer = buf;
	file.size = size;

	*outFile = file;
}

void FreeSEFile(SEFile* file)
{
	if (file->type == TEXT)
		arrfree(file->buffer);
	else //type == BINARY
		free(file->buffer);

	fclose(file->file);
}

void WriteFile(const char* filename, char* buffer, uint32_t numChars, FileType type)
{
	FILE* file = nullptr;

	switch (type)
	{
	case TEXT:
		fopen_s(&file, filename, "w");
		break;
	case BINARY:
		fopen_s(&file, filename, "wb");
		break;
	}

	char errorMsg[256]{};
	if (!file)
	{
		strcat_s(errorMsg, "Failed to open file ");
		strcat_s(errorMsg, filename);
		strcat_s(errorMsg, ". Exiting prorgam.");
		MessageBoxA(nullptr, errorMsg, "File open error.", MB_OK);
		exit(-1);
	}

	int result = fwrite(buffer, sizeof(char), numChars, file);
	if (result != numChars)
	{
		strcat_s(errorMsg, "Failed to write to file ");
		strcat_s(errorMsg, filename);
		strcat_s(errorMsg, ". Exiting prorgam.");
		MessageBoxA(nullptr, errorMsg, "File write error.", MB_OK);
		exit(-1);
	}

	fclose(file);
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
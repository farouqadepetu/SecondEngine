#define WIN32_LEAN_AND_MEAN

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <Windows.h>

#define MAX_FILE_PATH 256

enum ShadingLanguage
{
	HLSL,
	GLSL
};

enum ShaderType
{
	NONE,
	VERT,
	FRAG,
	COMP
};

void CompileShaders(const char* currentDir, const char* lastSlash, const char* debug, ShadingLanguage language)
{
	uint32_t length = 0;
	length = lastSlash - currentDir + 1;

	char inputDirectory[MAX_FILE_PATH]{};
	strncpy_s(inputDirectory, currentDir, length);

	switch (language)
	{
	case HLSL:
		strcat_s(inputDirectory, "Shaders\\HLSL\\*");
		break;
	case GLSL:
		strcat_s(inputDirectory, "Shaders\\GLSL\\*");
		break;
	}

	HANDLE fileHandle = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA findData{};

	fileHandle = FindFirstFileA(inputDirectory, &findData);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		uint32_t fileError = GetLastError();
		if (fileError == ERROR_FILE_NOT_FOUND)
		{
			printf("No files found! Exiting program!\n");
			ExitProcess(-1);
		}

		printf("Error finding files. Exiting prorgam.\n");
		ExitProcess(-1);
	}

	do
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}
		else
		{
			char inputFileWithPath[MAX_FILE_PATH]{};
			strncpy_s(inputFileWithPath, currentDir, length);

			switch (language)
			{
			case HLSL:
				strcat_s(inputFileWithPath, "Shaders\\HLSL\\");
				break;
			case GLSL:
				strcat_s(inputFileWithPath, "Shaders\\GLSL\\");
				break;
			}
			strcat_s(inputFileWithPath, findData.cFileName);

			//Find last dot
			char* dot = strrchr(findData.cFileName, '.');
			uint32_t dotLength = 0;
			dotLength = dot - findData.cFileName;

			char outputFile[MAX_FILE_PATH]{};
			strncpy_s(outputFile, findData.cFileName, dotLength);

			char outputFileWithPath[MAX_FILE_PATH]{};
			strncpy_s(outputFileWithPath, currentDir, length);
			switch (language)
			{
			case HLSL:
				strcat_s(outputFileWithPath, "CompiledShaders\\HLSL\\");
				break;
			case GLSL:
				strcat_s(outputFileWithPath, "CompiledShaders\\GLSL\\");
				break;
			}
			strcat_s(outputFileWithPath, outputFile);

			//Find first dot
			dot = strchr(findData.cFileName, '.');
			dotLength = dot - findData.cFileName;

			char shaderType[5]{};
			strncpy_s(shaderType, dot + 1, 4);
			shaderType[4] = '\0';

			ShaderType type = NONE;
			if (strcmp(shaderType, "vert") == 0)
				type = VERT;
			else if(strcmp(shaderType, "frag") == 0)
				type = FRAG;
			else if(strcmp(shaderType, "comp") == 0)
				type = COMP;

			if (type == NONE)
				continue;

			char commandLine[1024]{};

			if (language == HLSL)
			{
				strcat_s(commandLine, "dxc.exe ");
				strcat_s(commandLine, inputFileWithPath);

				if (debug != nullptr)
				{
					strcat_s(commandLine, " -Zi -Qembed_debug ");
				}

				switch (type)
				{
				case VERT:
					strcat_s(commandLine, " -T vs_6_5 -E vsMain ");
					break;

				case FRAG:
					strcat_s(commandLine, " -T ps_6_5 -E psMain ");
					break;

				case COMP:
					strcat_s(commandLine, " -T cs_6_5 -E csMain ");
					break;
				}

				strcat_s(commandLine, "-Fo ");
				strcat_s(commandLine, outputFileWithPath);
			}
			else //GLSL
			{
				strcat_s(commandLine, "glslc.exe  --target-env=vulkan1.3 ");

				switch (type)
				{
				case VERT:
					strcat_s(commandLine, "-fshader-stage=vert ");
					break;

				case FRAG:
					strcat_s(commandLine, "-fshader-stage=frag ");
					break;

				case COMP:
					strcat_s(commandLine, "-fshader-stage=comp ");
					break;
				}

				strcat_s(commandLine, inputFileWithPath);
				strcat_s(commandLine, " -o ");
				strcat_s(commandLine, outputFileWithPath);
			}

			STARTUPINFOA startupInfo{};
			PROCESS_INFORMATION processInfo{};

			ZeroMemory(&startupInfo, sizeof(startupInfo));
			startupInfo.cb = sizeof(startupInfo);
			ZeroMemory(&processInfo, sizeof(processInfo));

			bool result = CreateProcessA(nullptr, commandLine, nullptr, nullptr, false, 0, nullptr, nullptr,
				&startupInfo, &processInfo);

			if (result == false)
			{
				printf("CreateProcess failed (%d).\n", GetLastError());
				ExitProcess(-1);
			}

			WaitForSingleObject(processInfo.hProcess, INFINITE);
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);

		}

	} while (FindNextFileA(fileHandle, &findData));

	uint32_t dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		printf("Error should equal to ERROR_NO_MORE_FILES. Exiting Program!\n");
		ExitProcess(-1);
	}
}

int main()
{
	char currentDirectory[MAX_FILE_PATH]{};
	GetModuleFileNameA(nullptr, currentDirectory, MAX_FILE_PATH);

	//Find last slash
	char* lastSlash = strrchr(currentDirectory, '\\');

	//Find Debug substring
	char* debug = strstr(currentDirectory, "Debug");

	CompileShaders(currentDirectory, lastSlash, debug, HLSL);
	CompileShaders(currentDirectory, lastSlash, debug, GLSL);


	return 0;
}
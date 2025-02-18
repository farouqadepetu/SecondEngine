#pragma once

#include <Windows.h>
#include <strsafe.h>
#include <cstdint>

#if defined(CreateWindow)
#undef CreateWindow
#endif

struct WndInfo
{
	const char* wndName;
    uint32_t width;
    uint32_t height;
};

struct Window
{
	WNDCLASSEXW wndClass;
	HWND wndHandle;
};


void CreateWindow(WndInfo* data, Window* window);

uint32_t GetWidth(Window* window);
uint32_t GetHeight(Window* window);

bool CheckKeyDown(int vKey);
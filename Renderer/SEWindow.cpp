#include "SEWindow.h"

extern void OnResize();
extern bool gAppPaused;

LRESULT CALLBACK SEWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			gAppPaused = true;
		}
		else
		{
			gAppPaused = false;
			OnResize();
		}
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void CreateSEWindow(SEWndInfo* data, SEWindow* window)
{
    size_t numChars = strlen(data->wndName) + 1;
    wchar_t cName[255]{};
    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, cName, numChars, data->wndName, _TRUNCATE);

	WNDCLASSEXW SEWndClass{};
    SEWndClass.cbSize = sizeof(WNDCLASSEXW);
	SEWndClass.style = 0;
	SEWndClass.lpfnWndProc = SEWindowProc;
	SEWndClass.cbClsExtra = 0;
	SEWndClass.cbWndExtra = 0;
	SEWndClass.hInstance = GetModuleHandle(nullptr);
	SEWndClass.hIcon = nullptr;
	SEWndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	SEWndClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	SEWndClass.lpszMenuName = nullptr;
	SEWndClass.lpszClassName = cName;
	SEWndClass.hIconSm = nullptr;

    if (!RegisterClassEx(&SEWndClass))
    {
        ErrorExit(L"RegisterClassEx");
    }

	RECT desiredClientSize{ 0, 0, (long)data->width,(long)data->height };

	//Calulates the required size of the window to make sure the client window size is (WIDTH, HEIGHT)
	//When the function returns, the structure contains the coordinates of the top-left and bottom-right corners of the window 
	//to accommodate the desired client area.
	AdjustWindowRect(&desiredClientSize, WS_OVERLAPPEDWINDOW, false);

	uint32_t width = desiredClientSize.right - desiredClientSize.left;
	uint32_t height = desiredClientSize.bottom - desiredClientSize.top;

    window->wndHandle = CreateWindowEx(0, cName, cName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

    if (!window->wndHandle)
    {
        ErrorExit(L"CreateWindowEx");
    }

    ShowWindow(window->wndHandle, SW_NORMAL);
}

uint32_t GetWidth(SEWindow* window)
{
	RECT rect{};
	GetClientRect(window->wndHandle, &rect);

	return rect.right - rect.left;
}

uint32_t GetHeight(SEWindow* window)
{
	RECT rect{};
	GetClientRect(window->wndHandle, &rect);

	return rect.bottom - rect.top;
}

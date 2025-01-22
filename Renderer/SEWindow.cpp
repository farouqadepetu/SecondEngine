#include "SEWindow.h"

void (*gOnResize)() = nullptr;
void (*gFuncOnKeyReleased)(int) = nullptr;

extern bool gAppPaused;

bool gMinimized = false;
bool gMaximized = false;
bool gResizing = false;
bool gTracked = false;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			gAppPaused = true;
		}
		else
		{
			gAppPaused = false;
		}
		break;

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			gAppPaused = true;
			gMinimized = true;
			gMaximized = false;
		}
		else if(wParam == SIZE_MAXIMIZED)
		{
			gAppPaused = false;
			gMaximized = true;
			gMinimized = false;
			if(gOnResize)
				gOnResize();
		}
		else if (wParam == SIZE_RESTORED)
		{
			if (gMinimized)
			{
				gAppPaused = false;
				gMinimized = false;
				if (gOnResize)
					gOnResize();
			}
			else if (gMaximized)
			{
				gAppPaused = false;
				gMaximized = false;
				if (gOnResize)
					gOnResize();
			}
			else if (gResizing)
			{
				// If user is dragging the resize bars, we do not resize 
				// the buffers here because as the user continuously 
				// drags the resize bars, a stream of WM_SIZE messages are
				// sent to the window, and it would be pointless (and slow)
				// to resize for each WM_SIZE message received from dragging
				// the resize bars.  So instead, we reset after the user is 
				// done resizing the window and releases the resize bars, which 
				// sends a WM_EXITSIZEMOVE message.
			}
			else
			{
				if (gOnResize)
					gOnResize();
			}
		}
		break;

	case WM_ENTERSIZEMOVE:
		gAppPaused = true;
		gResizing = true;
		break;

	case WM_EXITSIZEMOVE:
		gAppPaused = false;
		gResizing = false;
		if (gOnResize)
			gOnResize();
		break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

	case WM_KEYUP:
		if(gFuncOnKeyReleased != nullptr)
			gFuncOnKeyReleased(wParam);
		break;

	case WM_LBUTTONUP:
		if (gFuncOnKeyReleased != nullptr)
			gFuncOnKeyReleased(VK_LBUTTON);
		break;

	case WM_RBUTTONUP:
		if (gFuncOnKeyReleased != nullptr)
			gFuncOnKeyReleased(VK_RBUTTON);
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}


void CreateWindow(WndInfo* data, Window* window)
{
    size_t numChars = strlen(data->wndName) + 1;
    wchar_t cName[255]{};
    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, cName, numChars, data->wndName, _TRUNCATE);

	WNDCLASSEXW SEWndClass{};
    SEWndClass.cbSize = sizeof(WNDCLASSEXW);
	SEWndClass.style = 0;
	SEWndClass.lpfnWndProc = WindowProc;
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

uint32_t GetWidth(Window* window)
{
	RECT rect{};
	GetClientRect(window->wndHandle, &rect);

	return rect.right - rect.left;
}

uint32_t GetHeight(Window* window)
{
	RECT rect{};
	GetClientRect(window->wndHandle, &rect);

	return rect.bottom - rect.top;
}

bool CheckKeyDown(int vKey)
{
	return GetAsyncKeyState(vKey) & 0x8000;
}

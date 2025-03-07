#include "SEWindow.h"
#include "../Time/SETimer.h"
#include "../SEApp.h"
#include "SERenderer.h"
#include "../UI/SEUI.h"

bool gAppPaused = false;
bool gMinimized = false;
bool gMaximized = false;
bool gResizing = false;

//bool gMouseCaptured = false;
bool gKeys[0x100] = { 0 };

bool gResize = false;

MainComponent gFrameStatsWindow;
char gFrameStatString[64];

MainComponent gApiWindow;
const char* gApiNames[] = { "DIRECTX", "VULKAN" };
uint32_t gCurrentApiIndex = 0;
uint32_t gNextApiIndex = 0;

void CheckInput(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int currentKeyDown = 0;
	if (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse == false)
	{
		switch (uMsg)
		{
		case WM_KEYDOWN:
			currentKeyDown = wParam & 0xFF;
			gKeys[wParam & 0xFF] = 1;
			break;

		case WM_KEYUP:
			gKeys[wParam & 0xFF] = 0;
			break;

		case WM_LBUTTONDOWN:
			currentKeyDown = VK_LBUTTON;
			gKeys[VK_LBUTTON] = 1;
			break;

		case WM_LBUTTONUP:
			gKeys[VK_LBUTTON] = 0;
			break;

		case WM_RBUTTONDOWN:
			currentKeyDown = VK_RBUTTON;
			gKeys[VK_RBUTTON] = 1;
			break;

		case WM_RBUTTONUP:
			gKeys[VK_RBUTTON] = 0;
			break;

		case WM_MOUSEMOVE:

			/*// Get the current mouse position (in client coordinates)
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			RECT clientRect;
			GetClientRect(hwnd, &clientRect);

			if (PtInRect(&clientRect, pt))
			{
				// Mouse is inside: if not already captured, capture it.
				if (gMouseCaptured == false)
				{
					OutputDebugStringA("Mouse is inside the window\n");
					SetCapture(hwnd);
					gMouseCaptured = true;
				}
			}
			else
			{
				// Mouse is outside: if we have capture, release it.
				if (gMouseCaptured == true)
				{
					OutputDebugStringA("Mouse is outside the window\n");
					ReleaseCapture();
					gMouseCaptured = false;
				}

			}*/
			break;
		}
	}
	else if(ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse == true)
	{
		gKeys[currentKeyDown] = 0;
	}
	
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		return true;

	switch (uMsg)
	{
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				//ReleaseCapture();
				//SetFocus(nullptr);
			}
			else
			{
				//SetCapture(hwnd);
				//SetFocus(hwnd);
			}
			break;
		}

		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
			{
				gAppPaused = true;
				gMinimized = true;
				gMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				gAppPaused = false;
				gMaximized = true;
				gMinimized = false;
				gResize = true;
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (gMinimized)
				{
					gAppPaused = false;
					gMinimized = false;
					gResize = true;
				}
				else if (gMaximized)
				{
					gAppPaused = false;
					gMaximized = false;
					gResize = true;
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
					gResize = true;
				}
			}
			break;
		}

		case WM_ENTERSIZEMOVE:
		{
			gAppPaused = true;
			gResizing = true;
			break;
		}

		case WM_EXITSIZEMOVE:
		{
			gAppPaused = false;
			gResizing = false;
			gResize = true;
			break;
		}

		case WM_DESTROY:
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}

		default:
		{
			CheckInput(hwnd, uMsg, wParam, lParam);
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}

void ErrorExit(LPCTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
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
	//SEWndClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	SEWndClass.lpszMenuName = nullptr;
	SEWndClass.lpszClassName = cName;
	SEWndClass.hIconSm = nullptr;

	if (!RegisterClassEx(&SEWndClass))
	{
		// Get the error message, if any.
		DWORD errorMessageID = GetLastError();
		if (errorMessageID != ERROR_CLASS_ALREADY_EXISTS)
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
	UpdateWindow(window->wndHandle);

	window->wndClass = SEWndClass;
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
	return gKeys[vKey];
}

bool ProcessMessages()
{
	bool quit = false;
	MSG msg{};
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT || msg.message == WM_CLOSE)
			quit = true;
	}

	return quit;
}

void CreateComponents(Window* pWindow)
{
	MainComponentInfo mcInfo{};
	mcInfo.pLabel = "Frame Stats Window";
	mcInfo.position = vec2(0.0f, 0.0f);
	mcInfo.size = vec2(200.0f, 40.0f);
	mcInfo.flags = MAIN_COMPONENT_FLAGS_NO_DECORATION | MAIN_COMPONENT_FLAGS_NO_INPUTS | MAIN_COMPONENT_FLAGS_NO_MOVE
		| MAIN_COMPONENT_FLAGS_NO_NAV | MAIN_COMPONENT_FLAGS_NO_BACKGROUND | MAIN_COMPONENT_FLAGS_NO_SAVED_SETTINGS;
	CreateMainComponent(&mcInfo, &gFrameStatsWindow);

	SubComponent fpsText{};
	fpsText.type = SUB_COMPONENT_TYPE_TEXT;
	fpsText.text.text = gFrameStatString;
	fpsText.text.color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	AddSubComponent(&gFrameStatsWindow, &fpsText);

	uint32_t width = GetWidth(pWindow);
	mcInfo.pLabel = "API";
	mcInfo.size = vec2(150.0f, 60.0f);
	mcInfo.position = vec2(width - mcInfo.size.GetX(), 0.0f);
	mcInfo.flags = MAIN_COMPONENT_FLAGS_NO_DECORATION | MAIN_COMPONENT_FLAGS_NO_SAVED_SETTINGS | MAIN_COMPONENT_FLAGS_NO_NAV |
		MAIN_COMPONENT_FLAGS_NO_RESIZE;
	CreateMainComponent(&mcInfo, &gApiWindow);

	SubComponent api{};
	api.type = SUB_COMPONENT_TYPE_DROPDOWN;
	api.dropDown.pLabel = "Renderer API";
	api.dropDown.pData = &gNextApiIndex;
	api.dropDown.numNames = 2;
	api.dropDown.pNames = gApiNames;
	AddSubComponent(&gApiWindow, &api);
}

void OnApiSwitch(App* pApp, Window* pWindow, Timer* timer)
{
	pApp->Exit();

	DestroyWindow(pWindow->wndHandle);
	pWindow->wndHandle = nullptr;
	ProcessMessages();

	DestroyUserInterface();
	DestroyMainComponent(&gFrameStatsWindow);
	DestroyMainComponent(&gApiWindow);

	WndInfo wndData{};
	wndData.wndName = pApp->appName;
	wndData.width = 1280;
	wndData.height = 800;
	CreateWindow(&wndData, pWindow);

	InitUserInterface(pWindow);

	CreateComponents(pWindow);

	OnRendererApiSwitch();

	InitTimer(timer);

	pApp->Init();
}

void FrameStats(float deltaTime)
{
	static uint32_t frameCounter = 0;
	static float timeCounter = 0.0f;
	static uint32_t gFps = 0;

	++frameCounter;

	timeCounter += deltaTime;

	if (timeCounter >= 1.0f)
	{
		gFps = frameCounter;
		frameCounter = 0;
		timeCounter = 0;
	}

	snprintf(gFrameStatString, 64, "FPS = %d\nFrame Time = %f ms", gFps, deltaTime);
}


int WindowsMain(App* pApp)
{
	Window window{};
	WndInfo wndData{};
	wndData.wndName = pApp->appName;
	wndData.width = 1280;
	wndData.height = 800;
	CreateWindow(&wndData, &window);

	Timer timer{};
	InitTimer(&timer);

	pApp->pWindow = &window;

	InitUserInterface(&window);
	CreateComponents(&window);

	pApp->Init();

	MSG msg{};
	bool quit = false;
	while (!quit)
	{
		Tick(&timer);
		float deltaTime = timer.deltaTime;
		quit = ProcessMessages();

		if (!gAppPaused)
		{
			FrameStats(deltaTime);
			pApp->UserInput(deltaTime);

			extern void UpdateUserInterface();
			UpdateUserInterface();

			if (gResize == true)
			{
				pApp->Resize();
				gResize = false;
			}

			if (gCurrentApiIndex != gNextApiIndex)
			{
				OnApiSwitch(pApp, &window, &timer);
				gCurrentApiIndex = gNextApiIndex;
				continue;
			}

			pApp->Update(deltaTime);
			pApp->Draw();
		}

	}

	pApp->Exit();
	DestroyMainComponent(&gFrameStatsWindow);
	DestroyMainComponent(&gApiWindow);
	DestroyUserInterface();
	return 0;
}
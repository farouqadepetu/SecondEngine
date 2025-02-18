#pragma once

#include "Renderer/SEWindow.h"

class App
{
public:
	virtual void Init() = 0;
	virtual void Exit() = 0;
	virtual void UserInput(float deltaTime) = 0;
	virtual void Resize() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;

	const char* appName;
	Window* pWindow;
};

int WindowsMain(App* pApp);
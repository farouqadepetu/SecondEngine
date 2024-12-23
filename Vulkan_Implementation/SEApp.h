#pragma once

bool gAppPaused = false;

class App
{
public:
	virtual void Init() = 0;
	virtual void Exit() = 0;
	virtual void UserInput() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};
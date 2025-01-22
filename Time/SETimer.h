#pragma once

#include "Windows.h"

struct Timer
{
	double secondsPerCount;
	__int64 prevTime;
	__int64 currTime;
	float deltaTime;
};

inline void InitTimer(Timer* timer)
{
	__int64 countsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	timer->secondsPerCount = 1.0 / (double)countsPerSec;

	timer->prevTime = 0;
	timer->currTime = 0;
	timer->deltaTime = -1.0f;
}

inline void Tick(Timer* timer)
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	timer->currTime = currTime;

	double dt = (timer->currTime - timer->prevTime) * timer->secondsPerCount;
	timer->deltaTime = dt;

	timer->prevTime = timer->currTime;

	if (timer->deltaTime < 0.0f)
		timer->deltaTime = 0.0f;
}
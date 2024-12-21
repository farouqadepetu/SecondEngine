#include "ProfileTime.h"

ProfileTime::ProfileTime()
{
	__int64 countsPerSec = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;

	mStartTime = 0;
	mEndTime = 0;
	
}

void ProfileTime::Reset()
{
	mStartTime = 0;
	mEndTime = 0;
}

void ProfileTime::Start()
{
	__int64 currentTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	mStartTime = currentTime;
}

void ProfileTime::End()
{
	__int64 currentTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	mEndTime = currentTime;
}

float ProfileTime::TotalTime(double unitOfTime)
{
	double totalTime = (mEndTime - mStartTime) * mSecondsPerCount;

	if (totalTime < 0.0)
		totalTime = 0.0;

	return (float)(totalTime * unitOfTime);
}



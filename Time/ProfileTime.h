#pragma once

#include "Windows.h"

#define SECONDS 1.0
#define MILLISECONDS 1000.0
#define MICROSECONDS 1000000.0
#define NANOSECONDS 1000000000.0

class ProfileTime
{
public:
	ProfileTime();

	//Resets the timer
	void Reset();

	//Starts the timer
	void Start();

	//Ends the timer
	void End();

	//Returns the elasped time.
	//Pass in SECONDS to return the time in seconds
	//Pass in MILLISECONDS to return the time in milliseconds
	//Pass in MICROSECONDS to return the time in microseconds
	//Pass in NANOSECONDS to return the time in nanoseconds
	float TotalTime(double unitofTime);

private:
	double mSecondsPerCount;
	__int64 mStartTime;
	__int64 mEndTime;
};
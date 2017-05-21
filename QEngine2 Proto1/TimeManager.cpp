#define NOMINMAX

#include <Windows.h>
#include "TimeManager.h"

TimeManager::TimeManager()
{
	LastSystemTime = GetTime();
	FPSTimer = GetTime();
	DeltaTime = 0;
	FrameCount = 0;
	FPSCount = 0;
	StartTime = GetTime();
}

float TimeManager::GetTime()
{
	return GetTickCount() * 0.001f;
}

void TimeManager::TimeUpdate()
{
	float CurrentTime = GetTime();

	//updating time shift
	if(CurrentTime - LastSystemTime > 0)
		DeltaTime = CurrentTime - LastSystemTime;
	else
		DeltaTime = 0.0;

	//calculate FPS every second
	if(FPSTimer + 1.0 < CurrentTime)
	{
		FPSCount = (float)FrameCount / (CurrentTime - FPSTimer);
		FPSTimer = CurrentTime;
		FrameCount = 0;
	}
	FrameCount++;

	//update time counter
	LastSystemTime = CurrentTime;
}

float TimeManager::GetDeltaTime()
{
	return DeltaTime;
}

float TimeManager::GetFPS()
{
	return FPSCount;
}
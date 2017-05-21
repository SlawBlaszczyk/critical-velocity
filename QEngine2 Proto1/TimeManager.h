#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

class TimeManager
{
	float LastSystemTime;
	float DeltaTime;	//diffrence of time between two consecutive frames
	float StartTime;
	int FrameCount;		//frame counter
	float FPSCount;
	float FPSTimer;

public:
	TimeManager();
	void TimeUpdate();
	float GetTime();	//in seconds
	float GetDeltaTime();
	float GetFPS();
	float GetLastSystemTime() {return LastSystemTime;}
	void SetStartTime(float t) {StartTime = t;}
	float GetStartTime() {return StartTime;}

};

#endif
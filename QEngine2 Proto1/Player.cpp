#include "Player.h"
#include "Game.h"
#include "Logic.h"
#include "Level.h"
#include "IQGraphics.h"
#include "TimeManager.h"

Player::Player(Game* eng, Vehicle* veh)
{
	engine = eng;
	vehicle = veh;

	nCheckpoints = 0;
	CurrentCheckpoint = 0;
	LapsFinished = 0;
	LapTime = 0.0f;
	BestLap = 0.0f;
	bLastLapBest = false;

	LapStartedTime = 0;
}

void Player::CheckCheckpoints()
{
	if (IsCheckpointReached())
	{
		CheckpointReachedEvent();
	}
}

bool Player::IsCheckpointReached()
{
	//get car position
	NxVec3 nxcpos = vehicle->actor->getGlobalPosition();
	QVector3 cpos (nxcpos.x, nxcpos.y, nxcpos.z);

	//get vector to checkpoint
	QVector3 ToCheckpoint = cpos - checkpoints[CurrentCheckpoint];
	
	//get distance
	float DistToCheckpoint = ToCheckpoint.Magnitude();

	if(DistToCheckpoint <= 30.0f)
		return true;
	else
		return false;
}

void Player::CheckpointReachedEvent()
{
	if (CurrentCheckpoint >= nCheckpoints-1)
	{
		CurrentCheckpoint = 0;
		LapFinishedEvent();
	}
	else CurrentCheckpoint++;
}

void Player::LapFinishedEvent()
{
	//update lap time
	if(LapsFinished == 0)
		LapTime = engine->tman->GetTime() - engine->logic->RaceStartedTime;
	else
		LapTime = engine->tman->GetTime() - LapStartedTime;

	LapStartedTime = engine->tman->GetTime();

	//check for best lap
	if((LapsFinished == 0) || LapTime < BestLap)
	{
		BestLap = LapTime;
		bLastLapBest = true;
	}
	else
		bLastLapBest = false;

	LapsFinished++;

	if (LapsFinished > 3)
	{	
		engine->logic->GameState = GAME_STATE_OVER;
	}
}
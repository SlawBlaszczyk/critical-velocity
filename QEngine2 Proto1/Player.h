#ifndef PLAYER_H
#define PLAYER_H

#include "Q3DObject.h"
#include "QVector3.h"

class Game;
class Vehicle;

class Player

{
public:
	Game* engine;
	Vehicle* vehicle;
	QVector3 checkpoints [64];
	int nCheckpoints;	//number of checkpoints to check on the race track
	
	int CurrentCheckpoint;
	int LapsFinished;
	float LapTime;
	float BestLap;
	bool bLastLapBest;

	float LapStartedTime;

	Player::Player(Game* eng, Vehicle* veh);

	void CheckCheckpoints();
	bool IsCheckpointReached();
	void CheckpointReachedEvent();
	void LapFinishedEvent();
};

#endif
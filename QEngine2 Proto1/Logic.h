#ifndef LOGIC_H
#define LOGIC_H

#include <list>
#include <queue>
#include "Game.h"
#include "Q3DObject.h"
#include "Player.h"
#include "Vehicle.h"
#include "OBox.h"
#include "OTree.h"
#include "OFinishGate.h"
#include "QPhysicalObject.h"

#define GAME_STATE_COUNTDOWN 0
#define GAME_STATE_RUNNING 1
#define GAME_STATE_OVER 2

class Level;

class Logic
{
	Game* engine;
	Level* level;

	bool LevelInitialized;

	//event list
	list<Event*> Events;

public:

	int GameState;

	//players
	Player* players [8];

	//PATTERN OBJECTS
	Q3DObject* car1body;
	Q3DObject* car1body_lod1;
	Q3DObject* car1body_lod2;
	Q3DObject* car1body_lod3;

	Q3DObject* car1wheel;
	Q3DObject* car1wheel_lod1;
	Q3DObject* car1wheel_lod2;
	Q3DObject* car1wheel_lod3;

	Q3DObject* car1frontlights;
	Q3DObject* car1spoiler;
	Q3DObject* car1exhaustl;
	Q3DObject* car1exhaustr;
	Q3DObject* car1floorrear;
	Q3DObject* car1halogen;
	Q3DObject* car1taillightsout;
	Q3DObject* car1taillightsin;
	Q3DObject* car1floormain;

	Q3DObject* car2body;

	//KASIA AI stuff

	//Q3DObject* car3body;
	D3DXVECTOR2* wektorPredkosci2d;
	D3DXVECTOR3* targetsPositions[ 1000 ];
	float distance;
	double iloczynSkalarny;
	int numberOfTargets;

	//////

	//obstacle
	Q3DObject* PatternTree01;
	OBox* PatternBox;
	OTree* PatternTree02;

	OFinishGate* PatternFinishGate;

	Vehicle* car1;
	Vehicle* car2;
	//Vehicle* car3;

	//aux objects
	//QPhysicalObject* aux_box;
	

	//goals
	//Patrol* patrol;
	//Flee* flee;
	//Pursue* pursue;

	Logic(Game* eng);
	~Logic();
	void Init();
	void Update();
	Player** GetPlayers() {return players;}
	Level* GetLevel() {return level;}
	bool GetLevelInitialized() {return LevelInitialized;}

	void AddEvent(Event* e);
	void HandleEvents();

	void CheckGameStatus();
	void AIUpdate(Vehicle* car);

	//linking free cam with car
	bool CamLinkedWithPlayer;
	void MoveLinkedCamera();


	//time variables
	float GameLaunchedTime;
	float RaceStartedTime;

};

#endif
#ifndef GOAL_H
#define GOAL_H

#include "Game.h"

class Vehicle;

class Goal
{
protected:
	Game* engine;

public:
	Goal(Game* eng);
	virtual void Activate(Vehicle* cr);
	virtual void Process(Vehicle* cr);
	virtual void Terminate(Vehicle* cr);

};

#endif
#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "Q3DObject.h"

class Obstacle : public Q3DObject
{
private:
	wchar_t Name [20];

public:
	Obstacle(Q3DObject* PatternObject, wchar_t* Name_);

	wchar_t* GetName() {return Name;}
};

#endif
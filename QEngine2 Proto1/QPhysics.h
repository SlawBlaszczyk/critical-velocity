#ifndef QPHYSICS_H
#define QPHYSICS_H

#define NOMINMAX

#include "QEngine.h"
#include "Structures.h"
#include "NxPhysics.h"
#include "ErrorOutputStream.h"

class QPhysics
{
	QEngine* engine;

	//main physx classes
	NxPhysicsSDK* PhysicsSDK;
	NxScene* Scene;
	NxVec3 DefaultGravity;

	ErrorOutputStream* errorOutputStream;

public:
	QPhysics(QEngine* eng);
	~QPhysics();
	int Init();
	int Update();
	int FetchResults();

	NxScene* GetScene() {return Scene;}
	NxPhysicsSDK* GetSDK() {return PhysicsSDK;}
};

#endif
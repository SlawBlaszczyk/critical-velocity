#ifndef QPHYSICALOBJECT_H
#define QPHYSICALOBJECT_H

#define NOMINMAX

#include "Q3DObject.h"
#include "NxPhysics.h"

class QPhysicalObject : public Q3DObject
{
protected:
	NxActorDesc* actorDesc;
	NxBodyDesc* bodyDesc;
	NxActor* actor;

public:
	QPhysicalObject(Game* eng, char* MeshFileName, LPCTSTR TexFileName);
	QPhysicalObject(Q3DObject* src);
	//QPhysicalObject();
	~QPhysicalObject();
	void UpdatePoseFromActor();
	virtual void InitPhysicsActor() = 0;
	void Draw();
	NxActor* GetActor() {return actor;}

};

#endif
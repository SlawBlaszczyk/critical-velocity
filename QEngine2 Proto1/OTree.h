#ifndef OTREE_H
#define OTREE_H

#include "QPhysicalObject.h"

class OTree : public QPhysicalObject
{
public:
	OTree(Game* eng, char* MeshFileName, LPCTSTR TexFileName);
	OTree(Q3DObject* src);
	virtual void InitPhysicsActor();
};

#endif

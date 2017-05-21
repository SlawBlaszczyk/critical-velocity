#ifndef OFINISHGATE_H
#define OFINISHGATE_H

#include "QPhysicalObject.h"

class OFinishGate : public QPhysicalObject
{
public:
	OFinishGate(Game* eng, char* MeshFileName, LPCTSTR TexFileName);
	OFinishGate(Q3DObject* src);
	void InitPhysicsActor();
};

#endif

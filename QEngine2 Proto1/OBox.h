#ifndef OBOX_H
#define OBOX_H

#include "QPhysicalObject.h"

class OBox : public QPhysicalObject
{
public:
	OBox(Game* eng, char* MeshFileName, LPCTSTR TexFileName);
	OBox(Q3DObject* src);
	void InitPhysicsActor();
};

#endif

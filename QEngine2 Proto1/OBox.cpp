#include "OBox.h"
#include "QPhysics.h"
#include "Game.h"

void OBox::InitPhysicsActor()
{
	//creating box 2x2m
	actorDesc = new NxActorDesc;
	bodyDesc = new NxBodyDesc;

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions	= NxVec3(1.0f, 1.0f, 1.0f);
	actorDesc->shapes.pushBack(&boxDesc);

	actorDesc->density		= 1.0f;
	actorDesc->body			= bodyDesc;

	//base position
	actorDesc->globalPose.t = NxVec3(0.0f, 0.0f, 0.0f);	//starting position

	actor = engine->physics->GetScene()->createActor(*actorDesc);	
	actor->setName("Box");

}

OBox::OBox(Game* eng, char* MeshFileName, LPCTSTR TexFileName) : QPhysicalObject(eng, MeshFileName, TexFileName)
{
	InitPhysicsActor();
}

OBox::OBox(Q3DObject *src) : QPhysicalObject(src)
{
	InitPhysicsActor();
}
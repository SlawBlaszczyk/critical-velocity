#include "OFinishGate.h"
#include "QPhysics.h"
#include "Game.h"

void OFinishGate::InitPhysicsActor()
{
	//capsule shape
	actorDesc = new NxActorDesc;
	bodyDesc = new NxBodyDesc;

	NxCapsuleShapeDesc capsuleDesc1;
	capsuleDesc1.radius	= 0.5f;
	capsuleDesc1.height	= 16.0f;
	capsuleDesc1.mass = 5000.0f;
	capsuleDesc1.density = 500.0f;
	capsuleDesc1.localPose.t = NxVec3(13.5f, 0, 0);

	NxCapsuleShapeDesc capsuleDesc2;
	capsuleDesc2.radius	= 0.5f;
	capsuleDesc2.height	= 16.0f;
	capsuleDesc2.mass = 5000.0f;
	capsuleDesc2.density = 500.0f;
	capsuleDesc2.localPose.t = NxVec3(-13.5f, 0, 0);

	//bodyDesc->flags		|= NX_BF_KINEMATIC;
	bodyDesc->mass = 5000.0f;

	actorDesc->shapes.pushBack(&capsuleDesc1);
	actorDesc->shapes.pushBack(&capsuleDesc2);
	actorDesc->density		= 500.0f;
	actorDesc->body			= NULL;

	//base position
	actorDesc->globalPose.t = NxVec3(0.0f, 0.0f, 0.0f);	//starting position

	actor = engine->physics->GetScene()->createActor(*actorDesc);

	actor->setName("FinishGate");
}

OFinishGate::OFinishGate(Game* eng, char* MeshFileName, LPCTSTR TexFileName) : QPhysicalObject(eng, MeshFileName, TexFileName)
{
	InitPhysicsActor();
}

OFinishGate::OFinishGate(Q3DObject *src) : QPhysicalObject(src)
{
	InitPhysicsActor();
}
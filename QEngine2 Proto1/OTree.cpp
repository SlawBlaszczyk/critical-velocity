#include "OTree.h"
#include "QPhysics.h"
#include "Game.h"

void OTree::InitPhysicsActor()
{
	//capsule shape
	actorDesc = new NxActorDesc;
	bodyDesc = new NxBodyDesc;

	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.radius	= 0.5f;
	capsuleDesc.height	= 16.0f;
	capsuleDesc.mass = 5000.0f;
	capsuleDesc.density = 500.0f;

	//bodyDesc->flags		|= NX_BF_KINEMATIC;
	bodyDesc->mass = 5000.0f;

	actorDesc->shapes.pushBack(&capsuleDesc);
	actorDesc->density		= 500.0f;
	actorDesc->body			= NULL;

	//base position
	actorDesc->globalPose.t = NxVec3(0.0f, 0.0f, 0.0f);	//starting position

	actor = engine->physics->GetScene()->createActor(*actorDesc);

	//actor->setCMassOffsetLocalPosition(NxVec3(0.0f, -3.0f, 0.0f));
	actor->setName("Tree");

}

OTree::OTree(Game* eng, char* MeshFileName, LPCTSTR TexFileName) : QPhysicalObject(eng, MeshFileName, TexFileName)
{
	InitPhysicsActor();
}

OTree::OTree(Q3DObject *src) : QPhysicalObject(src)
{
	//actorDesc = NULL;
	//bodyDesc = NULL;
	//actor = NULL;

	InitPhysicsActor();
}
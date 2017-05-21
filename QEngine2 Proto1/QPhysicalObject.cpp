#include "QPhysicalObject.h"
#include "Game.h"
#include "QPhysics.h"
#include "IQGraphics.h"

QPhysicalObject::QPhysicalObject(Game* eng, char* MeshFileName, LPCTSTR TexFileName) : Q3DObject(eng, MeshFileName, TexFileName)
{
	this->engine = eng;
	actorDesc = NULL;
	bodyDesc = NULL;
	actor = NULL;

	//InitPhysicsActor();
}

QPhysicalObject::QPhysicalObject(Q3DObject *src) : Q3DObject(src)
{
	actorDesc = NULL;
	bodyDesc = NULL;
	actor = NULL;

	//InitPhysicsActor();
}

//QPhysicalObject::QPhysicalObject()
//{
//	actorDesc = NULL;
//	bodyDesc = NULL;
//	actor = NULL;
//}

QPhysicalObject::~QPhysicalObject()
{
	engine->physics->GetScene()->releaseActor(*actor);
	delete actorDesc;
	delete bodyDesc;
}

//void QPhysicalObject::InitPhysicsActor()
//{
//	//creating simplest box 3x3m
//	actorDesc = new NxActorDesc;
//	bodyDesc = new NxBodyDesc;
//
//	NxBoxShapeDesc boxDesc;
//	boxDesc.dimensions	= NxVec3(1.0f, 1.0f, 1.0f);
//	actorDesc->shapes.pushBack(&boxDesc);
//
//	actorDesc->density		= 1.0f;
//	actorDesc->body			= bodyDesc;
//
//	//base position
//	actorDesc->globalPose.t = NxVec3(1040.0f, 106.5f, 476.0f);	//starting position
//
//	actor = engine->physics->GetScene()->createActor(*actorDesc);
//	actor->setName("DefaultBox");
//}

void QPhysicalObject::Draw()
{
	engine->graphics->DrawObject(GetMeshIndex(), GetTextureIndex(), TransformMatrix,
		GetVertices(), GetPolys());
}

void QPhysicalObject::UpdatePoseFromActor()
{
	NxMat34 pose = actor->getGlobalPose();

	//Full pose copy
	NxF32 srcmat [4][4];
	pose.getColumnMajor44(srcmat);

	TransformMatrix._11 = srcmat[0][0];
	TransformMatrix._12 = srcmat[0][1];
	TransformMatrix._13 = srcmat[0][2];
	TransformMatrix._14 = srcmat[0][3];

	TransformMatrix._21 = srcmat[1][0];
	TransformMatrix._22 = srcmat[1][1];
	TransformMatrix._23 = srcmat[1][2];
	TransformMatrix._24 = srcmat[1][3];

	TransformMatrix._31 = srcmat[2][0];
	TransformMatrix._32 = srcmat[2][1];
	TransformMatrix._33 = srcmat[2][2];
	TransformMatrix._34 = srcmat[2][3];

	TransformMatrix._41 = srcmat[3][0];
	TransformMatrix._42 = srcmat[3][1];
	TransformMatrix._43 = srcmat[3][2];
	TransformMatrix._44 = srcmat[3][3];
}
#include "Vehicle.h"
#include "Frame.h"
#include "Q3DObject.h"
#include "TimeManager.h"
#include "Game.h"
#include "Level.h"
#include "Logic.h"
#include "QMath.h"
#include "QPhysics.h"
#include "IQSound.h"
#include "IQGraphics.h"
#include "ParticleSystem.h"
#include "OrbitCam.h"
#include "../QGraphicsD3D/Camera.h"
#include <cstdlib>

Vehicle::Vehicle(Game* eng) : QVObject()
{
	this->engine = eng;
	for(int i=0; i<20; i++)
		Frames[i] = NULL;
	RootFrame = NULL;
	actorDesc = NULL;
	bodyDesc = NULL;
	actor = NULL;
	wheelMat = NULL;
	wheelShapeFR = NULL;
	wheelShapeFL = NULL;
	wheelShapeRR = NULL;
	wheelShapeRL = NULL;

	WheelSteering = STEER_NONE;
	WheelTurningSpeed = 1.0f;
	WheelTurningLimit = QMath::DegToRad(30.0f);

	bAnimate = true;
	bTurning = false;
	MaxSpeed = 3.0f;
	InitFrames();
	InitPhysicsActor();
	UpdateFrameMatrices();
	InitSounds();
	InitParticleSystems();

	orbitCam = new OrbitCam(eng, this);
}

Vehicle::~Vehicle()
{
	//reset LOD frames
	Frames[CAR_BODY]->Model->SetMeshIndex(engine->logic->car1body->GetMeshIndex());
	Frames[CAR_BODY]->Model->SetVertices(engine->logic->car1body->GetVertices());
	Frames[CAR_BODY]->Model->SetPolys(engine->logic->car1body->GetPolys());

	//destroy all the frames
	for(int i=0; i<20; i++)
		if(Frames[i] != NULL)
			delete Frames[i];

	//destroy physics components
	actor->releaseShape(*wheelShapeFR);
	actor->releaseShape(*wheelShapeFL);
	actor->releaseShape(*wheelShapeRR);
	actor->releaseShape(*wheelShapeRL);
	engine->physics->GetScene()->releaseActor(*actor);
	engine->physics->GetScene()->releaseMaterial(*wheelMat);
	delete actorDesc;
	delete bodyDesc;

	//delete particle systems
	if (dustmaker[0] != NULL)
		delete dustmaker[0];
	if (dustmaker[1] != NULL)
		delete dustmaker[1];

	//delete camera
	if (orbitCam != NULL)
		delete orbitCam;
}

void Vehicle::InitFrames()
{

	for (int i=0; i<20; i++)
		Frames[i] = new Frame(engine);

	//setup each hierarchically relative frame position
	Frames[CAR_BODY]->Model = new Q3DObject(engine->logic->car1body);
	//Frames[CAR_BODY]->Position.z = 476.0f;
	//Frames[CAR_BODY]->Position.x = 1020.5f;
	//Frames[CAR_BODY]->Position.y = 96.5f;
	Frames[CAR_BODY]->Position.z = 0.0f;
	Frames[CAR_BODY]->Position.x = 0.0f;
	Frames[CAR_BODY]->Position.y = 0.0f;
	Frames[CAR_BODY]->Rotation.x = 5.4f; //D3DX_PI;

	Frames[CAR_WHEEL_FR]->Model = new Q3DObject(engine->logic->car1wheel);
	Frames[CAR_WHEEL_FR]->Position.z = 1.513f;
	Frames[CAR_WHEEL_FR]->Position.x = -0.196f;
	Frames[CAR_WHEEL_FR]->Position.y = -1.715f;
	Frames[CAR_WHEEL_FR]->Rotation.y = D3DX_PI;

	Frames[CAR_WHEEL_FL]->Model = new Q3DObject(engine->logic->car1wheel);
	Frames[CAR_WHEEL_FL]->Position.z = -1.513f;
	Frames[CAR_WHEEL_FL]->Position.x = -0.196f;
	Frames[CAR_WHEEL_FL]->Position.y = -1.715f;
	Frames[CAR_WHEEL_FL]->Rotation.y = 0.0f;

	Frames[CAR_WHEEL_RL]->Model = new Q3DObject(engine->logic->car1wheel);
	Frames[CAR_WHEEL_RL]->Position.z = -1.513f;
	Frames[CAR_WHEEL_RL]->Position.x = 5.554f;
	Frames[CAR_WHEEL_RL]->Position.y = -1.715f;
	Frames[CAR_WHEEL_RL]->Rotation.y = 0.0f;

	Frames[CAR_WHEEL_RR]->Model = new Q3DObject(engine->logic->car1wheel);
	Frames[CAR_WHEEL_RR]->Position.z = 1.513f;
	Frames[CAR_WHEEL_RR]->Position.x = 5.554f;
	Frames[CAR_WHEEL_RR]->Position.y = -1.715f;
	Frames[CAR_WHEEL_RR]->Rotation.y = D3DX_PI;

	Frames[CAR_FRONTLIGHTS]->Model = new Q3DObject(engine->logic->car1frontlights);
	Frames[CAR_SPOILER]->Model = new Q3DObject(engine->logic->car1spoiler);
	Frames[CAR_EXHAUSTL]->Model = new Q3DObject(engine->logic->car1exhaustl);
	Frames[CAR_EXHAUSTR]->Model = new Q3DObject(engine->logic->car1exhaustr);
	Frames[CAR_FLOORREAR]->Model = new Q3DObject(engine->logic->car1floorrear);
	Frames[CAR_HALOGEN]->Model = new Q3DObject(engine->logic->car1halogen);
	Frames[CAR_TAILLIGHTSOUT]->Model = new Q3DObject(engine->logic->car1taillightsout);
	Frames[CAR_TAILLIGHTSIN]->Model = new Q3DObject(engine->logic->car1taillightsin);


	//link frames into hierarchy
	Frames[CAR_BODY]->pChild = Frames[CAR_FRONTLIGHTS];
	Frames[CAR_FRONTLIGHTS]->pSibling = Frames[CAR_WHEEL_FR];
	Frames[CAR_WHEEL_FR]->pSibling = Frames[CAR_WHEEL_FL];
	Frames[CAR_WHEEL_FL]->pSibling = Frames[CAR_WHEEL_RL];
	Frames[CAR_WHEEL_RL]->pSibling = Frames[CAR_WHEEL_RR];
	
	Frames[CAR_WHEEL_RR]->pSibling = Frames[CAR_SPOILER];
	Frames[CAR_SPOILER]->pSibling = Frames[CAR_EXHAUSTL];
	Frames[CAR_EXHAUSTL]->pSibling = Frames[CAR_EXHAUSTR];
	Frames[CAR_EXHAUSTR]->pSibling = Frames[CAR_FLOORREAR];
	Frames[CAR_FLOORREAR]->pSibling = Frames[CAR_HALOGEN];
	Frames[CAR_HALOGEN]->pSibling = Frames[CAR_TAILLIGHTSOUT];
	Frames[CAR_TAILLIGHTSOUT]->pSibling = Frames[CAR_TAILLIGHTSIN];

	
	RootFrame = Frames[CAR_BODY];

	//map vehicle rotation and position to root frame
	Position = Frames[CAR_BODY]->Position;
	Rotation = Frames[CAR_BODY]->Rotation;

	//engine->SetAnimate(true);
}

void Vehicle::UpdateFrameMatrices()
{
	//get root global matrix from physics actor
	UpdateRootPoseMatrixFromPhysicsActor();

	//propagate root frame matrix through frame hierarchy
	Frames[CAR_FRONTLIGHTS]->UpdateHierarchy(RootFrame->TransformMatrix);
}

void Vehicle::Draw()
{
	RootFrame->DrawHierarchy();

	//draw particles
	dustmaker[0]->Draw();
	dustmaker[1]->Draw();
}

int Vehicle::InitPhysicsActor()
{
	actorDesc = new NxActorDesc;
	bodyDesc = new NxBodyDesc;

	//create wheel material without classic friction model
	if (!wheelMat)
    {
        NxMaterialDesc m;
        //m.flags |= NX_MF_DISABLE_FRICTION;	///////////FOR DEBUG OFF
        wheelMat = engine->physics->GetScene()->createMaterial(m);
    }

	//create shapes

	//rigid car body main box shape
	float size = 1.0f;
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3(4.91f, 1.04f, 1.92f);
	boxDesc.localPose.t = NxVec3(2.72f, -1.00f, 0.00f);
	boxDesc.density = 1.0f;

	//wheel shapes

	//default wheel descriptor
	NxWheelShapeDesc wheelDesc;
	wheelDesc.radius = 0.75f;
	wheelDesc.materialIndex = wheelMat->getMaterialIndex();
	wheelDesc.wheelFlags = NX_WF_INPUT_LNG_SLIPVELOCITY | NX_WF_INPUT_LAT_SLIPVELOCITY;
	wheelDesc.inverseWheelMass = 0.05f;  //experimental
	wheelDesc.mass = 20.0f;
	wheelDesc.density = 2.0f;
	//wheelDesc.inverseWheelMass = 0.1f;  //experimental
	//wheelDesc.mass = 10.0f;
	//wheelDesc.density = 1.0f;
	wheelDesc.name = "Wheel";
	wheelDesc.suspensionTravel = 0.02f;
	wheelDesc.suspension.damper = 0;
	wheelDesc.suspension.spring = 7000;


	//create actor
	//bodyDesc->mass = 5000;
	actorDesc->shapes.push_back(&boxDesc);	//adding box shape
	actorDesc->body = bodyDesc;
	actorDesc->density = 1.0f;
	actorDesc->globalPose.t = NxVec3(1020.0f, 102.5f, 476.0f);	//starting position
	//actorDesc->globalPose.t = NxVec3(869.0f, 256.5f, 617.0f);	//starting position

	actor = engine->physics->GetScene()->createActor(*actorDesc);
	actor->setName("Player Car");
	actor->setCMassOffsetLocalPosition(NxVec3(1.7f, -2.0f, 0.0f));	//center of mass local position

	//WARNING: center of mass too low

	//add wheels to actor
	//set initial position and rotation of each wheel in desc and add it to the actor

	//rotation of all wheels
	NxQuat q;
    q.fromAngleAxis(90, NxVec3(0,1,0));
    wheelDesc.localPose.M.fromQuat(q);

	//wheel FL
	wheelDesc.localPose.t = NxVec3(-0.196f, -1.715f, 1.513f);
	wheelShapeFL = static_cast<NxWheelShape *>(actor->createShape(wheelDesc));

	//wheel FR
	wheelDesc.localPose.t = NxVec3(-0.196f, -1.715f, -1.513f);
	wheelShapeFR = static_cast<NxWheelShape *>(actor->createShape(wheelDesc));

	//wheel RR
	wheelDesc.localPose.t = NxVec3(5.554f, -1.715f, -1.513f);
	wheelShapeRR = static_cast<NxWheelShape *>(actor->createShape(wheelDesc));

	//wheel RL
	wheelDesc.localPose.t = NxVec3(5.554f, -1.715f, 1.513f);
	wheelShapeRL = static_cast<NxWheelShape *>(actor->createShape(wheelDesc));

	LastPosition = actor->getGlobalPosition();
	BaseOrientation = actor->getGlobalOrientationQuat();

	return 1;
}

int Vehicle::SetTorqueRear(float torque)
{
	actor->wakeUp();

	wheelShapeRR->setMotorTorque(torque);
	wheelShapeRL->setMotorTorque(torque);

	return 1;
}

int Vehicle::SetBrakes(float brakes)
{
	wheelShapeFR->setBrakeTorque(brakes);
	wheelShapeFL->setBrakeTorque(brakes);
	wheelShapeRR->setBrakeTorque(brakes);
	wheelShapeRL->setBrakeTorque(brakes);
	return 1;
}

int Vehicle::SetSteerAngle(float steerAngle)
{
	wheelShapeFR->setSteerAngle(steerAngle);
	wheelShapeFL->setSteerAngle(steerAngle);
	return 1;
}

void Vehicle::ProcessSteering()
{
	float SteerAngle = GetSteerAngle();

	switch(WheelSteering)
	{
	case STEER_LEFT:
		//turn wheels left

		SteerAngle -= WheelTurningSpeed * engine->tman->GetDeltaTime();

		//check if the wheel turn angle is over the constraint limit
		if (SteerAngle < (WheelTurningLimit * -1.0f))
		{
			SteerAngle = (WheelTurningLimit * -1.0f);
		}

		break;

	case STEER_RIGHT:
		//turn wheels right

		SteerAngle += WheelTurningSpeed * engine->tman->GetDeltaTime();

		//check if the wheel turn angle is over the constraint limit
		if (SteerAngle > WheelTurningLimit)
		{
			SteerAngle = WheelTurningLimit;
		}

		break;

	case STEER_BOTH:
		//don't turn the wheels

		break;

	case STEER_NONE:
		//straighten the wheels

		//determine the way of turning
		if(SteerAngle < 0)
		{
			//turn right until zero
			SteerAngle += WheelTurningSpeed * engine->tman->GetDeltaTime();

			//check if the wheel turn is beyond zero
			if (SteerAngle > 0)
				SteerAngle = 0.0f;
		}
		else if (SteerAngle > 0)
		{
			//turn left until zero
			SteerAngle -= WheelTurningSpeed * engine->tman->GetDeltaTime();
			
			//check if the wheel turn is beyond zero
			if (SteerAngle < 0)
				SteerAngle = 0.0f;
		}

		break;
	}

	//apply calculated angle to wheel shape
	SetSteerAngle(SteerAngle);
	
	WheelSteering = STEER_NONE;
}

float Vehicle::GetTorqueRear()
{
	return wheelShapeRL->getMotorTorque();
}

float Vehicle::GetBrakes()
{
	return wheelShapeRL->getBrakeTorque();
}

float Vehicle::GetSteerAngle()
{
	return wheelShapeFL->getSteerAngle();
}

int Vehicle::UpdateRootPoseMatrixFromPhysicsActor()
{
	NxVec3 position = actor->getGlobalPosition();
	NxMat33 orient = actor->getGlobalOrientation();
	NxMat34 pose = actor->getGlobalPose();

	//Full pose copy
	NxF32 srcmat [4][4];
	pose.getColumnMajor44(srcmat);

	RootFrame->TransformMatrix._11 = srcmat[0][0];
	RootFrame->TransformMatrix._12 = srcmat[0][1];
	RootFrame->TransformMatrix._13 = srcmat[0][2];
	RootFrame->TransformMatrix._14 = srcmat[0][3];

	RootFrame->TransformMatrix._21 = srcmat[1][0];
	RootFrame->TransformMatrix._22 = srcmat[1][1];
	RootFrame->TransformMatrix._23 = srcmat[1][2];
	RootFrame->TransformMatrix._24 = srcmat[1][3];

	RootFrame->TransformMatrix._31 = srcmat[2][0];
	RootFrame->TransformMatrix._32 = srcmat[2][1];
	RootFrame->TransformMatrix._33 = srcmat[2][2];
	RootFrame->TransformMatrix._34 = srcmat[2][3];

	RootFrame->TransformMatrix._41 = srcmat[3][0];
	RootFrame->TransformMatrix._42 = srcmat[3][1];
	RootFrame->TransformMatrix._43 = srcmat[3][2];
	RootFrame->TransformMatrix._44 = srcmat[3][3];

	return 1;
}

void Vehicle::UpdateWheelMatrices()
{
	Frames[CAR_WHEEL_FR]->Rotation.x = GetSteerAngle();
	Frames[CAR_WHEEL_FL]->Rotation.x = GetSteerAngle();

	float stala = 0.5f;
	float speed = wheelShapeFR->getAxleSpeed() * engine->tman->GetDeltaTime() * stala;
	Frames[CAR_WHEEL_FL]->Rotation.z -= speed;

	speed = wheelShapeRR->getAxleSpeed() * engine->tman->GetDeltaTime() * stala;
	Frames[CAR_WHEEL_RL]->Rotation.z -= speed;

	speed = wheelShapeFL->getAxleSpeed() * engine->tman->GetDeltaTime() * stala;
	Frames[CAR_WHEEL_FR]->Rotation.z += speed;

	speed = wheelShapeRL->getAxleSpeed() * engine->tman->GetDeltaTime() * stala;
	Frames[CAR_WHEEL_RR]->Rotation.z += speed;
}

void Vehicle::Animate(float dDeltaTime, float dTime)
{
	//if((engine->GetAnimate()) && (bAnimate))
	//{
	//	Frames[5]->Rotation.y += dDeltaTime * 0.8f * sin (dTime*2);
	//	Frames[7]->Rotation.y -= dDeltaTime * 0.8f * sin (dTime*2);

	//	Frames[6]->Rotation.y = (-30.0f * D3DX_PI / 180.0f) + Frames[5]->Rotation.y;
	//	Frames[8]->Rotation.y = (-30.0f * D3DX_PI / 180.0f) + Frames[7]->Rotation.y;

	//	Frames[1]->Rotation.y += dDeltaTime * 0.8f * sin (dTime*2);
	//	Frames[2]->Rotation.y += dDeltaTime * 0.8f * sin (dTime*2);

	//	Frames[3]->Rotation.y = (30.0f * D3DX_PI / 180.0f) + Frames[1]->Rotation.y;
	//	Frames[4]->Rotation.y = (30.0f * D3DX_PI / 180.0f) - Frames[1]->Rotation.y;
	//}

	//Frames[0]->Position.x += dDeltaTime * engine->logic->cmovement;
}

void Vehicle::CheckSituation()
{

}

void Vehicle::Seek(D3DXVECTOR3)
{
	D3DXVECTOR3 DesiredVel = TargetLocation - Frames[CAR_BODY]->Position;
	DesiredVel = QMath::LimitVector3(DesiredVel, MaxSpeed);
	Steering += DesiredVel;
}

void Vehicle::ApplySteering()
{
	////cl
	//Steering = QMath::LimitVector3(Steering, 1.0f);
	//Velocity += Steering;
	//Velocity = QMath::LimitVector3(Velocity, MaxSpeed);
	//Steering = D3DXVECTOR3(0, 0, 0);
}

void Vehicle::Move()
{
	//if(!engine->GetAnimate()) return;
	//Frames[0]->Position += Velocity * engine->tman->GetDeltaTime();
}

void Vehicle::CalcRotation()
{
	D3DXVECTOR3 lv(-1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 heading = Velocity;
	D3DXVec3Normalize(&heading, &heading);
	float cos_alpha = D3DXVec3Dot(&heading, &lv);
	Frames[CAR_BODY]->Rotation.x = acos(cos_alpha);  //Rotation.y?

	if(heading.z > 0)	//heading.x?
		Frames[CAR_BODY]->Rotation.x *= (-1);
}

void Vehicle::ResetPosition()
{
	NxVec3 pos = actor->getGlobalPosition();
	pos.y += 1.0f;
	actor->setGlobalPosition(pos);
	actor->setGlobalOrientationQuat(BaseOrientation);
}

void Vehicle::InitSounds()
{
	NxVec3 src_pos = actor->getGlobalPosition();
	QVector3 pos = QVector3(src_pos.x, src_pos.y, src_pos.z);
	QVector3 vel = QVector3(0, 0, 0);

	EngineSound = engine->sound->Create3DSample("sounds\\car_idle.wav", true);
	EngineChannel = engine->sound->PlaySound(EngineSound, true);
	engine->sound->Set3DChannelParams(EngineChannel, pos, vel);
	
	//sound DEBUG TURN OFF
	engine->sound->PauseChannel(EngineChannel, false);

	//get base engine sound frequency
	engine->sound->GetChannelFrequency(EngineChannel, &EngineBaseFreq); 
}

void Vehicle::UpdateEngineSound()
{
	//update engine sound channel position, velocity
	NxVec3 src_pos = actor->getGlobalPosition();
	QVector3 pos = QVector3(src_pos.x, src_pos.y, src_pos.z);
	QVector3 vel = QVector3(0, 0, 0);
	
	engine->sound->Set3DChannelParams(EngineChannel, pos, vel);

	//set frequency
	float stala = -0.02f;
	float mod = wheelShapeRR->getAxleSpeed() * stala;
	float freq = EngineBaseFreq * mod;

	if ((freq > EngineBaseFreq) && (freq < 13500))
		engine->sound->SetChannelFrequency(EngineChannel, freq);
}

void Vehicle::Update()
{
	UpdateFrameMatrices();
	UpdateWheelMatrices();
	UpdateEngineSound();
	SelectLODInstance();
	UpdateParticleSystems();
	orbitCam->UpdateOrbitRotation();
}

void Vehicle::InitParticleSystems()
{
	NxVec3 nv = actor->getGlobalPosition();
	LastDustEmissionPosition = QVector3(nv.x, nv.y, nv.z);

	dustmaker[0] = new ParticleSystem(engine, L"textures/perlin noise 128.dds", QVector3(0, 0, 0), 3.0f);	//3 seconds decay time
	dustmaker[1] = new ParticleSystem(engine, L"textures/perlin noise 128.dds", QVector3(0, 0, 0), 3.0f);
}

void Vehicle::UpdateParticleSystems()
{
	//update Position based on full transform matrix
	D3DXMATRIX mat;
	D3DXVECTOR4 pos4;
	D3DXVECTOR3 orgpos (0, 0, 0);

	//dustmaker0
	mat = Frames[CAR_WHEEL_RL]->TransformMatrix;
	D3DXVec3Transform(&pos4, &orgpos, &mat);
	dustmaker[0]->SetPosition(QVector3(pos4.x, pos4.y, pos4.z));

	//dustmaker1
	mat = Frames[CAR_WHEEL_RR]->TransformMatrix;
	D3DXVec3Transform(&pos4, &orgpos, &mat);
	dustmaker[1]->SetPosition(QVector3(pos4.x, pos4.y, pos4.z));

	//check distance for emission
	double dist;
	QVector3 vdist, cpos;
	NxVec3 nv = actor->getGlobalPosition();
	cpos = QVector3(nv.x, nv.y, nv.z);
	vdist = cpos - LastDustEmissionPosition;
	dist = vdist.Magnitude();

	if (dist > 1.0f)
	{
		//emit particles
		dustmaker[0]->Emit();
		dustmaker[1]->Emit();

		//update emission pos
		LastDustEmissionPosition = cpos;
	}

	//update systems
	dustmaker[0]->Update();
	dustmaker[1]->Update();
}

void Vehicle::SelectLODInstance()
{
	//calculate distance to camera
	double dist;
	NxVec3 src_pos = actor->getGlobalPosition();
	QVector3 pos = QVector3(src_pos.x, src_pos.y, src_pos.z);

	D3DXVECTOR3 src_cam_pos = engine->graphics->GetCurrentCam()->Position;
	QVector3 cam_pos = QVector3(src_cam_pos.x, src_cam_pos.y, src_cam_pos.z);

	QVector3 vdist = pos - cam_pos;
	dist = vdist.Magnitude();	//final distance value

	//switch lod level
	if(dist < 20)
	{
		LODLevel = 0;
		//Frames[CAR_BODY]->Model = engine->logic->car1body;
		Frames[CAR_BODY]->Model->SetMeshIndex(engine->logic->car1body->GetMeshIndex());
		Frames[CAR_BODY]->Model->SetVertices(engine->logic->car1body->GetVertices());
		Frames[CAR_BODY]->Model->SetPolys(engine->logic->car1body->GetPolys());
		//Frames[CAR_WHEEL_FL]->Model = engine->logic->car1wheel;
		//Frames[CAR_WHEEL_FR]->Model = engine->logic->car1wheel;
		//Frames[CAR_WHEEL_RL]->Model = engine->logic->car1wheel;
		//Frames[CAR_WHEEL_RR]->Model = engine->logic->car1wheel;
	}
	else if (dist < 40)
	{
		LODLevel = 1;
		//Frames[CAR_BODY]->Model = engine->logic->car1body_lod1;
		Frames[CAR_BODY]->Model->SetMeshIndex(engine->logic->car1body_lod1->GetMeshIndex());
		Frames[CAR_BODY]->Model->SetVertices(engine->logic->car1body_lod1->GetVertices());
		Frames[CAR_BODY]->Model->SetPolys(engine->logic->car1body_lod1->GetPolys());
		//Frames[CAR_WHEEL_FL]->Model = engine->logic->car1wheel_lod1;
		//Frames[CAR_WHEEL_FR]->Model = engine->logic->car1wheel_lod1;
		//Frames[CAR_WHEEL_RL]->Model = engine->logic->car1wheel_lod1;
		//Frames[CAR_WHEEL_RR]->Model = engine->logic->car1wheel_lod1;
	}
	else if (dist < 70)
	{
		LODLevel = 2;
		//Frames[CAR_BODY]->Model = engine->logic->car1body_lod2;
		Frames[CAR_BODY]->Model->SetMeshIndex(engine->logic->car1body_lod2->GetMeshIndex());
		Frames[CAR_BODY]->Model->SetVertices(engine->logic->car1body_lod2->GetVertices());
		Frames[CAR_BODY]->Model->SetPolys(engine->logic->car1body_lod2->GetPolys());
		//Frames[CAR_WHEEL_FL]->Model = engine->logic->car1wheel_lod2;
		//Frames[CAR_WHEEL_FR]->Model = engine->logic->car1wheel_lod2;
		//Frames[CAR_WHEEL_RL]->Model = engine->logic->car1wheel_lod2;
		//Frames[CAR_WHEEL_RR]->Model = engine->logic->car1wheel_lod2;
	}
	else
	{
		LODLevel = 3;
		//Frames[CAR_BODY]->Model = engine->logic->car1body_lod3;
		Frames[CAR_BODY]->Model->SetMeshIndex(engine->logic->car1body_lod3->GetMeshIndex());
		Frames[CAR_BODY]->Model->SetVertices(engine->logic->car1body_lod3->GetVertices());
		Frames[CAR_BODY]->Model->SetPolys(engine->logic->car1body_lod3->GetPolys());
		//Frames[CAR_WHEEL_FL]->Model = engine->logic->car1wheel_lod3;
		//Frames[CAR_WHEEL_FR]->Model = engine->logic->car1wheel_lod3;
		//Frames[CAR_WHEEL_RL]->Model = engine->logic->car1wheel_lod3;
		//Frames[CAR_WHEEL_RR]->Model = engine->logic->car1wheel_lod3;
	}

}
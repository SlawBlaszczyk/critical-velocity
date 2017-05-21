#ifndef CREATURE_H
#define CREATURE_H

#define NOMINMAX

#include <D3DX9.h>
#include "QVObject.h"
#include "Structures.h"
#include "Goal.h"
#include "QVector3.h"
#include "NxPhysics.h"

class Frame;
class ParticleSystem;
class OrbitCam;

//steering constants
#define STEER_NONE 0
#define STEER_LEFT 1
#define STEER_RIGHT 2
#define STEER_BOTH 3

class Vehicle : public QVObject
{
public:

	int State; //creature AI state
	Goal* CurrentGoal; //AI goal

	bool bAnimate;
	bool bTurning;	//state of the creature (turn or walk)
	Frame* Frames[20];
	Frame* RootFrame;

	D3DXVECTOR3 Velocity;
	D3DXVECTOR3 Steering;
	D3DXVECTOR3 TargetLocation;
	int numberOfTargetLocationFromGlobalLocations;		//kaska
	float MaxSpeed;

	//physics data
	NxActorDesc* actorDesc;
	NxBodyDesc* bodyDesc;
	NxActor* actor;
	NxMaterial* wheelMat;
	NxWheelShape* wheelShapeFR;
	NxWheelShape* wheelShapeFL;
	NxWheelShape* wheelShapeRR;
	NxWheelShape* wheelShapeRL;

	ParticleSystem* dustmaker [2];
	QVector3 LastDustEmissionPosition;

	NxVec3 LastPosition; //position of a vehicle in previous frame
	NxVec3 PositionBeforeLast;
	int LODLevel;

	//orbit camera
	OrbitCam* orbitCam;

	//sound data
	int EngineSound;
	int EngineChannel;
	float EngineBaseFreq;

	//used to determine if the wheels are turning
	int WheelSteering;
	float WheelTurningSpeed;
	float WheelTurningLimit;	//in radians
	
	void SetWorldTransform(D3DXMATRIX* mWorld);
	HRESULT AdvanceTime(double TimeDelta);
	Vehicle(Game* eng);
	~Vehicle();
	void InitFrames();
	void UpdateFrameMatrices();	//update pose matrices using physics data
	void UpdateWheelMatrices();	//directly from global actor poses

	void Update();	//general update function

	void Draw();
	void SelectLODInstance();
	void InitParticleSystems();
	void UpdateParticleSystems();

	void Animate(float dDeltaTime, float dTime);

	//physics functions
	int InitPhysicsActor();
	int UpdateRootPoseMatrixFromPhysicsActor(); //copy pose from physics to core object for rendering
	int SetTorqueRear(float torque);
	int SetBrakes(float brakes);
	int SetSteerAngle(float steerAngle);
	float GetTorqueRear();
	float GetBrakes();
	float GetSteerAngle();
	void ProcessSteering();

	NxQuat BaseOrientation;
	void ResetPosition(); //puts the car back on wheels

	//sound functions
	void InitSounds();
	void UpdateEngineSound();

	//AI functions
	void Seek(D3DXVECTOR3); //seek a location

	void SeekNewTarget();
	void CheckTarget();	//checks if the target wasn't reached

	void CheckSituation(); //check for AI situation changes

	void Move();

	//this function is used for applying steering forces to the vehicle
	void ApplySteering();
	void SetGoal(Goal* g) {CurrentGoal = g;}

	//calculates rotation angle (Rotation) based on Velocity vector direction
	void CalcRotation();
};

#endif
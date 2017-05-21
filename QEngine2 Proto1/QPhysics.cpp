#include "QPhysics.h"
#include "TimeManager.h"

QPhysics::QPhysics(QEngine* eng)
{
	engine = eng;
	PhysicsSDK = NULL;
	Scene = NULL;

	errorOutputStream = new ErrorOutputStream;
}

QPhysics::~QPhysics()
{
	if(Scene != NULL)
		PhysicsSDK->releaseScene(*Scene);

	if(PhysicsSDK != NULL)
		PhysicsSDK->release();

	delete errorOutputStream;
}

int QPhysics::Init()
{
	//create SDK
	PhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, errorOutputStream);

	if(PhysicsSDK == NULL)
		engine->Decease(L"Couldn't load physics SDK.");

	//create scene
	NxSceneDesc sceneDesc;
	sceneDesc.simType = NX_SIMULATION_SW;
	sceneDesc.gravity = NxVec3(0, -100.0f, 0);

	Scene = PhysicsSDK->createScene(sceneDesc);
	if (Scene == NULL)
		engine->Decease(L"Couldn't create physics scene.");

	//create the default material
	NxMaterial* defaultMaterial = Scene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.5);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.5);

	//connect to the visual remote debugger
	PhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect ("localhost", 5425);

	return 1;
}

int QPhysics::Update()
{
	//update simulation by time interval in seconds
	Scene->simulate(engine->tman->GetDeltaTime());
	Scene->flushStream();

	return 1;
}

int QPhysics::FetchResults()
{
	//fetch simulation results
	while (!Scene->fetchResults(NX_RIGID_BODY_FINISHED, false));

	return 1;
}
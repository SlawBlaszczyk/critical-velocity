#include "Logic.h"
#include "Level.h"
#include "..\QGraphicsD3D\Camera.h"
#include "IQGraphics.h"
#include "TimeManager.h"
#include "Frame.h"
#include "QMath.h"
#include <iostream>

Logic::Logic(Game *eng)
{
	engine = eng;

	//nullifying pointers
	level = NULL;
	car1 = NULL;
	car2 = NULL;
	//car3 = NULL;
	car1body = NULL;
	car1wheel = NULL;

	LevelInitialized = false;
	CamLinkedWithPlayer = false;
}

Logic::~Logic()
{
	if(car1 != NULL)
		delete car1;
	if(car1body != NULL)
		delete car1body;
	if(car1wheel != NULL)
		delete car1wheel;

	if(level != NULL)
		delete level;
}

void Logic::Init()
{
	//load level objects
	PatternTree01 = new Q3DObject(engine, "meshes/drzewko1.3ds", L"textures/fir2_modded.png");
	PatternBox = new OBox(engine, "meshes/box22.3ds", L"textures/box.tga");
	PatternTree02 = new OTree(engine, "meshes/drzewko1.3ds", L"textures/fir2_modded.png");
	PatternFinishGate = new OFinishGate(engine, "meshes/finish.3ds", L"textures/steelplt2.jpg");
	
	//init goal objects

	//load game models
	car1body = new Q3DObject(engine, "meshes/skyline 5500f.3ds", L"textures/Skyline Body 2048 GAME_rotation.jpg");
	car1body_lod1 = new Q3DObject(engine, "meshes/skyline 5500f.3ds");
	car1body_lod2 = new Q3DObject(engine, "meshes/skyline 1000f.3ds");
	car1body_lod3 = new Q3DObject(engine, "meshes/skyline 300f.3ds");
	//car1body_lod1->SetTextureIndex(car1body->GetTextureIndex());
	//car1body_lod2->SetTextureIndex(car1body->GetTextureIndex());
	//car1body_lod3->SetTextureIndex(car1body->GetTextureIndex());
	
	car1wheel = new Q3DObject(engine, "meshes/skywheel 2000f.3ds", L"textures/dkgrey.jpg");
	car1wheel_lod1 = new Q3DObject(engine, "meshes/skywheel 2000f.3ds");
	car1wheel_lod2 = new Q3DObject(engine, "meshes/skywheel 2000f.3ds");
	car1wheel_lod3 = new Q3DObject(engine, "meshes/skywheel 300f.3ds");
	//car1wheel_lod1->SetTextureIndex(car1wheel->GetTextureIndex());
	//car1wheel_lod2->SetTextureIndex(car1wheel->GetTextureIndex());
	//car1wheel_lod3->SetTextureIndex(car1wheel->GetTextureIndex());

	car1frontlights = new Q3DObject(engine, "meshes/frontlights.3ds", L"textures/headlight map l.jpg");
	car1spoiler = new Q3DObject(engine, "meshes/spoiler.3ds", L"textures/white.jpg");
	car1exhaustl = new Q3DObject(engine, "meshes/exhaustl.3ds", L"textures/dkgrey.jpg");
	car1exhaustr = new Q3DObject(engine, "meshes/exhaustr.3ds", L"textures/dkgrey.jpg");
	car1floorrear = new Q3DObject(engine, "meshes/floorrear.3ds", L"textures/dkgrey.jpg");
	car1halogen = new Q3DObject(engine, "meshes/halogen.3ds", L"textures/halogen.jpg");
	car1taillightsout = new Q3DObject(engine, "meshes/taillightsout.3ds", L"textures/rearlight1.jpg");
	car1taillightsin = new Q3DObject(engine, "meshes/taillightsin.3ds", L"textures/rearlight2.jpg");
	car1 = new Vehicle(engine);

	car1->actor->setGlobalPosition(NxVec3(1080.2f, 96.42f, 708.7f));
	NxMat33 m;
	m.rotY(QMath::DegToRad(-100));
	//q.fromAngleAxisFast(120, NxVec3(0, 1, 0));
	car1->actor->setGlobalOrientation(m);

	car2body = new Q3DObject(engine, "meshes/skyline.3ds", L"textures/Skyline Body Green_rotation.jpg");
	car2 = new Vehicle(engine);
	//car2->actor->setGlobalPosition(NxVec3(1020.0f, 96.42f, 486.0f));
	car2->actor->setGlobalPosition(NxVec3(960.0f, 96.5f, 440.0f));
	car2->Frames[0]->Model->SetTextureIndex(car2body->GetTextureIndex());
	NxMat33 mat = (car2->actor->getGlobalOrientation());
	mat.rotY( -0.8 );
	//mat.rotX( 0 );
	//mat.rotZ( 0 );
	car2->actor->setGlobalOrientation( mat );

	//car3body = new Q3DObject(engine, "meshes/skyline.3ds", L"textures/Skyline Body Green_rotation.jpg");
	//car3 = new Vehicle(engine);
	//car3->actor->setGlobalPosition(NxVec3(1020.0f, 102.5f, 486.0f));
	//car3->Frames[0]->Model->SetTextureIndex(car2body->GetTextureIndex());

	//initialize players
	players [0] = new Player(engine, car1);
	players [1] = new Player(engine, car2);

	//load level
	
	distance = 0.0;
	//level = new Level(engine, "heightmaps/map1r4.bmp");
	level = new Level(engine, "heightmaps/map1r8 ai_checkpoint.bmp");

	car2->TargetLocation = *targetsPositions[0];
	car2->numberOfTargetLocationFromGlobalLocations = 0;

	//DEBUG TEST
	//wchar_t txt [100];
	//swprintf_s(txt, 100, L"Wczytano drzewek: %.2f", (float)level->GetNObstacles());
	//MessageBoxW(0, txt, L"Drzewka", 0);

	LevelInitialized = true;

	GameLaunchedTime = engine->tman->GetTime();
	RaceStartedTime = 0;

	GameState = GAME_STATE_COUNTDOWN;

	//engine->graphics->SetCurrentCam((Camera*)engine->logic->car1->orbitCam);
	engine->graphics->SetCurrentCam(engine->graphics->GetFlyCam());
}

void Logic::AddEvent(Event* e)
{
	Events.push_back(e);
}

void Logic::HandleEvents()
{
	//call event handler function for every event in queue
	for(list<Event*>::iterator it = Events.begin(); it != Events.end(); it++)
	{
		(*it)->EventHandler();
		delete (*it);
	}
	Events.clear();
}

void Logic::Update()
{
	//game world update for each frame

	car1->Update();
	car1->ProcessSteering();

	car2->Update();

	MoveLinkedCamera();
	
	//update obstacles
	for (int i=0; i < level->nBoxes; i++)
		level->Boxes[i]->UpdatePoseFromActor();

	for (int i=0; i < level->nTrees; i++)
		level->Trees[i]->UpdatePoseFromActor();

	level->finishGate->UpdatePoseFromActor();

	//check game progress
	players[0]->CheckCheckpoints();

	CheckGameStatus();
}

void Logic::MoveLinkedCamera()
{
	if (CamLinkedWithPlayer)
	{
		NxVec3 Offset = car1->actor->getGlobalPosition() - car1->LastPosition;
		Camera* cam = engine->graphics->GetFlyCam();

		cam->Position.x += Offset.x;
		cam->Position.y += Offset.y;
		cam->Position.z += Offset.z;
	}

	//update last position
	car1->LastPosition = car1->actor->getGlobalPosition();

	//update ai
	AIUpdate(car2);
}

void Logic::CheckGameStatus()
{
	if(engine->tman->GetTime() - GameLaunchedTime < 3)
	{
		GameState = GAME_STATE_RUNNING;
		RaceStartedTime = engine->tman->GetTime();
	}
}

void Logic::AIUpdate(Vehicle* car)
{

	D3DXVECTOR2* wektorDoCelu2d = new D3DXVECTOR2();
	
	wektorDoCelu2d->x = car->TargetLocation.x - car->LastPosition.x;
	wektorDoCelu2d->y = car->TargetLocation.z - car->LastPosition.z;
	
	D3DXVec2Normalize(wektorDoCelu2d, wektorDoCelu2d);
	
	wektorPredkosci2d = new D3DXVECTOR2();
	wektorPredkosci2d->x = car->LastPosition.x - car->actor->getGlobalPosition().x;
	wektorPredkosci2d->y = car->LastPosition.z - car->actor->getGlobalPosition().z;
	
	wektorPredkosci2d->x = car->actor->getLinearVelocity().x;
	wektorPredkosci2d->y = car->actor->getLinearVelocity().z;

	D3DXVec2Normalize(wektorPredkosci2d, wektorPredkosci2d);
	
	D3DXVECTOR2* osZ = new D3DXVECTOR2();
	osZ->x = 0;
	osZ->y = -1;

	D3DXVECTOR2* osX = new D3DXVECTOR2();
	osX->x = -1;
	osX->y = 0;

	double iloczynSkalarnyCeluIOsiZ = ( wektorDoCelu2d->x * osZ->x + wektorDoCelu2d->y * osZ->y );
	double katOdOsiZDoCelu = acos( ( iloczynSkalarnyCeluIOsiZ/(1.0*1.0) ) );
	double iloczynSkalarnyPredkosciIOsiZ = ( wektorPredkosci2d->x * osZ->x 
		+ wektorPredkosci2d->y * osZ->y );
	double katOdWektoraPredkosciDoOsiZ = acos( ( iloczynSkalarnyPredkosciIOsiZ/(1.0*1.0) ) );
	
	double iloczynSkalarnyCeluIOsiX = ( wektorDoCelu2d->x * osX->x + wektorDoCelu2d->y * osX->y );
	double katOdOsiXDoCelu = acos( ( iloczynSkalarnyCeluIOsiX/(1.0*1.0) ) );
	double iloczynSkalarnyPredkosciIOsiX = ( wektorPredkosci2d->x * osX->x 
		+ wektorPredkosci2d->y * osX->y );
	double katOdWektoraPredkosciDoOsiX = acos( ( iloczynSkalarnyPredkosciIOsiX/(1.0*1.0) ) );
	
	double iloczynSkalarnyWektoraPredkosciIWektoraCelu = ( wektorDoCelu2d->x * wektorPredkosci2d->x 
		+ wektorDoCelu2d->y * wektorPredkosci2d->y );
	double katOKtoryNalezyObrocicSamochod = acos( ( iloczynSkalarnyWektoraPredkosciIWektoraCelu/(1.0*1.0) ) );
	NxReal katRealOdOsiXDoPojazdu = car->actor->getGlobalOrientationQuat().getAngle();
	
	double velocity = -500;

	//if( distance > 30 ){
	if( katOdOsiZDoCelu < katOdWektoraPredkosciDoOsiZ && (katRealOdOsiXDoPojazdu < 1.57 || katRealOdOsiXDoPojazdu >= 4.71) && katOdOsiXDoCelu < 1.57 && katOKtoryNalezyObrocicSamochod > 0.2){
			car->WheelTurningLimit = katOKtoryNalezyObrocicSamochod/4;
			car->WheelSteering = STEER_LEFT;
			car->UpdateWheelMatrices();
			car->ProcessSteering();
			velocity = -200;
			
	} 
	else if( katOdOsiZDoCelu >= katOdWektoraPredkosciDoOsiZ && (katRealOdOsiXDoPojazdu < 1.57 || katRealOdOsiXDoPojazdu >= 4.71) && katOdOsiXDoCelu < 1.57 && katOKtoryNalezyObrocicSamochod > 0.2){
			car->WheelTurningLimit = katOKtoryNalezyObrocicSamochod/4;
			//car->WheelSteering = STEER_LEFT; //ok
			car->WheelSteering = STEER_RIGHT; 
			car->UpdateWheelMatrices();
			car->ProcessSteering();
			velocity = -200;

	}
	//
	else if( katOdOsiZDoCelu >= katOdWektoraPredkosciDoOsiZ && (katRealOdOsiXDoPojazdu < 1.57 || katRealOdOsiXDoPojazdu >= 4.71) && katOdOsiXDoCelu >= 1.57 && katOKtoryNalezyObrocicSamochod > 0.2){
			car->WheelTurningLimit = katOKtoryNalezyObrocicSamochod/4;
			//car->WheelSteering = STEER_LEFT; //ok
			car->WheelSteering = STEER_RIGHT; 
			car->UpdateWheelMatrices();
			car->ProcessSteering();
			velocity = -200;

	}
	//
	else if( katOdOsiZDoCelu < katOdWektoraPredkosciDoOsiZ && (katRealOdOsiXDoPojazdu >= 1.57 || (katRealOdOsiXDoPojazdu >= 3.14 && katRealOdOsiXDoPojazdu <4.71)) && katOdOsiXDoCelu >= 1.57 && katOKtoryNalezyObrocicSamochod > 0.2 ){	
			car->WheelTurningLimit = katOKtoryNalezyObrocicSamochod/4;
			//car->WheelSteering = STEER_RIGHT;
			//car->WheelSteering = STEER_LEFT;
			car->WheelSteering = STEER_RIGHT;
			car->UpdateWheelMatrices();
			car->ProcessSteering();
			velocity = -200;

	} else if( katOdOsiZDoCelu >= katOdWektoraPredkosciDoOsiZ && (katRealOdOsiXDoPojazdu >= 1.57 || (katRealOdOsiXDoPojazdu >= 3.14 && katRealOdOsiXDoPojazdu <4.71)) && katOdOsiXDoCelu >= 1.57 && katOKtoryNalezyObrocicSamochod > 0.2 ){	
			car->WheelTurningLimit = katOKtoryNalezyObrocicSamochod/4;
			car->WheelSteering = STEER_LEFT;
			car->UpdateWheelMatrices();
			car->ProcessSteering();
			velocity = -200;
	} 
	//}

	car->SetTorqueRear( velocity );
	car->UpdateFrameMatrices();

	distance = sqrt( ( pow( ( car->LastPosition.x-car->TargetLocation.x ), 2) )
				   + ( pow( ( car->LastPosition.y-car->TargetLocation.y ), 2) )
				   + ( pow( ( car->LastPosition.z-car->TargetLocation.z ), 2) ) );
	
	if( katRealOdOsiXDoPojazdu > 0 ){
	}
	
	if( distance < 25 ){
	

		if( (car->numberOfTargetLocationFromGlobalLocations + 1) >= engine->logic->numberOfTargets ){
			car->numberOfTargetLocationFromGlobalLocations = 0;
		} else {

			car->numberOfTargetLocationFromGlobalLocations = car->numberOfTargetLocationFromGlobalLocations  + 1;
		}
		
		car->TargetLocation = *targetsPositions[ car->numberOfTargetLocationFromGlobalLocations ];

		car->SetBrakes(100.0f);
	} else 
	{
	
		car->SetBrakes(0.0f);
	}

	car->LastPosition = car->actor->getGlobalPosition();

}
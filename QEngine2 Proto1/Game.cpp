#include "Game.h"
#include "InputProcessor.h"
#include "IQGraphics.h"
#include "TimeManager.h"
#include "Logic.h"
#include "Level.h"
#include "QPhysics.h"
#include "IQSound.h"
#include "Music.h"
#include "QVector3.h"
#include "QMath.h"
#include "ViewFrustum.h"
#include "OrbitCam.h"
#include "..\QGraphicsD3D\Camera.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

#pragma warning(disable : 4996) // disable deprecated warning 
#include <strsafe.h>
#pragma warning(default : 4996)

#include <D3D9.h> ///temp

Game::Game() : QEngine()
{
	bAnimate = false;
	bFreeLook = false;
	logic = NULL;
	State = GAME_STATE_RUNNING;
}

Game::~Game()
{
	if (logic != NULL)
		delete logic;

	//the rest is at GameDestroy()
	return;
}

void Game::CheckInput()
{
	input->CheckKeys();
}

void Game::GameProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_MOUSEMOVE:
		input->ProcessMouse(message, wParam, lParam);
		break;

	case WM_LBUTTONDOWN:
		input->ProcessMouse(message, wParam, lParam);
		break;
	}
}

void Game::WorldUpdate()
{
	if (bPaused || bMinimized /*|| graphics->GetDeviceLost()*/) return;

	viewfrustum->Update();
	physics->Update();	//step physics simulation
	physics->FetchResults();
	logic->Update();
	logic->HandleEvents();
}

void Game::RenderScene()
{
	graphics->UpdateCamera();

	graphics->Clear(true, true, false);
	graphics->BeginRendering();

		graphics->SetTechnique("vpshader");

		logic->GetLevel()->Draw();
		RenderObstacles();
		RenderCreatures();
		graphics->ShowMessages();
		PrintCustomMessages();
		PrintMajorMessages();

		//misc, debug
		//logic->aux_box->Draw();

	graphics->EndRendering();

	graphics->Display();
}


void Game::RenderCreatures()
{
	logic->car1->Draw();
	logic->car2->Draw();
}

void Game::RenderObstacles()
{
	//for each obstacle
	for (int i=0; i < logic->GetLevel()->GetNObstacles() ; i++)
	{
		graphics->DrawObject((Q3DObject*)logic->GetLevel()->ObstacleArray[i]);
	}

	//for each box
	for (int i=0; i < logic->GetLevel()->nBoxes; i++)
		logic->GetLevel()->Boxes[i]->Draw();

	//for each tree
	for (int i=0; i < logic->GetLevel()->nTrees; i++)
		logic->GetLevel()->Trees[i]->Draw();

	//finish gate
	logic->GetLevel()->finishGate->Draw();
}

void Game::PrintCustomMessages()
{
	//camera position

	//drawing rectangle
	RECT rc;
	SetRect(&rc, GetBufferWidth()-520, 50, GetBufferWidth()-10, 90);

	//text buffer
	wchar_t txtCamPos [100];
	swprintf_s(txtCamPos, 100, L"Camera position: %.2f, %.2f, %.2f\nCamera orientation: %.2f, %.2f, %.2f", 
		graphics->GetCurrentCam()->Position.x, graphics->GetCurrentCam()->Position.y,
		graphics->GetCurrentCam()->Position.z, graphics->GetCurrentCam()->Rotation.x,
		graphics->GetCurrentCam()->Rotation.y, graphics->GetCurrentCam()->Rotation.z);

	//printing
	graphics->PrintText(txtCamPos, &rc, DT_RIGHT, 0, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	//vehicle cam rotate
	SetRect(&rc, 10, 50, 430, 90);
	swprintf_s(txtCamPos, 100, L"Orbit rotation: %.2f", QMath::RadToDeg(logic->car1->orbitCam->orbitRotation));
	graphics->PrintText(txtCamPos, &rc, DT_LEFT, 0, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	SetRect(&rc, 10, 80, 430, 120);
	float car_angle;
	NxQuat carq = logic->car1->actor->getGlobalOrientationQuat();
	carq.getAngleAxis(car_angle, NxVec3(0, 1, 0));
	swprintf_s(txtCamPos, 100, L"Car rotation: %.2f", car_angle);
	graphics->PrintText(txtCamPos, &rc, DT_LEFT, 0, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	//vehicle position
	SetRect(&rc, GetBufferWidth()-520, 120, GetBufferWidth()-10, 160);
	NxVec3 vecpos = logic->car1->actor->getGlobalPosition();
	swprintf_s(txtCamPos, 100, L"Car position: %.2f, %.2f, %.2f", 
		vecpos.x, vecpos.y, vecpos.z);
	graphics->PrintText(txtCamPos, &rc, DT_RIGHT, 0, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	//steering angle
	//SetRect(&rc, 10, 120, 430, 160);
	////swprintf_s(txtCamPos, 100, L"Car steering angle: %.4f", QMath::RadToDeg(logic->car1->GetSteerAngle()));
	//float freq;
	//sound->GetChannelFrequency(logic->car1->EngineChannel, &freq);
	//swprintf_s(txtCamPos, 100, L"Engine frequency: %.4f", freq);

	SetRect(&rc, 10, 120, 430, 160);
	swprintf_s(txtCamPos, 100, L"Rendered patches: %d", logic->GetLevel()->RenderedPatches);
	graphics->PrintText(txtCamPos, &rc, DT_LEFT, 0, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	SetRect(&rc, 10, 150, 430, 190);
	swprintf_s(txtCamPos, 100, L"Tested nodes: %d", logic->GetLevel()->NodesTested);
	graphics->PrintText(txtCamPos, &rc, DT_LEFT, 0, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	SetRect(&rc, 10, 180, 430, 220);
	swprintf_s(txtCamPos, 100, L"Car 1 LOD level: %d", logic->car1->LODLevel);
	graphics->PrintText(txtCamPos, &rc, DT_LEFT, 0, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	SetRect(&rc, 10, 230, 430, 270);
	swprintf_s(txtCamPos, 100, L"Current checkpoint: %d", logic->players[0]->CurrentCheckpoint);
	graphics->PrintText(txtCamPos, &rc, DT_LEFT, 0, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	//SetRect(&rc, 10, 230, 430, 270);
	//float t = tman->GetTime() - logic->RaceStartedTime;
	//swprintf_s(txtCamPos, 100, L"Time from race start: %.2f", t);
	//graphics->PrintText(txtCamPos, &rc, DT_LEFT, 0, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	SetRect(&rc, 10, 260, 430, 320);
	swprintf_s(txtCamPos, 100, L"Laps finished: %d", logic->players[0]->LapsFinished);
	graphics->PrintText(txtCamPos, &rc, DT_LEFT, 0, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	//lower corner of the screen:
	SetRect(&rc, 10, GetBufferHeight()-40, 430, GetBufferHeight()-10);
	swprintf_s(txtCamPos, 100, L"Lap time: %d", logic->players[0]->LapTime);
	graphics->PrintText(txtCamPos, &rc, DT_LEFT, 1, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	SetRect(&rc, 10, GetBufferHeight()-80, 430, GetBufferHeight()-50);
	swprintf_s(txtCamPos, 100, L"Best lap: %d", logic->players[0]->BestLap);
	graphics->PrintText(txtCamPos, &rc, DT_LEFT, 1, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	SetRect(&rc, 10, GetBufferHeight()-120, 430, GetBufferHeight()-90);
	swprintf_s(txtCamPos, 100, L"Lap: %d/3", logic->players[0]->LapsFinished+1);
	graphics->PrintText(txtCamPos, &rc, DT_LEFT, 1, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
}

void Game::PrintMajorMessages()
{
	float TimeFromStart = tman->GetTime() - logic->GameLaunchedTime;
	float TimeFromBestLap = tman->GetTime() - logic->players[0]->LapStartedTime;

	SetRect(&rc, GetBufferWidth()/2 - 220, GetBufferHeight()/2-50, GetBufferWidth()/2 + 220, GetBufferHeight()/2+50);

	if(TimeFromStart < 3.0f)
		graphics->PrintText(L"3", &rc, DT_CENTER, 2, D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));

	else if	(TimeFromStart < 4.0f)
		graphics->PrintText(L"2", &rc, DT_CENTER, 2, D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));


	else if(TimeFromStart < 5.0f)
		graphics->PrintText(L"1", &rc, DT_CENTER, 2, D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));

	else if(TimeFromStart < 16.0f)
		graphics->PrintText(L"START!", &rc, DT_CENTER, 2, D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));

	if (logic->GameState == GAME_STATE_OVER)
		graphics->PrintText(L"RACE OVER", &rc, DT_CENTER, 2, D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));

	else if((logic->players[0]->bLastLapBest) && TimeFromBestLap < 10)
		graphics->PrintText(L"BEST LAP!", &rc, DT_CENTER, 2, D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));


}

void Game::GameInit()
{
	//init game subsystems
	input = new InputProcessor(this);
	logic = new Logic(this);

	//setting initial current camera control
	//graphics->SetCurrentCam(graphics->GetFlyCam());

	InitGlobalLights();
	logic->Init();

	music->Load("sounds/10-orbital-tunnel_vision-mvp.mp3");
	//music->Play();
}

void Game::GameDestroy()
{

}

bool Game::GetAnimate()
{
	return bAnimate;
}

void Game::SetAnimate(bool bAnim)
{
	bAnimate = bAnim;
	if (bAnim)
		tman->SetStartTime(tman->GetTime() + 0.75f);
}

void Game::InitGlobalLights()
{
	D3DLIGHT9 l1;
	memset(&l1, 0, sizeof(l1));
	l1.Position = D3DXVECTOR3(300.0f, 300.0f, 300.0f);
	l1.Diffuse.r = 255;
	l1.Diffuse.g = 255;
	l1.Diffuse.b = 255;
	l1.Diffuse.a = 0;
	l1.Specular.r = 0;
	l1.Specular.g = 0;
	l1.Specular.b = 255;
	l1.Specular.a = 0;
	l1.Ambient.r = 0;
	l1.Ambient.g = 0;
	l1.Ambient.b = 0;
	l1.Ambient.a = 0;
	l1.Type = D3DLIGHT_DIRECTIONAL;
	l1.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	graphics->SetLight(&l1, 0);
	graphics->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void Game::SoundUpdate()
{
	//update listener parameters
	D3DXVECTOR3 pos = graphics->GetCurrentCam()->Position;
	D3DXVECTOR3 fwd = graphics->GetCurrentCam()->CalculateTarget();
	QVector3 qpos = QVector3(pos.x, pos.y, pos.z);
	QVector3 qfwd = QVector3(fwd.x, fwd.y, fwd.z);
	QVector3 qup = QVector3(0, 1, 0);
	QVector3 qvel = QVector3(0, 0, 0);

	sound->UpdateListener(0, qpos, qvel, qfwd, qup);

	//soundsystem internal update
	sound->Update();
}
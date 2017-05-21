#define NOMINMAX

#include "InputProcessor.h"
#include "TimeManager.h"
#include "Game.h"
#include "IQGraphics.h"
#include "Logic.h"
#include "QVObject.h"
#include "QPhysicalObject.h"
#include "NxPhysics.h"
#include "Frame.h"
#include "OrbitCam.h"
#include "Music.h"
#include "..\QGraphicsD3D\Camera.h"
#include <Windows.h>

InputProcessor::InputProcessor(Game *eng) : IInput()
{
	engine = eng;
	LastMouseX = 0;
	LastMouseY = 0;
	MouseSensitivity = 0.23f;
}

void InputProcessor::CheckKeys()
{
	if(aKeys[VK_ESCAPE])
		SendMessage(engine->GetHwnd(), WM_CLOSE, 0, 0);

	if(aKeys['F'] && !aKeyPressed['F'])
	{	
		aKeyPressed['F'] = true;
		if(engine->GetFreeLook()) 
		{	
			engine->SetFreeLook(false);
			engine->MsgX(L"Mouse freelook disabled.", 0.04f, 10.0f);
		}
		else 
		{
			engine->SetFreeLook(true);
			engine->MsgX(L"Mouse freelook enabled.", 0.04f, 10.0f);
		}
	}
	if(!aKeys['F']) aKeyPressed['F'] = false;

	//CAMERA MOVEMENT

	//camera mode
	if(aKeys['C'] && !aKeyPressed['C'])
	{	
		aKeyPressed['C'] = true;
		if(engine->graphics->GetFlyCam()->Speed == 1.0) 
		{	
			engine->graphics->GetFlyCam()->Speed = 4.0;
			engine->MsgX(L"Camera speed 4.", 0.04f, 10.0f);
		}
		else 
		{
			engine->graphics->GetFlyCam()->Speed = 1.0;
			engine->MsgX(L"Camera speed 1.", 0.04f, 10.0f);
		}
	}
	if(!aKeys['C']) aKeyPressed['C'] = false;

	if(aKeys['V'] && !aKeyPressed['V'])
	{	
		aKeyPressed['V'] = true;
		if(engine->logic->CamLinkedWithPlayer) 
		{	
			engine->logic->CamLinkedWithPlayer = false;
			engine->MsgX(L"Fly cam unlinked with car.", 0.04f, 10.0f);
		}
		else 
		{
			engine->logic->CamLinkedWithPlayer = true;
			engine->MsgX(L"Fly cam linked with car.", 0.04f, 10.0f);
		}
	}
	if(!aKeys['V']) aKeyPressed['V'] = false;

	//camera switching
	if(aKeys['B'] && !aKeyPressed['B'])
	{	
		aKeyPressed['B'] = true;
		if(engine->graphics->GetCurrentCam() == engine->graphics->GetFlyCam()) 
		{	
			engine->graphics->SetCurrentCam((Camera*)engine->logic->car1->orbitCam);
			engine->MsgX(L"Camera switched to car.", 0.04f, 10.0f);
		}
		else 
		{
			engine->graphics->SetCurrentCam(engine->graphics->GetFlyCam());
			engine->MsgX(L"Camera switched to fly cam.", 0.04f, 10.0f);
		}
	}
	if(!aKeys['B']) aKeyPressed['B'] = false;

	if(aKeys['W']) engine->graphics->GetCurrentCam()->MoveFB(5.0f*engine->graphics->GetFlyCam()->Speed);
	if(aKeys['S']) engine->graphics->GetCurrentCam()->MoveFB(-5.0f*engine->graphics->GetFlyCam()->Speed);
	if(aKeys['A']) engine->graphics->GetCurrentCam()->MoveLR(-5.0f*engine->graphics->GetFlyCam()->Speed);
	if(aKeys['D']) engine->graphics->GetCurrentCam()->MoveLR(5.0f*engine->graphics->GetFlyCam()->Speed);

	//CAR MOVEMENT

	//acceleration
	if(aKeys[VK_UP] && !aKeys[VK_SHIFT]) engine->logic->car1->SetTorqueRear(-1500.0f);
	else if(aKeys[VK_UP] && aKeys[VK_SHIFT]) engine->logic->car1->SetTorqueRear(500.0f);
	else engine->logic->car1->SetTorqueRear(0.0f);
	
	//braking
	if(aKeys[VK_DOWN]) engine->logic->car1->SetBrakes(1000.0f);
	else engine->logic->car1->SetBrakes(0.0f);

	//turning
	if(aKeys[VK_RIGHT] && aKeys[VK_LEFT]) engine->logic->car1->WheelSteering = STEER_BOTH;	//both are pressed
	if(aKeys[VK_RIGHT] && !aKeys[VK_LEFT]) engine->logic->car1->WheelSteering = STEER_RIGHT;	//right
	if(!aKeys[VK_RIGHT] && aKeys[VK_LEFT]) engine->logic->car1->WheelSteering = STEER_LEFT;		//left
	if(!aKeys[VK_RIGHT] && !aKeys[VK_LEFT]) engine->logic->car1->WheelSteering = STEER_NONE;

	if(aKeys['R'] && !aKeyPressed['R'])
	{	
		aKeyPressed['R'] = true;
		engine->logic->car1->ResetPosition();
		engine->MsgX(L"Position reset.", 0.04f, 10.0f);

	}
	if(!aKeys['R']) aKeyPressed['R'] = false;

	////////////music test
	if(aKeys['O'] && !aKeyPressed['O'])
	{	
		aKeyPressed['O'] = true;
		engine->music->SwitchPaused();
		engine->MsgX(L"Pause switched.", 0.04f, 10.0f);
	}
	if(!aKeys['O']) aKeyPressed['O'] = false;

	if(aKeys['I'] && !aKeyPressed['I'])
	{	
		aKeyPressed['I'] = true;
		engine->music->Stop();
		engine->MsgX(L"Music stopped.", 0.04f, 10.0f);
	}
	if(!aKeys['I']) aKeyPressed['I'] = false;

	if(aKeys['U'] && !aKeyPressed['U'])
	{	
		aKeyPressed['U'] = true;
		engine->music->Play();
		engine->MsgX(L"Music started.", 0.04f, 10.0f);
	}
	if(!aKeys['U']) aKeyPressed['U'] = false;

	if(aKeys['1'] && !aKeyPressed['1'])
	{	
		aKeyPressed['1'] = true;
		engine->music->Load("sounds\\10-orbital-tunnel_vision-mvp.mp3");
		engine->MsgX(L"Loaded track: Tunnel Vision", 0.04f, 10.0f);
	}
	if(!aKeys['1']) aKeyPressed['1'] = false;

	if(aKeys['2'] && !aKeyPressed['2'])
	{	
		aKeyPressed['2'] = true;
		engine->music->Load("sounds\\09-orbital-what_happens_next-mvp.mp3");
		engine->MsgX(L"Loaded track: What Happens Next", 0.04f, 10.0f);
	}
	if(!aKeys['2']) aKeyPressed['2'] = false;

	if(aKeys['3'] && !aKeyPressed['3'])
	{	
		aKeyPressed['3'] = true;
		engine->music->Load("sounds\\breakenter.mp3");
		engine->MsgX(L"Loaded track: Break & Enter", 0.04f, 10.0f);
	}
	if(!aKeys['3']) aKeyPressed['3'] = false;

	if(aKeys['4'] && !aKeyPressed['4'])
	{	
		aKeyPressed['4'] = true;
		engine->music->Load("sounds\\01-orbital-transient-mvp.mp3");
		engine->MsgX(L"Loaded track: Transient", 0.04f, 10.0f);
	}
	if(!aKeys['4']) aKeyPressed['4'] = false;

	if(aKeys['5'] && !aKeyPressed['5'])
	{	
		aKeyPressed['5'] = true;
		engine->music->Load("sounds\\wakeupcall.mp3");
		engine->MsgX(L"Loaded track: Wake Up Call", 0.04f, 10.0f);
	}
	if(!aKeys['5']) aKeyPressed['5'] = false;

	if(aKeys['6'] && !aKeyPressed['6'])
	{	
		aKeyPressed['6'] = true;
		engine->music->Load("sounds\\Wu-Tang Clan - 06 - Da Mystery Of Chessboxin'.mp3");
		engine->MsgX(L"Loaded track: a Mystery Of Chessboxin'", 0.04f, 10.0f);
	}
	if(!aKeys['6']) aKeyPressed['6'] = false;

	if(aKeys['9']) engine->music->SetVolume(engine->music->GetVolume()-0.01f);
	if(aKeys['0']) engine->music->SetVolume(engine->music->GetVolume()+0.01f);
	/////////////////////

	//DEBUG
	if(aKeys['N']) engine->logic->car1->orbitCam->orbitRotation+=(1.0f * engine->tman->GetDeltaTime());
	if(aKeys['M']) engine->logic->car1->orbitCam->orbitRotation-=(1.0f * engine->tman->GetDeltaTime());
	////////////////////////////

	if(aKeys['P'])
	{
		int pulapka = 1;
	}

	/////Debug!
	if(aKeys['T'] && !aKeyPressed['T'])
	{	
		aKeyPressed['T'] = true;
		engine->MsgX(L"Default debug message.", 0.04f, 0.0f);
	}
	if(!aKeys['T']) aKeyPressed['T'] = false;
}

void InputProcessor::ProcessMouse(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_MOUSEMOVE:
		FLOAT Xmove;
		FLOAT Ymove;
		Xmove = LOWORD(lParam) - LastMouseX;
		Ymove = HIWORD(lParam) - LastMouseY;
		if((wParam & MK_RBUTTON) || engine->GetFreeLook())
		{
			engine->graphics->GetCurrentCam()->Rotation.x -= Xmove * MouseSensitivity * engine->tman->GetDeltaTime();
			engine->graphics->GetCurrentCam()->Rotation.y += Ymove * MouseSensitivity * engine->tman->GetDeltaTime();
			engine->graphics->GetCurrentCam()->RotationCorrection();
			engine->graphics->GetCurrentCam()->CalculateTarget();
		}
		LastMouseX = LOWORD(lParam);
		LastMouseY = HIWORD(lParam);

		//resetting cursor position to screen center
		if((LOWORD(lParam)<=1) || (LOWORD(lParam) >= engine->GetBufferWidth()-1) ||
			(HIWORD(lParam)<=1) || (HIWORD(lParam) >= engine->GetBufferHeight()-1))
		{
			SetCursorPos(engine->GetBufferWidth()/2, engine->GetBufferHeight()/2);
			LastMouseX = (float)engine->GetBufferWidth()/2;
			LastMouseY = (float)engine->GetBufferHeight()/2;
		}
		break;

	case WM_LBUTTONDOWN:

		//TODO add handler here

		break;

	}
}
#include "Camera.h"
#include "..\QEngine2 Proto1\Game.h"
#include "..\QEngine2 Proto1\TimeManager.h"
#include <cmath>

Camera::Camera()
{
	bFlyCam = true;
	Up.x = 0;
	Up.y = 1;
	Up.z = 0;
	engine = NULL;
	Speed = 4.0;
}

Camera::Camera(Game* eng)
{
	bFlyCam = true;
	Up.x = 0;
	Up.y = 1;
	Up.z = 0;
	engine = eng;
	Speed = 4.0;
}

void Camera::MoveFB(FLOAT dist)
{
	dist *= engine->tman->GetDeltaTime();	//to make motion speed independent from FPS
	dist *= Speed;	//flycam speed correction
	Position.x += dist * Target.x;
	Position.y += dist * Target.y;
	Position.z += dist * Target.z;
}

void Camera::MoveLR(FLOAT dist)
{
	dist *= engine->tman->GetDeltaTime();
	dist *= Speed;
	D3DXVECTOR3 Right;
	D3DXVec3Cross(&Right, &Up, &Target);
	Position.x += dist * Right.x;
	Position.y += dist * Right.y;
	Position.z += dist * Right.z;
}

void Camera::Setup(D3DXVECTOR3 CamPos, FLOAT RX, FLOAT RY)
{

}

void Camera::UpdateView()
{

}
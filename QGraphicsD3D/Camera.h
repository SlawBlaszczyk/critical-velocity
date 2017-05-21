#ifndef CAMERA_H
#define CAMERA_H

#include "..\QEngine2 Proto1\QVObject.h"

class Camera : public QVObject
{	
protected:
	bool bFlyCam;
	D3DXVECTOR3 Up;	//up versor

public:
	float Speed;

	Camera();
	Camera(Game*);
	void MoveFB(FLOAT dist);
	void MoveLR(FLOAT dist);
	void Setup(D3DXVECTOR3 CamPos, FLOAT RX, FLOAT RY);
	void UpdateView();
};

#endif
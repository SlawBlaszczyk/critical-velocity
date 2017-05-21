#ifndef CAMERA_H
#define CAMERA_H

#include "QVObject.h"

class Camera : public QVObject
{	
protected:
	bool bFlyCam;
	D3DXVECTOR3 Up;	//up versor

public:
	float Speed;

	Camera();
	Camera(Game*);
	virtual void MoveFB(FLOAT dist);
	virtual void MoveLR(FLOAT dist);
	virtual void Setup(D3DXVECTOR3 CamPos, FLOAT RX, FLOAT RY);
	virtual void UpdateView();
};

#endif
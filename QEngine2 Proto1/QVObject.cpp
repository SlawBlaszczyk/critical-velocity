#include "QVObject.h"

QVObject::QVObject()
{
	Position.x = 0;
	Position.y = 0;
	Position.z = 0;
	Rotation.x = 0;
	Rotation.y = 0;
	Rotation.z = 0;
	engine = NULL;
	D3DXMatrixIdentity(&TransformMatrix);
}

QVObject::QVObject(Game* eng)
{
	Position.x = 0;
	Position.y = 0;
	Position.z = 0;
	Rotation.x = 0;
	Rotation.y = 0;
	Rotation.z = 0;
	engine = eng;
	D3DXMatrixIdentity(&TransformMatrix);
}

//virtual function definitions
void QVObject::MoveFB(FLOAT dist) {}
void QVObject::MoveLR(FLOAT dist) {}

D3DXVECTOR3 QVObject::CalculateTarget()
{
	FLOAT CosY = cos(Rotation.y); //cosinus kata y (zeby nie liczyc 2x)
	FLOAT CosX = cos(Rotation.x);
	FLOAT SinX = sin(Rotation.x);
	Target.y = sin(-Rotation.y);
	Target.x = CosY * CosX;
	Target.z = CosY * SinX;
	return Target;	//unit vector
}

void QVObject::RotationCorrection()
{
	//camera rotation correction & vertical limit
	//if(Rotation.x >= 2*D3DX_PI)
	//	Rotation.x -= 2*D3DX_PI;
	//if(Rotation.x < 0)
	//	Rotation.x += 2*D3DX_PI;
	//if(Rotation.y >= 0.46*D3DX_PI)
	//	Rotation.y = (float)0.46*D3DX_PI;
	//if(Rotation.y <= -0.46*D3DX_PI)
	//	Rotation.y = (float)-0.46*D3DX_PI;
	//if(Rotation.z >= 2*D3DX_PI)
	//	Rotation.z -= 2*D3DX_PI;
	//if(Rotation.z < 0)
	//	Rotation.z += 2*D3DX_PI;

	if(Rotation.x >= 2*D3DX_PI)
		Rotation.x -= 2*D3DX_PI;
	if(Rotation.x < 0)
		Rotation.x += 2*D3DX_PI;
	if(Rotation.y >= 2*D3DX_PI)
		Rotation.y -= 2*D3DX_PI;
	if(Rotation.y < 0)
		Rotation.y += 2*D3DX_PI;
	if(Rotation.z >= 2*D3DX_PI)
		Rotation.z -= 2*D3DX_PI;
	if(Rotation.z < 0)
		Rotation.z += 2*D3DX_PI;
}
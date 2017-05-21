#ifndef QVOBJECT_H
#define QVOBJECT_H

#include <D3DX9.h>

class Game;

class QVObject
{
protected:
	Game* engine;

public:
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Rotation;	//in radians
	D3DXVECTOR3 Target;		//versor
	D3DXMATRIX TransformMatrix;

	virtual void MoveFB(FLOAT dist);
	virtual void MoveLR(FLOAT dist);
	virtual void RotationCorrection();

	//calculates target versor
	virtual D3DXVECTOR3 CalculateTarget();
	QVObject();
	QVObject(Game*);
};

#endif
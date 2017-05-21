#ifndef QMATH_H
#define QMATH_H

#include <D3DX9.h>

class QMath
{
public:
	static float DegToRad(float Deg);
	static float RadToDeg(float Rad);
	static float Abs(float in);
	static D3DXVECTOR3 LimitVector3(D3DXVECTOR3 vec, float limit);
};

#endif
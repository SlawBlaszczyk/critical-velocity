#include "QMath.h"
#include <D3DX9.h>

D3DXVECTOR3 QMath::LimitVector3(D3DXVECTOR3 vec, float limit)
{
	//getting vector length
	float lgt = D3DXVec3Length(&vec);

	//decrease vector value
		if (lgt > limit)
	{
		vec.x -= (lgt - limit) * (vec.x / lgt);
		vec.y -= (lgt - limit) * (vec.y / lgt);
		vec.z -= (lgt - limit) * (vec.z / lgt);
	}

	return vec;
}

float QMath::RadToDeg(float Rad)
{
	return (Rad * 180.0f / D3DX_PI);
}

float QMath::DegToRad (float Deg)
{
	return (Deg / 180.0f * D3DX_PI);
}

float QMath::Abs(float in)
{
	if (in >= 0) return in;
	else return in * (-1.0f);
}
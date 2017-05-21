#include "QVector3.h"
#include <cmath>

QVector3::QVector3()
{
	x = 0;
	y = 0;
	z = 0;
}

QVector3::QVector3(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

QVector3 QVector3::operator -(QVector3 vec2)
{
	return QVector3(x - vec2.x, y - vec2.y, z - vec2.z);
}

QVector3 QVector3::operator +=(QVector3 vec2)
{
	x += vec2.x;
	y += vec2.y;
	z += vec2.z;

	return *this;
}

double QVector3::Magnitude()
{
	return (sqrt(x*x + y*y + z*z));
}
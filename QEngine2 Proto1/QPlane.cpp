#include "QPlane.h"
#include "QVector3.h"
#include <cmath>

float QPlane::DistanceToPoint(QVector3 p, bool normalize)
{
	if(normalize) Normalize();	//if not normalized, the distance will be in units of vector normal magnitude
	return a * p.x + b * p.y + c * p.z + d;
}

void QPlane::Normalize()
{
	float mag; //magnitude of the normal vector a, b, c
	mag = sqrt(a*a + b*b + c*c);

	//divide the components by normal magnitude
	a /= mag;
	b /= mag;
	c /= mag;
	d /= mag;
}
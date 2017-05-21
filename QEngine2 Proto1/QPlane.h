#ifndef QPLANE_H
#define QPLANE_H

class QVector3;

class QPlane
{
public:
	float a, b, c, d;

	float DistanceToPoint(QVector3 p, bool normalize = true);
	void Normalize();
};

#endif
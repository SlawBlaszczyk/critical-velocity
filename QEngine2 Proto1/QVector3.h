#ifndef QVECTOR3_H
#define QVECTOR3_H

class QVector3
{
public:
	float x, y, z;
	
	QVector3(float _x, float _y, float _z);
	QVector3();

	QVector3 operator-(QVector3 vec2);
	QVector3 operator+=(QVector3 vec2);
	double Magnitude();
};

#endif
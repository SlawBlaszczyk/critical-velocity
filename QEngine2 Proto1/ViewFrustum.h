#ifndef VIEWFRUSTUM_H
#define VIEWFRUSTUM_H

#include <D3DX9.h>
#include "QPlane.h"

class QEngine;
//class QPlane;
class QVector3;

class ViewFrustum
{
	QEngine* engine;
	D3DXMATRIX ProjectionMatrix;
	D3DXMATRIX ViewMatrix;
	D3DXMATRIX ViewProjectionMatrix;

	//array of clipping planes
	QPlane ClippingPlanes [6];

public:
	float FOV;
	float AspectRatio;
	float zn;	//near clipping plane distance, in view space
	float zf;	//far clipping plane distance

	//constructor taking as arguments: engine class pointer, field of view, aspect ratio and 
	//distance from far and near clipping planes
	ViewFrustum(QEngine* eng, float fov_, float aspect, float near, float far);

	//updates clipping plane equations each frame
	void Update(bool normalize = true);

	//tests intersection with an AABB bounding box
	int TestAgainstAABB(QVector3 position, QVector3 sideHalves);

};

#endif
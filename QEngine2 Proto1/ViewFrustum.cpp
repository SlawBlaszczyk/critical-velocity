#include "ViewFrustum.h"
#include "../QGraphicsD3D/Camera.h"
#include "QEngine.h"
#include "QVector3.h"
#include "IQGraphics.h"

ViewFrustum::ViewFrustum(QEngine *eng, float fov_, float aspect, float zn_, float zf_)
{
	engine = eng;
	FOV = fov_;
	AspectRatio = aspect;
	zn = zn_;
	zf = zf_;

	//init projection matrix
	D3DXMatrixPerspectiveFovLH(&ProjectionMatrix, FOV, AspectRatio, zn, zf);
}

void ViewFrustum::Update(bool normalize)
{
	//update view matrix
	Camera* cam = engine->graphics->GetCurrentCam();
	cam->CalculateTarget();
	D3DXVECTOR3 TargetPos(cam->Position.x + cam->Target.x, cam->Position.y + cam->Target.y,
		cam->Position.z + cam->Target.z);
	D3DXVECTOR3 UpVector(0, 1, 0);

	D3DXMatrixLookAtLH(&ViewMatrix, &cam->Position, &TargetPos, &UpVector);
	
	//multiply matrices
	D3DXMatrixMultiply(&ViewProjectionMatrix, &ViewMatrix, &ProjectionMatrix);

	//calculate frustum clipping planes

	// Left clipping plane
	ClippingPlanes[0].a = ViewProjectionMatrix._14 + ViewProjectionMatrix._11;
	ClippingPlanes[0].b = ViewProjectionMatrix._24 + ViewProjectionMatrix._21;
	ClippingPlanes[0].c = ViewProjectionMatrix._34 + ViewProjectionMatrix._31;
	ClippingPlanes[0].d = ViewProjectionMatrix._44 + ViewProjectionMatrix._41;

	// Left clipping plane
	ClippingPlanes[1].a = ViewProjectionMatrix._14 - ViewProjectionMatrix._11;
	ClippingPlanes[1].b = ViewProjectionMatrix._24 - ViewProjectionMatrix._21;
	ClippingPlanes[1].c = ViewProjectionMatrix._34 - ViewProjectionMatrix._31;
	ClippingPlanes[1].d = ViewProjectionMatrix._44 - ViewProjectionMatrix._41;

	// Top clipping plane
	ClippingPlanes[2].a = ViewProjectionMatrix._14 - ViewProjectionMatrix._12;
	ClippingPlanes[2].b = ViewProjectionMatrix._24 - ViewProjectionMatrix._22;
	ClippingPlanes[2].c = ViewProjectionMatrix._34 - ViewProjectionMatrix._32;
	ClippingPlanes[2].d = ViewProjectionMatrix._44 - ViewProjectionMatrix._42;

	// Bottom clipping plane
	ClippingPlanes[3].a = ViewProjectionMatrix._14 + ViewProjectionMatrix._12;
	ClippingPlanes[3].b = ViewProjectionMatrix._24 + ViewProjectionMatrix._22;
	ClippingPlanes[3].c = ViewProjectionMatrix._34 + ViewProjectionMatrix._32;
	ClippingPlanes[3].d = ViewProjectionMatrix._44 + ViewProjectionMatrix._42;

	// Near clipping plane
	ClippingPlanes[4].a = ViewProjectionMatrix._13;
	ClippingPlanes[4].b = ViewProjectionMatrix._23;
	ClippingPlanes[4].c = ViewProjectionMatrix._33;
	ClippingPlanes[4].d = ViewProjectionMatrix._43;

	// Far clipping plane
	ClippingPlanes[5].a = ViewProjectionMatrix._14 - ViewProjectionMatrix._13;
	ClippingPlanes[5].b = ViewProjectionMatrix._24 - ViewProjectionMatrix._23;
	ClippingPlanes[5].c = ViewProjectionMatrix._34 - ViewProjectionMatrix._33;
	ClippingPlanes[5].d = ViewProjectionMatrix._44 - ViewProjectionMatrix._43;

	if(normalize)
		for(int i=0; i<6; i++)
			ClippingPlanes[i].Normalize();
}

int ViewFrustum::TestAgainstAABB(QVector3 position, QVector3 sideHalves)
{
	//calculate verices of the AABB
	QVector3 abverts [8];
	abverts [0] = QVector3(position.x + sideHalves.x, position.y + sideHalves.y, position.z + sideHalves.z);
	abverts [1] = QVector3(position.x + sideHalves.x, position.y + sideHalves.y, position.z - sideHalves.z);
	abverts [2] = QVector3(position.x + sideHalves.x, position.y - sideHalves.y, position.z + sideHalves.z);
	abverts [3] = QVector3(position.x + sideHalves.x, position.y - sideHalves.y, position.z - sideHalves.z);
	abverts [4] = QVector3(position.x - sideHalves.x, position.y + sideHalves.y, position.z + sideHalves.z);
	abverts [5] = QVector3(position.x - sideHalves.x, position.y + sideHalves.y, position.z - sideHalves.z);
	abverts [6] = QVector3(position.x - sideHalves.x, position.y - sideHalves.y, position.z + sideHalves.z);
	abverts [7] = QVector3(position.x - sideHalves.x, position.y - sideHalves.y, position.z - sideHalves.z);

	int NegativeVertices;	//amount of vertices of AABB on a negative side of a plane

	//for each plane test all vertices of AABB
	for (int i=0; i<6; i++)
	{
		NegativeVertices = 0;

		//if all vertices of an AABB lie on a negaive side of a plane, return -1;
		//for each vertex
		for (int j=0; j<8; j++)
		{
			if (ClippingPlanes[i].DistanceToPoint(abverts[j]) < 0) NegativeVertices++;
		}

		//if whole AABB is on the negative side of a plane, it is outta the frustum
		if (NegativeVertices == 8) return -1;
	}

	//all planes have some AABB verts on the positive side, result positive
	return 1;
}
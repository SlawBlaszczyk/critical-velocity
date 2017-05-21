#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <D3DX9.h>

#define NOMINMAX

#define VFORMAT VERTEXUV
#define FVF D3DFVF_VERTEXUV

#define MAXVERTICES 100000
#define MAXPOLY 100000

#define MAX_ADDITIONAL_TEXTURES 10 //sets maximum no of additional textures for object

#define TILE_SIDE 10.0f

#define CREATURE_STATE_PATROL 0
#define CREATURE_STATE_PURSUE 1
#define CREATURE_STATE_FLEE 2

//car part definitions
#define CAR_BODY 0
#define CAR_WHEEL_FR 1
#define CAR_WHEEL_FL 2
#define CAR_WHEEL_RL 3
#define CAR_WHEEL_RR 4
#define CAR_FRONTLIGHTS 5
#define CAR_SPOILER 6
#define CAR_EXHAUSTL 7
#define CAR_EXHAUSTR 8
#define CAR_FLOORREAR 9
#define CAR_HALOGEN 10
#define CAR_TAILLIGHTSOUT 11
#define CAR_TAILLIGHTSIN 12
#define CAR_FLOORMAIN 13


struct VERTEXUVCOLOR
{
	FLOAT x, y, z;
	DWORD color;
	FLOAT u, v;
};
#define D3DFVF_VERTEXUVCOLOR (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

struct VERTEXUV
{
	FLOAT x, y, z;
	FLOAT nx, ny, nz;
	FLOAT u, v, u2, v2;
};
#define D3DFVF_VERTEXUV (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2)

struct VERTEX
{
	FLOAT x, y, z;
	DWORD color;
};
#define D3DFVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

class Poly
{
public:
	unsigned short x, y, z;
};

class MsgXData
{
public:
	bool write;
	wchar_t TextBuffer [128];
	float WritingInitTime;
	float CharDelayTime;
	float WritingDuration;
};

class IntVectorXY
{
public:
	int x, y;
};

struct DirectRange4
{
	int left, right, up, down;
};

class Plane
{
public:
	D3DXVECTOR3 normal;
	FLOAT dist;
};

//class Light
//{
//public:
//
//
//};

#endif
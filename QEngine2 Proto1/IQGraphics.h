#ifndef IQGRAPHICS_H
#define IQGRAPHICS_H

#define NOMINMAX

#include "Structures.h"
#include <Windows.h>
#include <D3DX9.h>


class QEngine;
class Q3DObject;
class QVObject;
class Camera;

class IQGraphics
{
public:

	HINSTANCE hDLL;
	virtual HRESULT Init(QEngine* eng, HWND parent) = 0;

	//function displays a message in render window console
	virtual void MsgX(LPCTSTR txt, float delay, float duration) = 0;

	//function allocates vertex and index buffers in video memory
	//and returns internal mesh index to the app
	virtual int AllocateMesh(Q3DObject* object) = 0;

	//function allocates texture in video memory from the given filename
	//and returns internal texture index to the app
	virtual int AllocateTexture(LPCTSTR FileName) = 0;

	//functions release allocated video resources
	virtual void ReleaseMesh(int MeshIndex) = 0;
	virtual void ReleaseTexture(int TexIndex) = 0;

	virtual void Clear(bool Color, bool Depth, bool Stencil) = 0;
	virtual void SetClearColor(DWORD Color) = 0;
	virtual void DrawObject (int MeshIndex, int TextureIndex, D3DXVECTOR3 Position, D3DXVECTOR3 Rotation, int nVertices, int nPolys) = 0;
	virtual void DrawObject (int MeshIndex, int TextureIndex, D3DXMATRIX TransformMatrix, int nVertices, int nPolys) = 0;
	virtual void DrawObject (Q3DObject* object) = 0;
	virtual void BeginRendering() = 0;
	virtual void EndRendering() = 0;
	virtual void Display() = 0;
	virtual void SetTechnique(LPCSTR Technique) = 0;
	virtual void SetUpCamera(D3DXVECTOR3 Position, D3DXVECTOR3 Rotation, float FOV, float Aspect, float zn, float zf) = 0;
	virtual void UpdateCamera() = 0;

	//shows console messages
	virtual void ShowMessages() = 0;

	//prints arbitrary text on screen with default font
	virtual void PrintText(LPCTSTR pString, LPRECT pRect, DWORD Format, int FontIndex, D3DXVECTOR4 Color) = 0;
	virtual void DrawSprite(D3DXVECTOR3 center, D3DXVECTOR3 position, D3DXVECTOR3 scale, int TexIndex) = 0;

	//get current camera object
	virtual Camera* GetCurrentCam() = 0;
	virtual Camera* GetFlyCam() = 0;
	virtual void SetCurrentCam(Camera* cam) = 0;

	//set state
	virtual void SetRenderState(DWORD variable, DWORD value) = 0; 
	virtual void SetLight(D3DLIGHT9* lparams, int nr) = 0; ///TEMPORARY
	//set shader vars

};

extern "C"
{
	HRESULT CreateGraphicsSubsystem(HINSTANCE hDLL, IQGraphics** pInterface);
	typedef HRESULT (*CREATEGRAPHICSSUBSYSTEM) (HINSTANCE hDLL, IQGraphics** pInterface);

	HRESULT ReleaseGraphicsSubsystem(IQGraphics** pInterface);
	typedef HRESULT (*RELEASEGRAPHICSSUBSYSTEM) (IQGraphics** pInterface);
}

#endif
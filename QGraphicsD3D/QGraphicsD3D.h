#ifndef QGRAPHICSD3D_H
#define QGRAPHICSD3D_H

#define MAX_MESH_BUFFERS 2048
#define MAX_TEXTURES 2048

#include "..\QEngine2 Proto1\IQGraphics.h"
#include "..\QEngine2 Proto1\Structures.h"
#include "..\QEngine2 Proto1\Q3DObject.h"
#include "Camera.h"
#include <D3D9.h>
#include <D3DX9.h>

class QGraphicsD3D : public IQGraphics
{
protected:
	QEngine* engine;
	Camera* FlyCam;
	Camera* CurrentCam;
	HWND hWnd;
	HRESULT hResult;
	LPDIRECT3D9 pD3D9;
	LPDIRECT3DDEVICE9 pDevice;
	LPD3DXBUFFER errorBuffer;
	LPD3DXEFFECT pEffect;
	LPD3DXFONT pFont [32];
	LPD3DXSPRITE pSprite;

	LPDIRECT3DVERTEXBUFFER9 VertexBuffers [MAX_MESH_BUFFERS];
	LPDIRECT3DINDEXBUFFER9 IndexBuffers [MAX_MESH_BUFFERS];
	LPDIRECT3DTEXTURE9 Textures [MAX_TEXTURES];

	D3DXMATRIX RotationMatrix;
	D3DXMATRIX TranslationMatrix;
	D3DXMATRIX ScaleMatrix;
	D3DXMATRIX WorldMatrix;
	D3DXMATRIX ProjectionMatrix;
	D3DXMATRIXA16 ViewMatrix;
	D3DXMATRIXA16 WorldViewProjectionMatrix;

	D3DCOLOR ClearColor;
	MsgXData msgXData;	//on-screen messages buffer

	void GeneratePresentParameters(D3DPRESENT_PARAMETERS* pp);
	HRESULT CreateFont(LPD3DXFONT*, LPCTSTR, int);

public:
	QGraphicsD3D (HINSTANCE hDLL_);
	~QGraphicsD3D();

	//config window message handler procedure
	BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

	//interface functions:
	HRESULT Init(QEngine* eng, HWND parent);

	//function displays a message in render window console
	void MsgX(LPCTSTR txt, float delay, float duration);

	//function allocates vertex and index buffers in video memory
	//and returns internal mesh index to the app
	int AllocateMesh(Q3DObject* object);

	//function allocates texture in video memory from the given filename
	//and returns internal texture index to the app
	int AllocateTexture(LPCTSTR FileName);

	//functions release allocated video resources
	void ReleaseMesh(int MeshIndex);
	void ReleaseTexture(int TexIndex);

	void Clear(bool Color, bool Depth, bool Stencil);
	void SetClearColor(DWORD Color);
	void DrawObject (int MeshIndex, int TextureIndex, D3DXVECTOR3 Position, D3DXVECTOR3 Rotation, int nVertices, int nPolys);
	void DrawObject (int MeshIndex, int TextureIndex, D3DXMATRIX TransformMatrix, int nVertices, int nPolys);
	void DrawObject (Q3DObject* object);
	void BeginRendering();
	void EndRendering();
	void Display();
	void SetTechnique(LPCSTR Technique);
	void SetUpCamera(D3DXVECTOR3 Position, D3DXVECTOR3 Rotation, float FOV, float Aspect, float zn, float zf);
	void UpdateCamera();
	void ShowMessages();
	void PrintText(LPCTSTR pString, LPRECT pRect, DWORD Format, int FontIndex, D3DXVECTOR4 Color);
	void DrawSprite(D3DXVECTOR3 center, D3DXVECTOR3 position, D3DXVECTOR3 scale, int TexIndex);

	Camera* GetCurrentCam() {return CurrentCam;}
	Camera* GetFlyCam() {return FlyCam;}
	void SetCurrentCam(Camera* cam) {CurrentCam = cam;}

	//set state
	void SetRenderState(DWORD variable, DWORD value); 
	void SetLight(D3DLIGHT9* lparams, int nr);
	//set shader vars

};

#endif
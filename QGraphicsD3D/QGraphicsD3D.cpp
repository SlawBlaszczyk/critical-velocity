#define NOMINMAX

#include <Windows.h>
#include "QGraphicsD3D.h"
#include "..\QEngine2 Proto1\QEngine.h"
#include "..\QEngine2 Proto1\TimeManager.h"

#pragma warning(disable : 4996) // disable deprecated warning 
#include <strsafe.h>
#pragma warning(default : 4996) 

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

//an exported function that creates an instance of the subsystem 
//and returns a pInterface pointer of the subsystem to the main appliccation
HRESULT CreateGraphicsSubsystem(HINSTANCE hDLL, IQGraphics** pInterface)
{
	if (!*pInterface)
	{
		*pInterface = new QGraphicsD3D(hDLL);
		return S_OK;
	}
	return E_FAIL;
}

//an exported function that releases the graphics subsystem
HRESULT ReleaseGraphicsSubsystem(IQGraphics** pInterface)
{
	if (!*pInterface)
	{
		return E_FAIL;
	}
	delete *pInterface;
	*pInterface = NULL;
	return S_OK;
}

QGraphicsD3D::QGraphicsD3D(HINSTANCE hDLL_)
{
	hDLL = hDLL_;
	pD3D9 = NULL;
	pDevice = NULL;

	for(int i=0; i<32; i++)
		pFont [i] = NULL;

	pEffect = NULL;
	pSprite = NULL;

	for(int i=0; i<MAX_MESH_BUFFERS; i++)
		VertexBuffers [i] = NULL;
	for(int i=0; i<MAX_MESH_BUFFERS; i++)
		IndexBuffers [i] = NULL;
	for(int i=0; i<MAX_TEXTURES; i++)
		Textures [i] = NULL;

	ClearColor = 0x00000000;

	FlyCam = NULL;
	CurrentCam = NULL;
}

QGraphicsD3D::~QGraphicsD3D()
{
	if(FlyCam != NULL)
		delete FlyCam;

	for(int i=0; i<MAX_MESH_BUFFERS; i++)
		if(VertexBuffers [i] != NULL)
			VertexBuffers [i]->Release();
	for(int i=0; i<MAX_MESH_BUFFERS; i++)
		if(IndexBuffers [i] != NULL)
			IndexBuffers [i]->Release();
	for(int i=0; i<MAX_TEXTURES; i++)
		if(Textures [i] != NULL)
			Textures [i]->Release();
	for(int i=0; i<32; i++)
		if(pFont [i] != NULL)
			pFont [i]->Release();

	if(pEffect != NULL)
		pEffect->Release();
	if(pSprite != NULL)
		pSprite->Release();

	if(pDevice != NULL)
		pDevice->Release();
	if(pD3D9 != NULL)
		pD3D9->Release();
}

HRESULT QGraphicsD3D::Init(QEngine* eng, HWND parent)
{
	engine = eng;
	hWnd = parent;
	//MessageBox(parent, L"Dziala", L"Message", MB_OK);

	//creating D3D object
	pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if(pD3D9 == NULL) return 0;

	//creating device
	D3DPRESENT_PARAMETERS pp = {0};
	GeneratePresentParameters(&pp);

	hResult = D3D_OK;
	hResult = pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &pDevice);
	if (FAILED(hResult))
	{
		MessageBox(NULL, L"Hardware vertex processing not available, using software mode.",
			L"Vertex Processing Failure", MB_OK);
		hResult = pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &pDevice);
	}
	if (FAILED(hResult)) engine->Decease(L"D3DInit failed.");
	
	//creating fonts
	hResult = CreateFont(&pFont[0], L"Bank Gothic Medium BT", 20);
	if (FAILED(hResult)) engine->Decease(L"Creating font failed.");

	hResult = CreateFont(&pFont[1], L"Neuropol", 30);
	if (FAILED(hResult)) engine->Decease(L"Creating font failed.");

	hResult = CreateFont(&pFont[2], L"Paint it black", 80);
	if (FAILED(hResult)) engine->Decease(L"Creating font failed.");

	//creating sprite
	D3DXCreateSprite(pDevice, &pSprite);

	//creating shader effect
	hResult = D3DXCreateEffectFromFile(pDevice, L"shader3.fx", 0, 0, D3DXSHADER_DEBUG, 0, &pEffect, &errorBuffer);
	//if(errorBuffer) engine->Decease((LPCTSTR)errorBuffer->GetBufferPointer());
	if(errorBuffer) MessageBoxA(0, (LPCSTR)errorBuffer->GetBufferPointer(), "Shader creation error", MB_OK);
	if(FAILED(hResult)) engine->Decease(L"Failed creating effect.");

	//creating camera object
	FlyCam = new Camera((Game*)engine);
	CurrentCam = FlyCam;///

	//set initial render state
	//SetClearColor(0x00150431);
	SetClearColor(0x00000000);
	pDevice->SetFVF(FVF);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	//pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 2);
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	return hResult;
}

void QGraphicsD3D::GeneratePresentParameters(D3DPRESENT_PARAMETERS *pp)
{
	D3DDISPLAYMODE disp = {0};
	pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &disp);

	pp->BackBufferWidth = engine->GetBufferWidth();
	pp->BackBufferHeight = engine->GetBufferHeight();
	pp->BackBufferFormat = disp.Format;
	pp->BackBufferCount = 1;
	pp->MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
	pp->MultiSampleQuality = 0;
	pp->SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp->hDeviceWindow = hWnd;
	pp->Windowed = !(engine->GetFullScreen());
	pp->EnableAutoDepthStencil = TRUE;
	pp->AutoDepthStencilFormat = D3DFMT_D16;
	pp->Flags = 0;
	pp->FullScreen_RefreshRateInHz = (engine->GetFullScreen() ? disp.RefreshRate : 0);
	pp->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
}

HRESULT QGraphicsD3D::CreateFont(LPD3DXFONT* ppFont, LPCTSTR faceName, int nHeight)
{
	hResult = D3DXCreateFont( pDevice,			    // D3D device
							 nHeight,				// Height
							 0,                     // Width
							 FW_BOLD,               // Weight
							 1,                     // MipLevels, 0 = autogen mipmaps
							 FALSE,                 // Italic
							 DEFAULT_CHARSET,       // CharSet
							 OUT_DEFAULT_PRECIS,    // OutputPrecision
							 DEFAULT_QUALITY,       // Quality
							 DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
							 faceName,              // pFaceName
							 ppFont);              // ppFont

	return hResult;
}

void QGraphicsD3D::Clear(bool Color, bool Depth, bool Stencil)
{
	DWORD ClearFlags = 0;
	if (Color) ClearFlags += D3DCLEAR_TARGET;
	if (Depth) ClearFlags += D3DCLEAR_ZBUFFER;
	if (Stencil) ClearFlags += D3DCLEAR_STENCIL;

	pDevice->Clear(0, NULL, ClearFlags, ClearColor, 1.0, 0);

}

void QGraphicsD3D::SetClearColor(DWORD Color) {	ClearColor = Color; }
void QGraphicsD3D::BeginRendering() { pDevice->BeginScene(); }
void QGraphicsD3D::EndRendering() {	pDevice->EndScene(); }
void QGraphicsD3D::Display() { pDevice->Present(0, 0, 0, 0); }
void QGraphicsD3D::SetTechnique(LPCSTR Technique) {	pEffect->SetTechnique(Technique); }

void QGraphicsD3D::DrawObject(int MeshIndex, int TextureIndex, D3DXVECTOR3 Position, D3DXVECTOR3 Rotation, int nVertices, int nPolys)
{
	UINT nPasses;

	//prepare data source
	pDevice->SetStreamSource(0, VertexBuffers[MeshIndex], 0, sizeof(VFORMAT));
	pDevice->SetIndices(IndexBuffers[MeshIndex]);
	pDevice->SetTexture(0, Textures[TextureIndex]);

	//set up matrices
	D3DXMatrixTranslation(&TranslationMatrix, Position.x, Position.y, Position.z);
	D3DXMatrixRotationYawPitchRoll(&RotationMatrix, Rotation.x, Rotation.y, Rotation.z);
	D3DXMatrixMultiply(&WorldMatrix, &RotationMatrix, &TranslationMatrix);
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);
	
	//set shader vars
	WorldViewProjectionMatrix = WorldMatrix * ViewMatrix * ProjectionMatrix;
	pEffect->SetMatrix("WorldViewProjection", &WorldViewProjectionMatrix);
	pEffect->SetMatrix("World", &WorldMatrix);
	pEffect->SetTexture("Texture0", Textures[TextureIndex]);


	//render object
	pEffect->Begin(&nPasses, 0);
		pEffect->BeginPass(0);
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nVertices, 0, nPolys);
		pEffect->EndPass();
	pEffect->End();

}

void QGraphicsD3D::DrawObject (int MeshIndex, int TextureIndex, D3DXMATRIX TransformMatrix, int nVertices, int nPolys)
{
	UINT nPasses;

	//prepare data source
	pDevice->SetStreamSource(0, VertexBuffers[MeshIndex], 0, sizeof(VFORMAT));
	pDevice->SetIndices(IndexBuffers[MeshIndex]);
	pDevice->SetTexture(0, Textures[TextureIndex]);

	WorldMatrix = TransformMatrix;
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);
	
	//set shader vars
	WorldViewProjectionMatrix = WorldMatrix * ViewMatrix * ProjectionMatrix;
	pEffect->SetMatrix("WorldViewProjection", &WorldViewProjectionMatrix);
	pEffect->SetMatrix("World", &WorldMatrix);
	pEffect->SetTexture("Texture0", Textures[TextureIndex]);

	//render object
	pEffect->Begin(&nPasses, 0);
		pEffect->BeginPass(0);
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nVertices, 0, nPolys);
		pEffect->EndPass();
	pEffect->End();
}

void QGraphicsD3D::DrawObject (Q3DObject* object)
{
	UINT nPasses;

	//prepare data source
	pDevice->SetStreamSource(0, VertexBuffers[object->GetMeshIndex()], 0, sizeof(VFORMAT));
	pDevice->SetIndices(IndexBuffers[object->GetMeshIndex()]);
	pDevice->SetTexture(0, Textures[object->GetTextureIndex()]);

	//set up matrices
	D3DXMatrixTranslation(&TranslationMatrix, object->Position.x, object->Position.y, object->Position.z);
	D3DXMatrixRotationYawPitchRoll(&RotationMatrix, object->Rotation.x, object->Rotation.y, object->Rotation.z);
	D3DXMatrixMultiply(&WorldMatrix, &RotationMatrix, &TranslationMatrix);
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);
	
	//set shader vars
	WorldViewProjectionMatrix = WorldMatrix * ViewMatrix * ProjectionMatrix;
	pEffect->SetMatrix("WorldViewProjection", &WorldViewProjectionMatrix);
	pEffect->SetMatrix("World", &WorldMatrix);
	pEffect->SetTexture("Texture0", Textures[object->GetTextureIndex()]);

	//load additional textures
	char tex_name [10];
	for (int i = 1; i <= object->nAddTex; i++)
	{
		sprintf_s(tex_name, 10, "Texture%u", i);
		pEffect->SetTexture(tex_name, Textures[object->AdditionalTextures[i-1]]);
	}

	//render object
	pEffect->Begin(&nPasses, 0);
		pEffect->BeginPass(0);
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, object->GetVertices(), 0, object->GetPolys());
		pEffect->EndPass();
	pEffect->End();
}

void QGraphicsD3D::SetUpCamera(D3DXVECTOR3 Position, D3DXVECTOR3 Rotation, float FOV, float Aspect, float zn, float zf)
{
	FlyCam->Position = Position;
	FlyCam->Rotation = Rotation;
	D3DXVECTOR3 TargetPos(0, 0, 0);
	D3DXVECTOR3 UpVector(0, 1, 0);
	D3DXMatrixLookAtLH(&ViewMatrix, &Position, &TargetPos, &UpVector);
	pDevice->SetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixPerspectiveFovLH(&ProjectionMatrix, FOV, Aspect, zn, zf);
	pDevice->SetTransform(D3DTS_PROJECTION, &ProjectionMatrix);

	FlyCam->CalculateTarget();

	if(engine->GetFullScreen()) engine->SetFreeLook(true);
}

void QGraphicsD3D::UpdateCamera()
{
	CurrentCam->CalculateTarget();

	D3DXVECTOR3 EyePos(CurrentCam->Position.x, CurrentCam->Position.y, CurrentCam->Position.z);
	D3DXVECTOR3 TargetPos(CurrentCam->Position.x + CurrentCam->Target.x, CurrentCam->Position.y + CurrentCam->Target.y,
		CurrentCam->Position.z + CurrentCam->Target.z);
	D3DXVECTOR3 UpVector(0, 1, 0);
	D3DXMatrixLookAtLH(&ViewMatrix, &EyePos, &TargetPos, &UpVector);

	pDevice->SetTransform(D3DTS_VIEW, &ViewMatrix);
}

int QGraphicsD3D::AllocateMesh(Q3DObject* object)
{
	//check for a free mesh index
	for (int i=1; i<MAX_MESH_BUFFERS; i++)
	{
		if(VertexBuffers[i] == NULL) //allocate mesh and return
		{
			//create vertex buffer
			hResult = pDevice->CreateVertexBuffer(object->GetVertices() * sizeof(VFORMAT), 0, 
				FVF, D3DPOOL_MANAGED, &VertexBuffers[i], NULL);
			if(FAILED(hResult)) engine->Decease(L"Failed creating vertex buffer.");

			//create index buffer
			hResult = pDevice->CreateIndexBuffer(object->GetPolys() * sizeof(Poly),
				D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &IndexBuffers[i], NULL);
			if(FAILED(hResult)) engine->Decease(L"Failed creating index buffer.");

			//fill up vertex buffer
			int objectSize = sizeof(VFORMAT) * object->GetVertices();
			VOID* pVertices;

			hResult = VertexBuffers[i]->Lock(0, objectSize, (void**)&pVertices, 0);
			if(FAILED(hResult)) engine->Decease(L"Locking vertex buffer failed.");
			memcpy(pVertices, object->GetVertexArray(), objectSize);
			VertexBuffers[i]->Unlock();

			//fill up index buffer
			objectSize = sizeof(Poly) * object->GetPolys();
			VOID* pIndices;

			hResult = IndexBuffers[i]->Lock(0, objectSize, (void**)&pIndices, 0);
			if(FAILED(hResult)) engine->Decease(L"Locking index buffer failed.");
			memcpy(pIndices, object->GetPolygonArray(), objectSize);
			IndexBuffers[i]->Unlock();

			return i;
		}
	}

	//free mesh index not found
	return -1;
}

int QGraphicsD3D::AllocateTexture(LPCTSTR FileName)
{
	//check for a free texture index
	for (int i=1; i<MAX_TEXTURES; i++)
	{
		if(Textures[i] == NULL)
		{
			//allocate texture and return
			HRESULT hResult = D3DXCreateTextureFromFile(pDevice, FileName, &Textures[i]);
			if(FAILED(hResult)) engine->Decease(L"Failed allocating texture.");

			return i;
		}
	}

	//free texture index not found
	return - 1;
}

void QGraphicsD3D::ReleaseMesh(int MeshIndex)
{
	if(VertexBuffers[MeshIndex] != NULL)
		VertexBuffers[MeshIndex]->Release();
	if(IndexBuffers[MeshIndex] != NULL)
		IndexBuffers[MeshIndex]->Release();
}

void QGraphicsD3D::ReleaseTexture(int TexIndex)
{
	if(Textures[TexIndex] != NULL)
		Textures[TexIndex]->Release();
}

void QGraphicsD3D::MsgX(LPCTSTR txt, float delay, float duration)
{
	msgXData.write = true;
	StringCchCopy(msgXData.TextBuffer, 127, txt);
	msgXData.WritingInitTime = engine->tman->GetTime();
	msgXData.CharDelayTime = delay;
	msgXData.WritingDuration = duration;
}

void QGraphicsD3D::ShowMessages()
{
	//render FPS counter
	RECT rc;
	SetRect(&rc, engine->GetBufferWidth()-140, 10, engine->GetBufferWidth()-10, 30);
	wchar_t txtFPS [20];
	swprintf_s(txtFPS, 20, L"FPS: %.2f", engine->tman->GetFPS());
	pFont[0]->DrawText(NULL, txtFPS, -1, &rc, 0, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));

	//render MsgX messages
	if(msgXData.write)
	{
		//TimeSinceInit is an amount of time since writing was started
		float TimeSinceInit = engine->tman->GetLastSystemTime() - msgXData.WritingInitTime;
		if(msgXData.WritingDuration == 0 || msgXData.WritingDuration > TimeSinceInit)
		{
			int nChars = 0; //amount of characters to write;
			int msgLen = (int)wcslen(msgXData.TextBuffer);
			wchar_t msgBuf [128];

			//setting amount of chars to write
			if(msgXData.CharDelayTime)
				nChars = (int)(TimeSinceInit / msgXData.CharDelayTime);
			else nChars = msgLen;
			if(nChars > msgLen)
				nChars = msgLen;

			//appending "_" to the end of string
			StringCchCopyNW(msgBuf, 128, msgXData.TextBuffer, nChars);
			if(nChars != msgLen)
			{
				StringCchCatW(msgBuf, 128, L"_");
				nChars++;
			}

			//writing text
			SetRect(&rc, 10, 10, 800, 30);
			pFont[0]->DrawText(NULL, msgBuf, nChars, &rc, 0, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
		}
		else msgXData.write = false;
	}
}

void QGraphicsD3D::PrintText(LPCTSTR pString, LPRECT pRect, DWORD Format, int FontIndex, D3DXVECTOR4 Color)
{
	D3DXCOLOR c (Color.x, Color.y, Color.z, Color.w);
	int nChars = (int)wcslen(pString);
	pFont[FontIndex]->DrawText(NULL, pString, nChars, pRect, Format, c);
}

void QGraphicsD3D::DrawSprite(D3DXVECTOR3 center, D3DXVECTOR3 position, D3DXVECTOR3 scale, int TexIndex)
{
	DWORD flags = D3DXSPRITE_OBJECTSPACE | D3DXSPRITE_BILLBOARD | D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT;
	D3DXMatrixScaling(&ScaleMatrix, scale.x, scale.y, scale.z);
	D3DXMatrixTranslation(&TranslationMatrix, position.x, position.y, position.z);
	D3DXMatrixMultiply(&WorldMatrix, &ScaleMatrix, &TranslationMatrix);

	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);
	pSprite->SetWorldViewLH(&WorldMatrix, &ViewMatrix);

	pSprite->Begin(flags);
	pSprite->Draw(Textures[TexIndex], NULL, &center, NULL, 0xFFFFFFFF);
	pSprite->End();
}

void QGraphicsD3D::SetRenderState(DWORD variable, DWORD value)
{
	pDevice->SetRenderState((D3DRENDERSTATETYPE)variable, value);
}

void QGraphicsD3D::SetLight(D3DLIGHT9* lparams, int nr)
{
	hResult = pDevice->SetLight(nr, lparams);
	hResult = pDevice->LightEnable(nr, TRUE);

	//setting material
	D3DMATERIAL9 Material;
	memset(&Material, 0, sizeof(Material));
	Material.Diffuse.r =  1.0;
	Material.Diffuse.g =  1.0;
	Material.Diffuse.b =  1.0;

	Material.Ambient.r =  0.0;
	Material.Ambient.g =  0.0;
	Material.Ambient.b =  0.0;

	Material.Specular.r = 0.0;
	Material.Specular.g = 0.0;
	Material.Specular.b = 0.0;

	Material.Power = 0.1f;

	pDevice->SetMaterial(&Material);
}
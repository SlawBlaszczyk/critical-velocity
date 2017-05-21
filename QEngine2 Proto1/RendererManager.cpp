#define NOMINMAX

#include "RendererManager.h"
#include <Windows.h>
#include "QEngine.h"

RendererManager::RendererManager(HINSTANCE hInst, QEngine* eng)
{
	hInstance = hInst;
	engine = eng;
	hDLL = NULL;
	pGFX = NULL;
}

RendererManager::~RendererManager()
{
	Release();
}

HRESULT RendererManager::CreateGFX(LPCTSTR name)
{
	if(lstrcmp(name, L"Direct3D") == 0)
	{
		hDLL = LoadLibrary(L"QGraphicsD3D.dll");
		if(!hDLL)
		{
			pGFX = NULL;
			engine->Msg(L"Failed loading graphics subsystem dll.");
			return E_FAIL;
		}
	}
	else
	{
		engine->Msg(L"Graphics subsystem not supported.");
		return E_FAIL;
	}

	CREATEGRAPHICSSUBSYSTEM cgs = 0;
	HRESULT hResult;

	//get pointer to function from DLL
	cgs = (CREATEGRAPHICSSUBSYSTEM) GetProcAddress(hDLL, "CreateGraphicsSubsystem");

	if(!cgs) return E_FAIL;

	//call fuction to create device
	hResult = cgs(hDLL, &pGFX);
	if(FAILED(hResult))
	{
		engine->Msg(L"Creating gfx subsystem from dll failed.");
		return E_FAIL;
	}

	return S_OK;
}

void RendererManager::Release()
{
	RELEASEGRAPHICSSUBSYSTEM rgs = 0;
	HRESULT hResult;

	//get pointer to function from DLL
	if (hDLL)
		rgs = (RELEASEGRAPHICSSUBSYSTEM) GetProcAddress(hDLL, "ReleaseGraphicsSubsystem");

	if (pGFX)
	{
		//call fuction to release device
		hResult = rgs(&pGFX);
		if(FAILED(hResult))
		{
			pGFX = NULL;
		}
	}
}
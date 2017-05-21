#include "SoundsystemManager.h"
#include <Windows.h>
#include "QEngine.h"

SoundsystemManager::SoundsystemManager(HINSTANCE hInst, QEngine* eng)
{
	hInstance = hInst;
	engine = eng;
	hDLL = NULL;
	pSndSys = NULL;
}

SoundsystemManager::~SoundsystemManager()
{
	Release();
}

HRESULT SoundsystemManager::CreateSndSys(LPCTSTR name)
{
	if(lstrcmp(name, L"FMOD") == 0)
	{
		hDLL = LoadLibrary(L"QSoundFMOD.dll");
		if(!hDLL)
		{
			pSndSys = NULL;
			engine->Msg(L"Failed loading sound subsystem dll.");
			return E_FAIL;
		}
	}
	else
	{
		engine->Msg(L"Sound subsystem not supported.");
		return E_FAIL;
	}

	CREATESOUNDSUBSYSTEM cgs = 0;
	HRESULT hResult;

	//get pointer to function from DLL
	cgs = (CREATESOUNDSUBSYSTEM) GetProcAddress(hDLL, "CreateSoundSubsystem");

	if(!cgs) return E_FAIL;

	//call fuction to create device
	hResult = cgs(hDLL, &pSndSys);
	if(FAILED(hResult))
	{
		engine->Msg(L"Creating sound subsystem from dll failed.");
		return E_FAIL;
	}

	return S_OK;
}

void SoundsystemManager::Release()
{
	RELEASESOUNDSUBSYSTEM rgs = 0;
	HRESULT hResult;

	//get pointer to function from DLL
	if (hDLL)
		rgs = (RELEASESOUNDSUBSYSTEM) GetProcAddress(hDLL, "ReleaseSoundSubsystem");

	if (pSndSys)
	{
		//call fuction to release device
		hResult = rgs(&pSndSys);
		if(FAILED(hResult))
		{
			pSndSys = NULL;
		}
	}
}
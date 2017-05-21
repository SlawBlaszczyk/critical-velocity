#ifndef SOUNDSYSTEMMANAGER_H
#define SOUNDSYSTEMMANAGER_H

#include <Windows.h>
#include "IQSound.h"

class QEngine;

class SoundsystemManager
{
private:
	IQSound* pSndSys; //snd 0interface pointer
	HINSTANCE hInstance;
	HMODULE hDLL;
	QEngine* engine;

public:
	SoundsystemManager(HINSTANCE hInst, QEngine* eng);
	~SoundsystemManager();

	HRESULT CreateSndSys(LPCTSTR name);
	void Release();
	IQSound* GetSndSys() {return pSndSys;}
	HINSTANCE GetModule() {return hDLL;}

};

#endif;
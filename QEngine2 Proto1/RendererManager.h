#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#define NOMINMAX

#include <Windows.h>
#include "IQGraphics.h"

class QEngine;

class RendererManager
{
private:
	IQGraphics* pGFX;
	HINSTANCE hInstance;
	HMODULE hDLL;
	QEngine* engine;

public:
	RendererManager(HINSTANCE hInst, QEngine* eng);
	~RendererManager();

	HRESULT CreateGFX(LPCTSTR name);
	void Release();
	IQGraphics* GetGFX() {return pGFX;}
	HINSTANCE GetModule() {return hDLL;}

};

#endif;
#ifndef QENGINE_H
#define QENGINE_H

#define NOMINMAX
#include "Structures.h"
#include <Windows.h>


class IQGraphics;
class IQSound;
class TimeManager;
class RendererManager;
class SoundsystemManager;
class IInput;
class QPhysics;
class Music;
class ViewFrustum;

class QEngine
{
protected:
	//variables
	HINSTANCE hInstance;
	HWND hWnd;
	HRESULT hResult;
	RECT rc;
	bool bFullScreen;
	bool bFreeLook;
	bool bPaused;
	bool bMinimized;
	int BufferWidth;
	int BufferHeight;

	//functions
	HWND CreateAppWindow(int Width, int Height, wchar_t* WindowName);
	ATOM RegisterWndClass();
	static LRESULT CALLBACK WndProcWrapper(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	int MessagePump();

	void InitEngine();
	void ShutdownEngine();
	void CreateSubsystems();

	virtual void CheckInput() = 0;
	virtual void WorldUpdate() = 0;
	virtual void SoundUpdate() = 0;
	virtual void RenderScene() = 0;
	virtual void GameInit() = 0;
	virtual void GameDestroy() = 0;
	virtual void GameProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

public:

	//subsystems
	IQGraphics* graphics;
	IQSound* sound;
	TimeManager* tman;
	RendererManager* rman;
	SoundsystemManager* sman;
	IInput* input;
	QPhysics* physics;
	Music* music;

	ViewFrustum* viewfrustum;

	QEngine();
	~QEngine();
	int Main(HINSTANCE hInst);
	void Decease();
	void Decease(LPCTSTR);
	void DeceaseA(LPCSTR);
	void Msg(LPCTSTR);
	void MsgX(LPCTSTR txt, float delay, float duration);

	//accessors
	HWND GetHwnd() {return hWnd;}
	int GetBufferWidth() {return BufferWidth;}
	int GetBufferHeight() {return BufferHeight;}
	bool GetFullScreen() {return bFullScreen;}
	void SetFreeLook(bool val) {bFreeLook = val;}
	bool GetFreeLook() {return bFreeLook;}
};

#endif
#include "QEngine.h"
#include "RendererManager.h"
#include "SoundsystemManager.h"
#include "TimeManager.h"
#include "IQGraphics.h"
#include "IQSound.h"
#include "IInput.h"
#include "QPhysics.h"
#include "Music.h"
#include "ViewFrustum.h"
#include <cstdlib>

//#pragma warning(disable : 4996) // disable deprecated warning 
//#include <strsafe.h>
//#pragma warning(default : 4996) 

const char* WND_CLASS_NAME = "QEngineApp";
QEngine* pEngine;

QEngine::QEngine()
{
	pEngine = this;
	bFullScreen = true;
	bMinimized = false;
	bPaused = false;
	tman = NULL;
	rman = NULL;
	sman = NULL;
	graphics = NULL;
	input = NULL;
	physics = NULL;
	music = NULL;
	viewfrustum = NULL;
}

QEngine::~QEngine()
{
	if(bFullScreen)
		ShowCursor(true);
	ShutdownEngine();
	return;
}

int QEngine::Main(HINSTANCE hInst)
{	
	hInstance = hInst;

	if(MessageBox(NULL, L"Run in windowed mode?",
      L"Window?", MB_YESNO | MB_ICONQUESTION) == IDYES) bFullScreen = false;
	else bFullScreen = true;

	RegisterWndClass();
	CreateAppWindow(1280, 720, L"QEngine 2.1 Prototype 6");

	InitEngine();
	GameInit();
	ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

	int exitParam = MessagePump();

	UnregisterClass((LPCWSTR)WND_CLASS_NAME, hInstance);
	return exitParam;
}

ATOM QEngine::RegisterWndClass()
{
	WNDCLASSEX WndClassEx = {0};
	WndClassEx.cbSize = sizeof(WNDCLASSEX);
	WndClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	WndClassEx.hInstance = GetModuleHandle(NULL);
	WndClassEx.lpszClassName = (LPCWSTR)WND_CLASS_NAME;
	WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClassEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClassEx.lpfnWndProc = &WndProcWrapper;
	WndClassEx.lpszMenuName = NULL;
	WndClassEx.cbClsExtra = 0;
	WndClassEx.cbWndExtra = 0;
	WndClassEx.hIconSm = NULL;

	return RegisterClassEx(&WndClassEx);
}

HWND QEngine::CreateAppWindow(int Width, int Height, wchar_t* WindowName)
{
	BufferWidth = Width;
	BufferHeight = Height;

	DWORD dwStyle =0, dwStyleEx =0;
	int WndXPos =0, WndYPos =0;
	if (bFullScreen)
	{
		dwStyle = WS_POPUP;
		dwStyleEx = WS_EX_APPWINDOW;
	}
	else
	{
		dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
		WndXPos = CW_USEDEFAULT;
		WndYPos = CW_USEDEFAULT;
	}

	RECT rc = {0, 0, Width, Height};
	AdjustWindowRectEx(&rc, dwStyle, false, dwStyleEx);
	hWnd = CreateWindowEx(dwStyleEx, (LPCWSTR)WND_CLASS_NAME, (LPCWSTR)WindowName, dwStyle, WndXPos, WndYPos,
		rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, GetModuleHandle(NULL), NULL);
	return hWnd;
}

int QEngine::MessagePump()
{
	MSG msg = {0};
	while(msg.message != WM_QUIT)
	{
		if(!bMinimized)
		{
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else //if(graphics->GetDevice())
			{
				tman->TimeUpdate();
				WorldUpdate();
				SoundUpdate();
				RenderScene();
				CheckInput();
			}
		}
		else
		{
			if(GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK QEngine::WndProcWrapper(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return pEngine->WndProc(hWnd, message, wParam, lParam);
}

LRESULT QEngine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		GameDestroy();
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		input->aKeys[wParam] = true;
		return 0;
	case WM_KEYUP:
		input->aKeys[wParam] = false;
		return 0;
	case WM_POWERBROADCAST:
		if (wParam == PBT_APMQUERYSUSPEND)
			return BROADCAST_QUERY_DENY;
		break;
	case WM_SIZE:
		switch(wParam)
		{
		case SIZE_MINIMIZED:
			bMinimized = true;
			break;
		case SIZE_RESTORED:
			bMinimized = false;
			break;
		}
	default:
		//other messages processed by game
		GameProcessMessage(hWnd, message, wParam, lParam);
	}
   return DefWindowProc(hWnd, message, wParam, lParam);
}

void QEngine::CreateSubsystems()
{
	//init RendererManager, TimeManager
	rman = new RendererManager(hInstance, this);
	sman = new SoundsystemManager(hInstance, this);
	tman = new TimeManager;

	//Load gfx subsystem
	rman->CreateGFX(L"Direct3D");
	graphics = rman->GetGFX();
	graphics->Init(this, hWnd);

	//Load sound subsystem
	sman->CreateSndSys(L"FMOD");
	sound = sman->GetSndSys();
	sound->Init(hWnd);

	//Load physics subsystem
	physics = new QPhysics(this);
	physics->Init();

	music = new Music(this);
}

void QEngine::InitEngine()
{
	if(bFullScreen)
		ShowCursor(false);
	
	CreateSubsystems();

	viewfrustum = new ViewFrustum(this, D3DX_PI/4.0f, 16.0f/10.0f, 0.8f, 700.0f);
	graphics->SetUpCamera(D3DXVECTOR3(1015.0f, 102.0f, 459.4f), D3DXVECTOR3(1.6f, 0.32f, 0.0f), viewfrustum->FOV, 
		viewfrustum->AspectRatio, viewfrustum->zn, viewfrustum->zf);

	//start random generator
	std::srand(GetTickCount());

	MsgX(L"Welcome to Critical Velocity.", 0, 0);
}

void QEngine::ShutdownEngine()
{
	//release gfx subsystem
	if(rman != NULL)
		delete rman;

	//release input
	if(input != NULL)
		delete input;

	//delete time manager
	if(tman != NULL)
		delete tman;

	//delete physics
	if(physics!= NULL)
		delete physics;

	//delete sound manager
	if(sman != NULL)
		delete sman;

	if(music != NULL)
		delete music;
	if(viewfrustum != NULL)
		delete viewfrustum;
}

void QEngine::Decease()
{
	MessageBox(hWnd, L"Generalnie cos nawalilo", L"Error encountered", MB_OK);
	SendMessage(hWnd, WM_DESTROY, 0, 0);
}

void QEngine::Decease(LPCTSTR err)
{
	MessageBox(hWnd, err, L"Error encountered", MB_OK);
	SendMessage(hWnd, WM_DESTROY, 0, 0);
}

void QEngine::DeceaseA(LPCSTR err)
{
	MessageBoxA(hWnd, err, "Error encountered", MB_OK);
	SendMessage(hWnd, WM_DESTROY, 0, 0);
}

void QEngine::Msg(LPCTSTR msg)
{
	MessageBox(hWnd, msg, L"Message", MB_OK);
}

void QEngine::MsgX(LPCTSTR txt, float delay, float duration)
{
	graphics->MsgX(txt, delay, duration);
}
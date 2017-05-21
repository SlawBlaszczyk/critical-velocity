#ifndef GAME_H
#define GAME_H

#define NOMINMAX
#include <Windows.h>
#include <queue>
#include "QEngine.h"
#include "Event.h"

using namespace std;

class Logic;

class Game : public QEngine
{
	friend class Frame;

	bool bAnimate;
	int State;
	int BlastTexIndex;

	virtual void CheckInput();
	virtual void WorldUpdate();
	virtual void SoundUpdate();
	virtual void RenderScene();
	virtual void GameInit();
	virtual void GameDestroy();
	virtual void GameProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void InitGlobalLights();
	void RenderBombs();
	void RenderCreatures();
	void RenderObstacles();
	void PrintCustomMessages();
	void PrintMajorMessages();

public:

	Logic* logic;

	bool GetAnimate();
	void SetAnimate(bool);
	Game();
	~Game();
};

#endif
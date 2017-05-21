#ifndef IINPUT_H
#define IINPUT_H

#define NOMINMAX
#include <Windows.h>

class Game;

class IInput
{
protected:
	Game* engine;

public:
	IInput();
	bool aKeys[256];
	bool aKeyPressed[256];
	virtual void CheckKeys() = 0;
	virtual void ProcessMouse(UINT message, WPARAM wParam, LPARAM lParam) = 0;
};

#endif
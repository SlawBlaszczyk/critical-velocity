#ifndef INPUTPROCESSOR_H
#define INPUTPROCESSOR_H

#include "IInput.h"

class InputProcessor : public IInput
{
	float LastMouseX;
	float LastMouseY;
	float MouseSensitivity;

public:
	InputProcessor(Game* eng);
	void CheckKeys();
	void ProcessMouse(UINT message, WPARAM wParam, LPARAM lParam);
};

#endif
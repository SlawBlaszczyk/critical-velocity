#include "IInput.h"

IInput::IInput()
{
	for (int i = 0; i < 256; i++)
		aKeys[i] = false;
	for (int i = 0; i < 256; i++)
		aKeyPressed[i] = false;
}
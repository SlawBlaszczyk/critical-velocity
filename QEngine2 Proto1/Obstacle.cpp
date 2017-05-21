#include "Obstacle.h"

#pragma warning(disable : 4996) // disable deprecated warning 
#include <strsafe.h>
#pragma warning(default : 4996) 

Obstacle::Obstacle(Q3DObject* PatternObject, wchar_t* Name_) : Q3DObject(PatternObject)
{
	StringCchCopyNW(Name, 20, Name_, 20);
}
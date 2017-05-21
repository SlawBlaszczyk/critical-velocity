#include "..\QEngine2 Proto1\QEngine.h"

void QEngine::Decease(LPCTSTR err)
{
	MessageBox(hWnd, err, L"Error encountered", MB_OK);
	SendMessage(hWnd, WM_DESTROY, 0, 0);
}
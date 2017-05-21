#ifndef FRAME_H
#define FRAME_H

#include <D3DX9.h>

class Q3DObject;
class Game;

class Frame
{
public:
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Rotation;
	D3DXMATRIX TransformMatrix;	//frame transformation matrix
	Frame* pSibling;
	Frame* pChild;
	Q3DObject* Model;
	Game* engine;

	Frame(Game* eng);
	Frame(Game* eng, char* _3DSName, LPCTSTR TexName);
	~Frame();
	void UpdateHierarchy(D3DXMATRIX srcMatrix); //updates all matrices in frame hierarchy
	void DrawHierarchy();	//draw all lower frames
	void Draw();
	void SetElementaryMatrix();

};

#endif

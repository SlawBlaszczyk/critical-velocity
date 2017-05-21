#include "Frame.h"
#include "Game.h"
#include "IQGraphics.h"
#include "Q3DObject.h"

Frame::Frame(Game* eng)
{
	engine = eng;
	pSibling = NULL;
	pChild = NULL;
	Model = NULL;

	Position.x = 0.0f;
	Position.y = 0.0f;
	Position.z = 0.0f;

	Rotation.x = 0.0f;
	Rotation.y = 0.0f;
	Rotation.z = 0.0f;
}

Frame::~Frame()
{
	if (Model != NULL)
		delete Model;
}

void Frame::Draw()
{
	engine->graphics->DrawObject(Model->GetMeshIndex(), Model->GetTextureIndex(), TransformMatrix,
		Model->GetVertices(), Model->GetPolys());
}

void Frame::SetElementaryMatrix()
{
	D3DXMATRIX tempMatrix;
	D3DXMatrixRotationYawPitchRoll(&tempMatrix, Rotation.x, Rotation.y, Rotation.z);
	D3DXMatrixTranslation(&TransformMatrix, Position.x, Position.y, Position.z);
	TransformMatrix = tempMatrix * TransformMatrix;
}

void Frame::UpdateHierarchy(D3DXMATRIX srcMatrix)
{
	SetElementaryMatrix();
	TransformMatrix = TransformMatrix * srcMatrix;	//update the matrix with parent's

	if (pSibling != NULL)	//transform sibling with the same matrix
		pSibling->UpdateHierarchy(srcMatrix);

	if (pChild != NULL)		//transform child with updated matrix
		pChild->UpdateHierarchy(TransformMatrix);

}

void Frame::DrawHierarchy()
{
	Draw();

	if (pSibling != NULL)	//draw sibling
		pSibling->DrawHierarchy();

	if (pChild != NULL)		//draw child
		pChild->DrawHierarchy();
}
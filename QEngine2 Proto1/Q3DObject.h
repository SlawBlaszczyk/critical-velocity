#ifndef Q3DOBJECT_H
#define Q3DOBJECT_H

#define NOMINMAX

#include <D3D9.h>
#include "QVObject.h"
#include "Structures.h"

class Game;

class Q3DObject : public QVObject
{
protected:
	char Name [32];
	int nVertices;
	int nPolys;
	bool textured;
	Game* engine;

	bool OwnMesh;
	bool OwnTexture;

	VFORMAT* VertexArray;
	Poly* PolygonArray;
	Plane* PlanesArray;

	//indices of VRAM buffers
	int MeshIndex;
	int TextureIndex;

	D3DMATERIAL9 Material; //TODO: change to universal material

	bool bRender;	//if false, do not render this object

public:

	//more indices of vram buffers
	int AdditionalTextures[MAX_ADDITIONAL_TEXTURES];
	int nAddTex; //number of additional textures

	//functions
	Q3DObject(Game* eng, char* MeshFileName);
	Q3DObject(Game* eng, char* MeshFileName, LPCTSTR TexFileName);
	Q3DObject(Q3DObject* src);
	Q3DObject(Game* eng);
	~Q3DObject();
	//loads mesh, allocates in vram and returns mesh index
	int LoadMesh3DS(char* MeshFileName);
	int LoadTextureFromFile(LPCTSTR TexFileName);
	void CalculateNormals();
	int AddAdditionalTexture(LPCTSTR TexFileName);

	//accessor functions
	void SetDefaultMaterial();
	int GetVertices() {return nVertices;}
	void SetVertices(int v) {nVertices = v;}
	int GetPolys() {return nPolys;}
	void SetPolys(int p) {nPolys = p;}
	void SetTextureIndex (int texIndex) {TextureIndex = texIndex;}
	int GetTextureIndex() {return TextureIndex;}
	int GetMeshIndex() {return MeshIndex;}
	void SetMeshIndex(int mi) {MeshIndex=mi;}
	VFORMAT* GetVertexArray() {return VertexArray;}
	void SetVertexArray(VFORMAT* va) {VertexArray = va;}
	Poly* GetPolygonArray() {return PolygonArray;}
	void SetPolygonArray(Poly* pa) {PolygonArray = pa;}
	void SetPlanesArray(Plane* pa) {PlanesArray = pa;}
	void SetOwnMesh(bool b) {OwnMesh = b;}
	void SetOwnTexture(bool b) {OwnTexture = b;}
	void SetRender (bool b) {bRender = b;}
	bool GetRender () {return bRender;}
};

#endif
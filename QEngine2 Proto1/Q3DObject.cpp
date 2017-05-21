#include "Q3DObject.h"
#include "Game.h"
#include "IQGraphics.h"
#include <D3DX9.h>
#include <cstdio>
#include <io.h>

#pragma warning(disable : 4996) // disable deprecated warning 
#include <strsafe.h>
#pragma warning(default : 4996)

Q3DObject::Q3DObject(Game* eng)
{
	VertexArray = NULL;
	PolygonArray = NULL;
	PlanesArray = NULL;
	MeshIndex = 0;
	TextureIndex = 0;
	SetDefaultMaterial();
	OwnMesh = false;
	OwnTexture = false;
	engine = eng;

	for (int i=0; i< MAX_ADDITIONAL_TEXTURES; i++)
		AdditionalTextures[i] = -1;
	nAddTex = 0;
}

//primary constructor
Q3DObject::Q3DObject(Game* eng, char* MeshFileName, LPCTSTR TexFileName)
{
	engine = eng;
	textured = true;
	VertexArray = NULL;
	PolygonArray = NULL;
	PlanesArray = NULL;
	SetDefaultMaterial();
	OwnMesh = true;
	OwnTexture = true;
	MeshIndex = LoadMesh3DS(MeshFileName);
	TextureIndex = LoadTextureFromFile(TexFileName);

	for (int i=0; i< MAX_ADDITIONAL_TEXTURES; i++)
		AdditionalTextures[i] = -1;
	nAddTex = 0;
}

//secondary constructor
Q3DObject::Q3DObject(Game* eng, char* MeshFileName)
{
	engine = eng;
	textured = true;
	VertexArray = NULL;
	PolygonArray = NULL;
	PlanesArray = NULL;
	SetDefaultMaterial();
	OwnMesh = true;
	OwnTexture = false;
	MeshIndex = LoadMesh3DS(MeshFileName);
	TextureIndex = NULL;

	for (int i=0; i< MAX_ADDITIONAL_TEXTURES; i++)
		AdditionalTextures[i] = -1;
	nAddTex = 0;
}

//a "reference" constructor, using texture and mesh specified in a source object
//this is used for multiple instancing of the same model
Q3DObject::Q3DObject(Q3DObject* src)
{
	this->engine = src->engine;
	textured = true;
	VertexArray = src->VertexArray;
	PolygonArray = src->PolygonArray;
	PlanesArray = src->PlanesArray;
	nVertices = src->nVertices;
	nPolys = src->nPolys;
	MeshIndex = src->MeshIndex;
	TextureIndex = src->TextureIndex;
	StringCbCopyA(Name, 20, src->Name);
	SetDefaultMaterial();

	OwnMesh = false;
	OwnTexture = false;

	for (int i=0; i< MAX_ADDITIONAL_TEXTURES; i++)
		AdditionalTextures[i] = -1;
	nAddTex = 0;
}

Q3DObject::~Q3DObject()
{
	if(OwnMesh)
		engine->graphics->ReleaseMesh(MeshIndex);
	if(OwnTexture)
	{
		engine->graphics->ReleaseTexture(TextureIndex);

		//release 'additional textures'
		for (int i=0; i< nAddTex; i++)
			engine->graphics->ReleaseTexture(AdditionalTextures[i]);
	}

	if(OwnMesh)
	{
		if(VertexArray != NULL)
			delete VertexArray;
		if(PolygonArray != NULL)
			delete PolygonArray;
		if(PlanesArray != NULL)
			delete PlanesArray;
	}
}

int Q3DObject::AddAdditionalTexture(LPCTSTR TexFileName)
{
	AdditionalTextures[nAddTex] = engine->graphics->AllocateTexture(TexFileName);
	return nAddTex++;
}

int Q3DObject::LoadMesh3DS(char *MeshFileName)
{
	unsigned short ChunkID;
	unsigned int ChunkLength;
	unsigned short Quantity;	//qty of vertices/indices in chunk
	unsigned short FaceFlags;
	
	int j=0;  //iterator
	unsigned char l_char;

	FILE* file3ds;
	if (fopen_s(&file3ds, MeshFileName, "rb") != 0) 
		engine->Decease(L"Couldn't open 3ds file.");

	while (ftell(file3ds) < _filelength(_fileno (file3ds)))
	{
		fread (&ChunkID, 2, 1, file3ds);
		fread (&ChunkLength, 4, 1, file3ds);
		j = ftell(file3ds);
		
		switch (ChunkID)
		{
		case 0x4d4d:	//main chunk
			break;

		case 0x3d3d:	//3deditor chunk
			break;

		case 0x4000:	//object block 
			j=0;
			do
			{
				fread (&l_char, 1, 1, file3ds);
                Name[j]=l_char;
				j++;
            }while(l_char != '\0' && j<20);
			break;

		case 0x4100:	//object trimesh
			break;

		case 0x4110:	//vertices list
			fread (&Quantity, sizeof (unsigned short), 1, file3ds);

			if (Quantity > MAXVERTICES)
				engine->Decease(L"Too many vertices in mesh.");

			nVertices = Quantity;
			VertexArray = new VFORMAT [Quantity];

			for (int i=0; i<Quantity; i++)
                {
					fread (&VertexArray[i].x, sizeof(float), 1, file3ds);
					fread (&VertexArray[i].y, sizeof(float), 1, file3ds);
					fread (&VertexArray[i].z, sizeof(float), 1, file3ds);
				}
			break;

		case 0x4120:	//faces desc
			fread (&Quantity, sizeof (unsigned short), 1, file3ds);

			if (Quantity > MAXPOLY)
				engine->Decease(L"Too many polygons in mesh.");

			nPolys = Quantity;
			PolygonArray = new Poly [nPolys];
			PlanesArray = new Plane [nPolys];

            for (int i=0; i<Quantity; i++)
            {
				fread (&PolygonArray[i].x, sizeof (unsigned short), 1, file3ds);
				fread (&PolygonArray[i].y, sizeof (unsigned short), 1, file3ds);
				fread (&PolygonArray[i].z, sizeof (unsigned short), 1, file3ds);
				fread (&FaceFlags, sizeof (unsigned short), 1, file3ds);
			}
			break;

		case 0x4140:	//mapping coord
			if (textured)
			{
				fread (&Quantity, sizeof (unsigned short), 1, file3ds);
				if (Quantity > MAXVERTICES)
					engine->Decease(L"Too many mapping coordinates in mesh.");

				for (int i=0; i<Quantity; i++)
				{
					fread (&VertexArray[i].u, sizeof (float), 1, file3ds);
					fread (&VertexArray[i].v, sizeof (float), 1, file3ds);
				}
			}
			else fseek(file3ds, ChunkLength-6, SEEK_CUR);
			break;

		case 0x4600:	//light - finalizae reading
			break;

		default:		//something else - skipping
			fseek(file3ds, ChunkLength-6, SEEK_CUR);
			break;
		}
	}
	if(file3ds)
		if (fclose(file3ds))
			engine->Decease(L"Error closing 3ds file");

	CalculateNormals();
	OwnMesh = true;
	//call graphics interface function allocating the mesh in vram
	return engine->graphics->AllocateMesh(this);
}

int Q3DObject::LoadTextureFromFile(LPCTSTR TexFileName)
{
	OwnTexture = true;
	return engine->graphics->AllocateTexture(TexFileName);
}

void Q3DObject::SetDefaultMaterial()
{
	Material.Diffuse.r = 1.0f;
	Material.Diffuse.g = 1.0f;
	Material.Diffuse.b = 1.0f;
	Material.Diffuse.a = 1.0f;
	Material.Specular.r = 0.1f;
	Material.Specular.g = 0.1f;
	Material.Specular.b = 0.1f;
	Material.Specular.a = 0.1f;
	Material.Power = 1.0f;
}

void Q3DObject::CalculateNormals()
{
	D3DXVECTOR3 vb1, vb2, nor, pt1;	//for poly normal calculations
	D3DXVECTOR3 vect1, vect2, vect3, normal; //vars used in vertex normal calculations

	//zeroing vertex normals
	for(int i=0; i<nVertices; i++)
	{
		VertexArray[i].nx = 0;
		VertexArray[i].ny = 0;
		VertexArray[i].nz = 0;
	}

	for (int i=0; i< nPolys; i++)
	{
		//calculating poly normal
		vb1.x = VertexArray[PolygonArray[i].y].x - VertexArray[PolygonArray[i].x].x;	//p2 - p1
		vb1.y = VertexArray[PolygonArray[i].y].y - VertexArray[PolygonArray[i].x].y;
		vb1.z = VertexArray[PolygonArray[i].y].z - VertexArray[PolygonArray[i].x].z;
		vb2.x = VertexArray[PolygonArray[i].z].x - VertexArray[PolygonArray[i].x].x;	//p3 - p1
		vb2.y = VertexArray[PolygonArray[i].z].y - VertexArray[PolygonArray[i].x].y;
		vb2.z = VertexArray[PolygonArray[i].z].z - VertexArray[PolygonArray[i].x].z;
		D3DXVec3Normalize(&vb1, &vb1);
		D3DXVec3Normalize(&vb2, &vb2);
		
		D3DXVec3Cross(&nor, &vb1, &vb2);
		normal = nor;

		//normalizing to load to planes array
		D3DXVec3Normalize(&nor, &nor);
		PlanesArray[i].normal = nor;

		//calculating distance
		pt1.x = VertexArray[PolygonArray[i].x].x;
		pt1.y = VertexArray[PolygonArray[i].x].y;
		pt1.z = VertexArray[PolygonArray[i].x].z;
		PlanesArray[i].dist = D3DXVec3Dot(&nor, &pt1);

		//got poly normals, calculate vertex normals
		VertexArray[PolygonArray[i].x].nx += normal.x;
		VertexArray[PolygonArray[i].x].ny += normal.y;
		VertexArray[PolygonArray[i].x].nz += normal.z;
		VertexArray[PolygonArray[i].y].nx += normal.x;
		VertexArray[PolygonArray[i].y].ny += normal.y;
		VertexArray[PolygonArray[i].y].nz += normal.z;
		VertexArray[PolygonArray[i].z].nx += normal.x;
		VertexArray[PolygonArray[i].z].ny += normal.y;
		VertexArray[PolygonArray[i].z].nz += normal.z;
	}

	//normalizing vertex normals
	for(int i=0; i<nVertices; i++)
	{
		normal.x = VertexArray[i].nx;
		normal.y = VertexArray[i].ny;
		normal.z = VertexArray[i].nz;

		D3DXVec3Normalize(&normal, &normal);

		VertexArray[i].nx = normal.x;
		VertexArray[i].ny = normal.y;
		VertexArray[i].nz = normal.z;
	}
}
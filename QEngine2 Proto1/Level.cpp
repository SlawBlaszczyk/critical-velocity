#define NOMINMAX

#include "Level.h"
#include "Game.h"
#include "Q3DObject.h"
#include "Logic.h"
#include "IQGraphics.h"
#include "NxCooking.h"
#include "NxFoundation.h"
#include "Stream.h"
#include "QuadTreeNode.h"
#include "ViewFrustum.h"
#include <cmath>
#include <cstdio>
#include <io.h>
#include <fstream>

using namespace std;

Level::Level(Game* eng)
{
	engine = eng;
	Terrain = NULL;
	for (int i = 0; i < PATCHESINCOL; i++)
		for (int j = 0; j < PATCHESINROW; j++)
			TerrainPatches[i][j] = NULL;

	nObstacles = 0;
	nBoxes = 0;
	nTrees = 0;
	for (int i = 0; i < 1000; i++)
		Boxes [i] = NULL;
	for (int i = 0; i < 1000; i++)
		Trees [i] = NULL;
	for (int i = 0; i < 1000; i++)
		ObstacleArray [i] = NULL;

	//physics data
	actorDesc = NULL;
	actor = NULL;
	terrainDesc = NULL;
	triangleMesh = NULL;
	tmsd = NULL;
}

Level::Level(Game* eng, char* FileName)
{
	engine = eng;
	Terrain = NULL;
	for (int i = 0; i < PATCHESINCOL; i++)
		for (int j = 0; j < PATCHESINROW; j++)
			TerrainPatches[i][j] = NULL;

	nObstacles = 0;
	nBoxes = 0;
	nTrees = 0;
	for (int i = 0; i < 1000; i++)
		ObstacleArray [i] = NULL;
	for (int i = 0; i < 1000; i++)
		Boxes [i] = NULL;
	for (int i = 0; i < 1000; i++)
		Trees [i] = NULL;

	//physics data
	actorDesc = NULL;
	actor = NULL;
	terrainDesc = NULL;
	triangleMesh = NULL;
	tmsd = NULL;

	//load heightmap
	LoadHeightMap(&HeightMapData, &GridWidth, &GridHeight, FileName);
	//load graphics data
	LoadFromHgtMapIntoPatches("");
	InitPhysicsActor();

	GenerateLevelQuadTree();
}

Level::~Level()
{
	//delete quadtree
	if (QuadTreeRoot != NULL)
		delete QuadTreeRoot;

	//destroy actor
	if (actor != NULL)
		engine->physics->GetScene()->releaseActor(*actor);
	if (terrainDesc != NULL)
		delete terrainDesc;
	if (actorDesc != NULL)
		delete actorDesc;
	if (triangleMesh != NULL)
		engine->physics->GetSDK()->releaseTriangleMesh(*triangleMesh);
	if (tmsd != NULL)
		delete tmsd;

	for (int i = 0; i < nObstacles; i++)
	{
		if (ObstacleArray[i] != NULL)
			delete ObstacleArray [i];
	}

	if(Terrain != NULL)	
		delete Terrain;

	for (int i = 0; i < PATCHESINCOL; i++)
		for (int j = 0; j < PATCHESINROW; j++)
			if(TerrainPatches[i][j] != NULL)
				delete TerrainPatches[i][j];
}

int Level::InitPhysicsActor()
{
	NxU32 nbColumns = GridWidth;
	NxU32 nbRows = GridHeight;

	NxHeightFieldDesc heightFieldDesc;
	heightFieldDesc.nbColumns		= nbColumns;
	heightFieldDesc.nbRows			= nbRows;
	heightFieldDesc.convexEdgeThreshold = 0;

	// allocate storage for samples
	heightFieldDesc.samples			= new NxU32[nbColumns*nbRows];
	heightFieldDesc.sampleStride	= sizeof(NxU32);



	char* currentByte = (char*)heightFieldDesc.samples;
	for (NxU32 row = 0; row < nbRows; row++)
		{
		for (NxU32 column = 0; column < nbColumns; column++)
			{
				NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;
				currentSample->height = (NxI16)HeightMapData[(column * GridWidth + row) *3]; //*3 because 24-bit image, red component
				//currentSample->height = 0;
				currentSample->tessFlag = 0;
				currentSample->materialIndex0 = 1;
				currentSample->materialIndex1 = 1;

				currentByte += heightFieldDesc.sampleStride;
			}
		}

	NxHeightField* heightField = engine->physics->GetSDK()->createHeightField(heightFieldDesc);   
	//Data has been copied therefore free the buffer.    
	delete[] heightFieldDesc.samples;

	////create heightfield shape
	NxHeightFieldShapeDesc heightFieldShapeDesc;    

	heightFieldShapeDesc.heightField     = heightField;    
	heightFieldShapeDesc.heightScale     = 1;    
	heightFieldShapeDesc.rowScale        = 5;    
	heightFieldShapeDesc.columnScale     = 5;    
	heightFieldShapeDesc.materialIndexHighBits = 0;    
	heightFieldShapeDesc.holeMaterial = 2;    


	NxActorDesc actorDesc;    
	actorDesc.shapes.pushBack(&heightFieldShapeDesc);    
	actorDesc.globalPose.t = NxVec3(0, 0, 0);    
	NxActor* newActor = engine->physics->GetScene()->createActor(actorDesc);


	actor = newActor;


	return 1;
}

int Level::LoadFromHgtMap(char* MapFileName)
{
	//if a terrain already exists, destroy it
	if(Terrain != NULL)
	{
		delete Terrain;
		Terrain = NULL;
	}

	int GridWidth, GridHeight; //dimensions of vertex grid (equanle to imagemap dimension)
	unsigned char* ImageData = NULL; //pixels of the bitmap
	LoadHeightMap(&ImageData, &GridWidth, &GridHeight, MapFileName); //loading a hmap

	//Width = GridWidth;
	//Height = GridHeight;

	////////////////////////////////
	//creating an object of a level
	Terrain = new Q3DObject(engine);

	//setting vertices basing on heightmap values
	int nVertices = GridWidth * GridHeight;
	if (nVertices > MAXVERTICES)
		engine->Decease(L"Heightmap too large.");

	//allocate vertex array
	VFORMAT* VertexArray = new VFORMAT [nVertices];
	Terrain->SetVertexArray(VertexArray); 

	//allocate poly array
	int nPolys = 2 * (GridWidth-1) * (GridHeight-1); //count polygons
	Poly* PolygonArray = new Poly [nPolys];
	Terrain->SetPolygonArray(PolygonArray);

	//allocate planes array
	Plane* PlanesArray = new Plane [nPolys];
	Terrain->SetPlanesArray(PlanesArray);

	//save amount of vertices and polygons
	Terrain->SetVertices(nVertices);
	Terrain->SetPolys(nPolys);

	//fill vertex array
	for(int i=0; i<GridHeight; i++)
		for(int j=0; j<GridWidth; j++) //for each pixel in heightmap
		{
			VertexArray[i*GridWidth+j].x = j * 5.0f;
			VertexArray[i*GridWidth+j].z = i * 5.0f;
			VertexArray[i*GridWidth+j].y = ImageData[(i*GridWidth+j)*3]; //vertex height, *3 because 24-bit
		}

	//combine vertices into polygons
	for(int i=0; i<GridHeight-1; i++)
		for(int j=0; j<GridWidth-1; j++) //for each vertex except last row and column make two polys
		{
			PolygonArray[2*(i*(GridWidth-1)+j)].x = i*GridWidth+j; //first poly
			PolygonArray[2*(i*(GridWidth-1)+j)].y = (i+1)*GridWidth+j;
			PolygonArray[2*(i*(GridWidth-1)+j)].z = (i+1)*GridWidth+(j+1);
			PolygonArray[2*(i*(GridWidth-1)+j)+1].x = i*GridWidth+j;	//second poly
			PolygonArray[2*(i*(GridWidth-1)+j)+1].y = (i+1)*GridWidth+(j+1);
			PolygonArray[2*(i*(GridWidth-1)+j)+1].z = i*GridWidth+(j+1);
		}

	//calc normals
	Terrain->CalculateNormals();

	//allocate object mesh in video memory and save handle
	int MeshIndex = engine->graphics->AllocateMesh(Terrain);
	Terrain->SetMeshIndex(MeshIndex);
	Terrain->SetOwnMesh(true); //setting object mesh flag

	////allocate texture
	int TexIndex = engine->graphics->AllocateTexture(L"textures/white.jpg");
	Terrain->SetTextureIndex(TexIndex);
	Terrain->SetOwnTexture(true); //setting object tex flag

	//allocate obstacles
	FillObstacleArray(ImageData, ObstacleArray, GridWidth, GridHeight);

	if(ImageData!=NULL)
		delete[] ImageData;


	return 1;
}

int Level::LoadFromHgtMapIntoPatches(char* MapFileName, int division)
{
	//if a terrain patch already exists, destroy it
	for (int i = 0; i < PATCHESINCOL; i++)
		for (int j = 0; j < PATCHESINROW; j++)
			if(TerrainPatches[i][j] != NULL)
			{
				delete TerrainPatches[i][j];
				TerrainPatches[i][j] = NULL;
			}

	////////////////////////////////
	//creating level patch objects
	for (int i = 0; i < PATCHESINCOL; i++)
		for (int j = 0; j < PATCHESINROW; j++)
			TerrainPatches[i][j] = new Q3DObject(engine);

	//setting total map vertices and polys number basing on heightmap values
	int nVertices = GridWidth * GridHeight;
	if (nVertices > MAXVERTICES)
		engine->Decease(L"Heightmap too large.");
	int nPolys = 2 * (GridWidth-1) * (GridHeight-1); //count polygons

	nPatchVertices = (division+1) * (division+1);
	nPatchPolys = division * division * 2;

	//min and max height values for patch
	int min_hgt, max_hgt;
	int height;
	int flags;

	//preparing temporary mesh data objects
	VFORMAT* tva = new VFORMAT [nPatchVertices];
	Poly* tpa = new Poly [nPatchPolys];
	Plane* tpla = new Plane [nPatchPolys];

	//allocate universal terrain texture
	int texture = engine->graphics->AllocateTexture(L"textures/grass2.jpg");
	int texture2 = engine->graphics->AllocateTexture(L"textures/dirt1.jpg");

	//process each patch
	for (int i = 0; i < PATCHESINCOL; i++)
		for (int j = 0; j < PATCHESINROW; j++)
		{
			//reset min, max height values
			min_hgt = 255;
			max_hgt = 0;

			//set temporary array poiters
			TerrainPatches[i][j]->SetVertexArray(tva); 
			TerrainPatches[i][j]->SetPolygonArray(tpa);
			TerrainPatches[i][j]->SetPlanesArray(tpla);

			//save amount of vertices and polygons
			TerrainPatches[i][j]->SetVertices(nPatchVertices);
			TerrainPatches[i][j]->SetPolys(nPatchPolys);

			//fill vertex array with data
			for(int k=0; k<(division+1); k++)
				for(int l=0; l<(division+1); l++) //for each vertex in patch including redundant edges
				{
					//vertices
					tva[(k*(division+1))+l].x = (j * division + l) * 5.0f;
					tva[(k*(division+1))+l].z = (i * division + k) * 5.0f;
					height = HeightMapData[((i * division + k) * GridWidth + (j * division + l)) * 3];	//vertex height, *3 because 24-bit
					tva[(k*(division+1))+l].y = (float) height;

					//texture coordinates
					tva[(k*(division+1))+l].u = 1.0f / division * l;
					tva[(k*(division+1))+l].v = 1.0f / division * k;

					//texture flags
					flags = HeightMapData[((i * division + k) * GridWidth + (j * division + l)) * 3 + 1];	//vertex height, *3 because 24-bit, +1 because green byte
					if (flags == 50)	//road terrain
					{
						tva[(k*(division+1))+l].u2 = 1.0f;
						tva[(k*(division+1))+l].v2 = 1.0f;
					}
					else if (flags == 100)	//rocky terrain
					{
						tva[(k*(division+1))+l].u2 = 0.0f;
						tva[(k*(division+1))+l].v2 = 0.0f;
					}
					else
					{
						tva[(k*(division+1))+l].u2 = 0.0f;
						tva[(k*(division+1))+l].v2 = 0.0f;		
					}

					//update min & max height for patch
					if (height > max_hgt)
						max_hgt = height;

					if (height < min_hgt)
						min_hgt = height;
				}

			//combine vertices into polygons
			for(int k=0; k<(division); k++)
				for(int l=0; l<(division); l++) //for each vertex in patch except last row/col
				{
					tpa[2*(k*division+l)].x = k*(division+1)+l; //first poly
					tpa[2*(k*division+l)].y = (k+1)*(division+1)+l;
					tpa[2*(k*division+l)].z = (k+1)*(division+1)+(l+1);
					tpa[2*(k*division+l)+1].x = k*(division+1)+l;	//second poly
					tpa[2*(k*division+l)+1].y = (k+1)*(division+1)+(l+1);
					tpa[2*(k*division+l)+1].z = k*(division+1)+(l+1);
				}

			//fix excess patch edges
			if (i == PATCHESINCOL-1)	//change to i/j?
			{
				for(int k=0; k<(division+1); k++)
					tva[(division*(division+1))+k].y = tva[((division-1)*(division+1))+k].y;

				//if (j == PATCHESINCOL-1)
				//	tva[(division+1)].y = 70;//tva[((division-1)*(division+1))+(division - 2)].y;
			}

			//save min & max height data
			MinHeightValues[i][j] = min_hgt;
			MaxHeightValues[i][j] = max_hgt;

			////calc normals
			TerrainPatches[i][j]->CalculateNormals();

			//set position
			//TerrainPatches[i][j]->Position = D3DXVECTOR3();

			//allocate object mesh in video memory and save handle
			TerrainPatches[i][j]->SetMeshIndex(engine->graphics->AllocateMesh(TerrainPatches[i][j]));
			TerrainPatches[i][j]->SetOwnMesh(true); //setting object mesh flag

			//allocate textures
			TerrainPatches[i][j]->SetTextureIndex(texture);
			TerrainPatches[i][j]->SetOwnTexture(false); //setting object tex flag

			//additional textures
			TerrainPatches[i][j]->AdditionalTextures[0] = texture2;
			TerrainPatches[i][j]->nAddTex++;

			//nullify array poiters
			TerrainPatches[i][j]->SetVertexArray(NULL); 
			TerrainPatches[i][j]->SetPolygonArray(NULL);
			TerrainPatches[i][j]->SetPlanesArray(NULL);
		}

	//allocate obstacles
	FillObstacleArray(HeightMapData, ObstacleArray, GridWidth, GridHeight);

	//clean up temporary objects
	if(tva != NULL)
		delete[] tva;
	if(tpa != NULL)
		delete[] tpa;
	if(tpla != NULL)
		delete[] tpla;

	return 1;
}

int Level::LoadHeightMap(unsigned char** ImageData, int* GridWidth, int* GridHeight, char* FileName)
{
	BITMAPFILEHEADER fileheader; 
    BITMAPINFOHEADER infoheader;
    RGBTRIPLE rgb;

	unsigned char* RGBWriter;
	ifstream BmpFile(FileName, ios::binary);
	
	if (!BmpFile) 
	{
		MessageBox(NULL, L"Heightmap file not found.", L"Heightmap error.", MB_OK);
		return (-1); //blad otwarcia pliku
	}
	BmpFile.read((char*)&fileheader, sizeof(fileheader));
	BmpFile.read((char*)&infoheader, sizeof(infoheader));
	
	if(infoheader.biBitCount != 24)
	{
		MessageBox(NULL, L"Only 24bit heightmaps currently supported.", L"Heightmap error.", MB_OK);
		return (-1); //blad glebi kolorow
	}

	*ImageData = new unsigned char[infoheader.biWidth*infoheader.biHeight*4];
	RGBWriter = *ImageData;
	for (int i=0; i<infoheader.biWidth*infoheader.biHeight; i++)
	{
		BmpFile.read((char*)&rgb, 3);
		RGBWriter[0]=rgb.rgbtRed;
		RGBWriter[1]=rgb.rgbtGreen;
		RGBWriter[2]=rgb.rgbtBlue;
		RGBWriter +=3;
	}
	BmpFile.close();

	//return grid dimensions information
	*GridWidth = infoheader.biWidth;
	*GridHeight = infoheader.biHeight;

	return 1;
}

int Level::FillObstacleArray (unsigned char* ImageData, Obstacle** ObstacleArray, int GridWidth, int GridHeight)
{
	//KASIA AI STUFF/////
	int numberOfReadPositions = 0;
	int licznik = 0;
	/////////////////////

	//check all pixels of heightmap for obstacle data
	for(int i=0; i<GridHeight; i++)
		for(int j=0; j<GridWidth; j++) //for each pixel in heightmap
		{
			NxVec3 pos;
			QVector3 qpos;

			//type of obstacle on this coordinate
			int ObstacleType = ImageData[((i*GridWidth+j)*3)+2];

			if (ObstacleType == 0) continue;	//no obstacle
			else if (ObstacleType == 1)			//a tree!
			{
				////allocate new obstacle
				Trees[nTrees] = new OTree(engine->logic->PatternTree02);

				pos.x = j * 5.0f;
				pos.z = i * 5.0f;
				pos.y = ImageData[(i*GridWidth+j)*3]; //vertex height, *3 because 24-bit
				
				//copy position to tree object
				Trees[nTrees]->GetActor()->setGlobalPosition(pos);
				nTrees++;
			}
			else if (ObstacleType == 2)	//pile of boxes
			{
				//create a bunch of boxes
				Boxes[nBoxes] = new OBox(engine->logic->PatternBox);

				//set position
				pos.x = j * 5.0f;
				pos.z = i * 5.0f;
				pos.y = ImageData[(i*GridWidth+j)*3]; //vertex height, *3 because 24-bit
				pos.y += 0.0f;

				//copy position to box object
				Boxes[nBoxes]->GetActor()->setGlobalPosition(pos);
				nBoxes++;

				//create more boxes
				Boxes[nBoxes] = new OBox(engine->logic->PatternBox);
				pos.x = j * 5.0f + 2.053f;
				pos.z = i * 5.0f - 0.0f;
				pos.y = ImageData[(i*GridWidth+j)*3]; //vertex height, *3 because 24-bit
				pos.y += 0.0f;
				Boxes[nBoxes]->GetActor()->setGlobalPosition(pos);
				nBoxes++;


				Boxes[nBoxes] = new OBox(engine->logic->PatternBox);
				pos.x = j * 5.0f - 2.067f;
				pos.z = i * 5.0f + 0.0f;
				pos.y = ImageData[(i*GridWidth+j)*3]; //vertex height, *3 because 24-bit
				pos.y += 0.0f;
				Boxes[nBoxes]->GetActor()->setGlobalPosition(pos);
				nBoxes++;

				Boxes[nBoxes] = new OBox(engine->logic->PatternBox);
				pos.x = j * 5.0f - 0.0f;
				pos.z = i * 5.0f + 2.27f;
				pos.y = ImageData[(i*GridWidth+j)*3]; //vertex height, *3 because 24-bit
				pos.y += 0.0f;
				Boxes[nBoxes]->GetActor()->setGlobalPosition(pos);
				nBoxes++;

				Boxes[nBoxes] = new OBox(engine->logic->PatternBox);
				pos.x = j * 5.0f - 0.0f;
				pos.z = i * 5.0f - 2.102f;
				pos.y = ImageData[(i*GridWidth+j)*3]; //vertex height, *3 because 24-bit
				pos.y += 0.0f;
				Boxes[nBoxes]->GetActor()->setGlobalPosition(pos);
				nBoxes++;

				Boxes[nBoxes] = new OBox(engine->logic->PatternBox);
				pos.x = j * 5.0f + 0.39f;
				pos.z = i * 5.0f - 1.23f;
				pos.y = ImageData[(i*GridWidth+j)*3]; //vertex height, *3 because 24-bit
				pos.y += 2.0f;
				Boxes[nBoxes]->GetActor()->setGlobalPosition(pos);
				nBoxes++;

				Boxes[nBoxes] = new OBox(engine->logic->PatternBox);
				pos.x = j * 5.0f - 0.85f;
				pos.z = i * 5.0f + 0.917f;
				pos.y = ImageData[(i*GridWidth+j)*3]; //vertex height, *3 because 24-bit
				pos.y += 2.0f;
				Boxes[nBoxes]->GetActor()->setGlobalPosition(pos);
				nBoxes++;
			}
			else if(ObstacleType == 10)	//start/finish gate
			{
				finishGate = new OFinishGate(engine->logic->PatternFinishGate);

				pos.x = j * 5.0f;
				pos.z = i * 5.0f;
				pos.y = ImageData[(i*GridWidth+j)*3] - 2.0f; //vertex height, *3 because 24-bit
				
				//copy position to gate object
				finishGate->GetActor()->setGlobalPosition(pos);

				NxQuat q;
				q.fromAngleAxis(-7, NxVec3(0, 1.0f, 0));
				finishGate->GetActor()->setGlobalOrientationQuat(q);
			}
			else if((ObstacleType >= 100) && (ObstacleType < 160))	//checkpoint
			{
				int CheckpointNum = ObstacleType % 100;	//determine number of the checkpoint

				qpos.x = j * 5.0f;
				qpos.z = i * 5.0f;
				qpos.y = ImageData[(i*GridWidth+j)*3]; //vertex height, *3 because 24-bit

				engine->logic->players[0]->checkpoints[CheckpointNum] = qpos;
				engine->logic->players[0]->nCheckpoints++;

			}

			//KASIA AI STUFF/////
			else if (ObstacleType == 200 && ( ImageData[((i*GridWidth+j)*3)+1]!= ObstacleType ) )			
			{
				D3DXVECTOR3* targetPosition = new D3DXVECTOR3();
				targetPosition->x = j * 5.0f;
				targetPosition->y = ImageData[(i*GridWidth+j)*3];
				targetPosition->z = i * 5.0f;
				engine->logic->targetsPositions[numberOfReadPositions] = targetPosition;
				

				numberOfReadPositions++;	
			}
			/////////////////////

		}

	//KASIA AI STUFF/////

	D3DXVECTOR3* localPositions [ 150 ];//numberOfReadPositions ];
	NxVec3 vecpos = engine->logic->car2->actor->getGlobalPosition();
	double distance = sqrt( 
		  ( pow( ( vecpos.x-engine->logic->targetsPositions[0]->x ), 2 ) )
		+ ( pow( ( vecpos.y-engine->logic->targetsPositions[0]->y ), 2 ) )
		+ ( pow( ( vecpos.z-engine->logic->targetsPositions[0]->z ), 2 ) ) );
	
	double smallestDistance = sqrt( 
		  ( pow( ( vecpos.x-engine->logic->targetsPositions[0]->x ), 2 ) )
		+ ( pow( ( vecpos.y-engine->logic->targetsPositions[0]->y ), 2 ) )
		+ ( pow( ( vecpos.z-engine->logic->targetsPositions[0]->z ), 2 ) ) );
	
	int indexOfSmallestDistance = 0;
	
	for( int i = 0; i < numberOfReadPositions; i++ ){
		
		distance = sqrt(
			  ( pow( ( vecpos.x-engine->logic->targetsPositions[i]->x ), 2 ) ) 
			+ ( pow( ( vecpos.y-engine->logic->targetsPositions[i]->y ), 2 ) )
			+ ( pow( ( vecpos.z-engine->logic->targetsPositions[i]->z ), 2 ) ) 
			);
		
		if( distance < smallestDistance ){
			smallestDistance = distance;
			indexOfSmallestDistance = i;
		}
	}

	localPositions[ 0 ] = engine->logic->targetsPositions[ indexOfSmallestDistance ];
	D3DXVECTOR3* a = new D3DXVECTOR3();
	a->x=0;
	a->y=0;
	a->z=0;
	engine->logic->targetsPositions[indexOfSmallestDistance] = a;
	//teraz mamy najbli¿szego s¹siada dla punktu pocz¹tkowego samochodu
	//Trzeba poszukaæ w nastêpnej kolejnoœæi najbli¿szego s¹siada dla znalezionego s¹siada,itd...

	int indexOfPreviousSmallestDistance = 0;
	for( int i = 1; i < numberOfReadPositions; i++ ){
	
		distance = 10000;
		smallestDistance = 10000;
		indexOfSmallestDistance = 0;
		for( int j = 0; j < numberOfReadPositions; j++ ){
		
			if( engine->logic->targetsPositions[j]->x != 0
 				&& engine->logic->targetsPositions[j]->y != 0){

				distance = sqrt( 
					  ( pow(( localPositions[ indexOfPreviousSmallestDistance ]->x - engine->logic->targetsPositions[j]->x ), 2) )
					+ ( pow(( localPositions[ indexOfPreviousSmallestDistance ]->y - engine->logic->targetsPositions[j]->y ), 2) )
					+ ( pow(( localPositions[ indexOfPreviousSmallestDistance ]->z - engine->logic->targetsPositions[j]->z ), 2) ) );
		
				if( distance < smallestDistance ){
					smallestDistance = distance;
					indexOfSmallestDistance = j;
				}
			}
		}
		
		indexOfPreviousSmallestDistance++;
		localPositions[ indexOfPreviousSmallestDistance ] = engine->logic->targetsPositions[ indexOfSmallestDistance ];
		
		engine->logic->targetsPositions[indexOfSmallestDistance] = a;
	}

	for( int i = 0; i < numberOfReadPositions; i++ ){
		engine->logic->targetsPositions[ i ] = localPositions[ i ];
	}

	engine->logic->numberOfTargets = numberOfReadPositions;



	/////////////////////
	return 1;
}

void Level::Draw()
{
	//check if level is ready to render
	if(!engine->logic->GetLevelInitialized()) return;

	//perform culling
	TerrainFrustumCulling();

	//render all patches
	engine->graphics->SetTechnique("terrain");
	for (int i = 0; i < PATCHESINCOL; i++)
		for (int j = 0; j < PATCHESINROW; j++)
			if (TerrainPatches[i][j]->GetRender())
				engine->graphics->DrawObject(TerrainPatches[i][j]);
	engine->graphics->SetTechnique("vpshader");


}

void Level::GenerateLevelQuadTree()
{
	//make tree
	int depth = (int)sqrt((float)PATCHESINCOL); //= 5
	QuadTreeRoot = new QuadTreeNode(this, NULL, depth, QTREE_ROOT);

	//calculate object count
	QuadTreeRoot->CalculateObjCount();
}

void Level::TerrainFrustumCulling()
{
	//clear all render flags for terrain
	for (int i = 0; i < PATCHESINCOL; i++)
		for (int j = 0; j < PATCHESINROW; j++)
			TerrainPatches[i][j]->SetRender(false);

	RenderedPatches = 0;
	NodesTested = 0;

	//traverse quadtree testing each node against view frustum
	QuadTreeRoot->FrustumCull(engine->viewfrustum);
}
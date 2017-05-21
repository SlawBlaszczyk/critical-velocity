#ifndef LEVEL_H
#define LEVEL_H

#define NOMINMAX
#define PATCHESINROW 32
#define PATCHESINCOL 32

#include "Q3DObject.h"
#include "Game.h"
#include "Obstacle.h"
#include "QPhysics.h"
#include "OBox.h"
#include "OTree.h"
#include "OFinishGate.h"
#include "QuadTreeNode.h"

class Level
{
	int nObstacles;	//number of obstacles on the level

	//physics data
	NxActorDesc* actorDesc;
	NxActor* actor;
	NxTriangleMeshDesc* terrainDesc;
	NxTriangleMesh* triangleMesh;
	NxTriangleMeshShapeDesc* tmsd;

	//heightmap data
	unsigned char* HeightMapData;
	int GridWidth, GridHeight;	//dimensions of the vertex grid (equal to heightmap dimension)

	//quadtree
	QuadTreeNode* QuadTreeRoot;

public:
	Game* engine;
	Obstacle* ObstacleArray[1000];
	Q3DObject* Terrain;	//one-object terrain
	Q3DObject* TerrainPatches[PATCHESINCOL][PATCHESINROW];	//multi-object terrain

	int MinHeightValues [PATCHESINCOL][PATCHESINROW];	//min and max height values for each patch
	int MaxHeightValues [PATCHESINCOL][PATCHESINROW];

	//physical obstacles
	OBox* Boxes[1000];
	OTree* Trees [1000];
	OFinishGate* finishGate;

	int nBoxes, nTrees;
	int nPatchVertices;
	int nPatchPolys;

	int RenderedPatches; //amount of patches rendered in a frame
	int NodesTested;	//amount of tested tree nodes in a frame

	Level(Game*);
	Level(Game*, char*);
	~Level();
	void Draw();

	//loads level from an imagemap
	int LoadFromHgtMap(char* MapFileName);

	//divided version, division is number of polys in patch row/column
	int LoadFromHgtMapIntoPatches(char* MapFileName, int division = 8);

	//loads an image
	int LoadHeightMap(unsigned char** ImageData, int*, int*, char* FileName);

	//load obstacle array frome height map
	int FillObstacleArray (unsigned char* ImageData, Obstacle** ObstacleArray, int GridWidth, int GridHeight);

	//makes the physical mesh and initializes physical data
	int InitPhysicsActor();

	void GenerateLevelQuadTree();

	//performs quadtree-based frustum culling for static level objects
	void TerrainFrustumCulling();

	int GetNObstacles() {return nObstacles;}
	int GetGridWidth() {return GridWidth;}
	int GetGridHeight() {return GridHeight;}
};

#endif
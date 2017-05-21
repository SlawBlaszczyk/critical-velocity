#ifndef QUADTREENODE_H
#define QUADTREENODE_H

#define QTREE_CHILD_UPPER_LEFT 0
#define QTREE_CHILD_UPPER_RIGHT 1
#define QTREE_CHILD_LOWER_LEFT 2
#define QTREE_CHILD_LOWER_RIGHT 3
#define QTREE_ROOT 4

#include "QVector3.h"

class Q3DObject;
class Level;
class ViewFrustum;

class QuadTreeNode
{
	QuadTreeNode* parent;
	QuadTreeNode* children [4];

	Level* gamelevel; //pointer to game level object

	bool leaf;
	int subLevel;	//current subdivision level
	int NodeType;	//relation to parent node
	int global_x, global_y;		//global coordinates within the current subLevel

	int objectCount;	//number of objects in all children

public:
	QuadTreeNode(Level* lev_, QuadTreeNode* parent_, int depth, int nodeType, int subLevel_ = 0);	
															//depth = number of subdivisions
															//lev_ = pointer to level
															//node type = relation to parent node
															//subLevel = current subdivision level

	~QuadTreeNode();

	Q3DObject* TerrainPatch;	//pointer to a corresponding terrain patch

	QVector3 AABBCenter;	//center point of node's AABB structure
	QVector3 AABBSideHalves;	//side halves of the AABB

	float AABBMaxHeight;	//max and min height value of AABB
	float AABBMinHeight;

	void CalculateNodeAABB();
	int CalculateObjCount();	//calculates object count in all leaves and returns its value

	//perform frustum culling for hierarchy
	void FrustumCull(ViewFrustum* frustum);
};

#endif
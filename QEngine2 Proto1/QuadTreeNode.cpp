#include "QuadTreeNode.h"
#include "Level.h"
#include "ViewFrustum.h"
#include <cmath>

QuadTreeNode::QuadTreeNode(Level* lev_, QuadTreeNode* parent_, int depth, int nodeType, int subLevel_)
{
	gamelevel = lev_;
	parent = parent_;
	subLevel = subLevel_;
	NodeType = nodeType;
	objectCount = 0;
	
	//determine global node coordinates in current sublevel
	//int sideTiles = pow(2, subLevel);	//number of nodes of this sublevel in whole row/column
	//int parentSideTiles = pow(2, (subLevel-1));

	if (NodeType == QTREE_ROOT) 
	{
		global_x = 0;
		global_y = 0;
	}
	else
	{
		switch (NodeType)
		{
		case QTREE_CHILD_UPPER_LEFT:
			global_x = (parent->global_x * 2);
			global_y = (parent->global_y * 2);
			break;

		case QTREE_CHILD_UPPER_RIGHT:
			global_x = (parent->global_x * 2) + 1;
			global_y = (parent->global_y * 2);
			break;

		case QTREE_CHILD_LOWER_LEFT:
			global_x = (parent->global_x * 2);
			global_y = (parent->global_y * 2) + 1;
			break;

		case QTREE_CHILD_LOWER_RIGHT:
			global_x = (parent->global_x * 2) + 1;
			global_y = (parent->global_y * 2) + 1;
			break;
		}
	}

	if (depth > 0)
		leaf = false;
	else
		leaf = true;

	//recursively initialize children
	for (int i=0; i<4; i++)
	{
		if (!leaf)
		{
			children [i] = new QuadTreeNode(gamelevel, this, depth - 1, i, subLevel + 1);
		}
		else
		{
			//max subdivision level reached, node is a leaf
			children [i] = 0;	//NULL
		}
	}

	//if node is a leaf, link it with the objects
	if(leaf) 
	{
		TerrainPatch = gamelevel->TerrainPatches[global_y][global_x];
		objectCount++;
	}
	else
		TerrainPatch = NULL;

	//calculate AABB
	CalculateNodeAABB();
}

QuadTreeNode::~QuadTreeNode()
{
	if (!leaf)
		for(int i=0; i<4; i++)
		{
			delete children [i];
		}
}

void QuadTreeNode::CalculateNodeAABB()
{
	//calculate AABB center and side x and z
	float x = (float)gamelevel->GetGridWidth(); //get grid width
	x *= 5; //multiply by scale value
	x /= pow(2.0f, subLevel); //divide to get length of a node on current sublevel
	AABBSideHalves.x = x / 2;	//store its half final value

	x *= (global_x + 1); //move to the edge of an an appropriate 'slot' in grid
	x -= AABBSideHalves.x;	//subtract side half to get center final value
	AABBCenter.x = x;

	float z = (float)gamelevel->GetGridHeight(); //get grid height
	z *= 5; //multiply by scale value
	z /= pow(2.0f, subLevel); //divide to get length of a node on current sublevel
	AABBSideHalves.z = z / 2;	//store its half final value

	z *= (global_y + 1); //move to the edge of an an appropriate 'slot' in grid
	z -= AABBSideHalves.z;	//subtract side half to get center final value
	AABBCenter.z = z;

	//get min and max terrain height values
	AABBMaxHeight = 0; //= gamelevel->MaxHeightValues
	AABBMinHeight = 255;
	float height;

	//if a leaf, get it from patch max, min values
	if (leaf)
	{
		AABBMaxHeight = (float)gamelevel->MaxHeightValues[global_y][global_x];
		AABBMinHeight = (float)gamelevel->MinHeightValues[global_y][global_x];
	}

	//else get it from child nodes
	else
	{
		for (int i=0; i<4; i++)
		{
			if (AABBMaxHeight < children[i]->AABBMaxHeight)
				AABBMaxHeight = children[i]->AABBMaxHeight;

			if (AABBMinHeight > children[i]->AABBMinHeight)
				AABBMinHeight = children[i]->AABBMinHeight;
		}
	}

	//get average from min and max to determine centre position
	AABBCenter.y = ((AABBMinHeight + AABBMaxHeight) / 2);

	//full AABB height = difference of min and max
	height = AABBMaxHeight - AABBMinHeight;

	//calculate half height value
	AABBSideHalves.y = height / 2;
}

int QuadTreeNode::CalculateObjCount()
{
	//Traverse tree to count objects
	if(leaf) return objectCount;
	else
	{
		objectCount = 0;
		for (int i = 0; i<4; i++)
			objectCount += children[i]->CalculateObjCount();
	}

	return objectCount;
}

void QuadTreeNode::FrustumCull(ViewFrustum *frustum)
{
	//test node's bounding box against frustum
	int result = frustum->TestAgainstAABB(AABBCenter, AABBSideHalves);
	gamelevel->NodesTested++;

	//if negative, return
	if (result == (-1)) return;

	//if positive, see what to do next
	if(leaf)
	{
		//set objects as renderable
		TerrainPatch->SetRender(true);
		gamelevel->RenderedPatches++;
	}
	else
	{
		//perform test for all children
		for (int i = 0; i < 4; i++)
		{
			children[i]->FrustumCull(frustum);
		}
	}
}
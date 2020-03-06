#pragma once
#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "j1Module.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "p2List.h"

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255

// --------------------------------------------------
// Recommended reading:
// Intro: http://www.raywenderlich.com/4946/introduction-to-a-pathfinding
// Details: http://theory.stanford.edu/~amitp/GameProgramming/
// --------------------------------------------------

class j1PathFinding : public j1Module
{
public:

	j1PathFinding();

	~j1PathFinding();														// Destructor

	bool CleanUp();															// Called before quitting

	void SetMap(uint width, uint height, uchar* data);						// Sets up the walkability map

	int CreatePath(const iPoint& origin, const iPoint& destination);		// Main function to request a path from A to B

	const p2DynArray<iPoint>* GetLastPath() const;							// To request all tiles involved in the last generated path

	bool CheckBoundaries(const iPoint& pos) const;							// Utility: return true if pos is inside the map boundaries

	bool IsWalkable(const iPoint& pos) const;								// Utility: returns true is the tile is walkable

	uchar GetTileAt(const iPoint& pos) const;								// Utility: return the walkability value of a tile

private:

	
	uint width;																// Width of the map.
	uint height;															// Height of the map.

	uchar* map;																// All map walkability values [0..255]

	p2DynArray<iPoint> last_path;											// We store the created path here
};

// forward declaration
struct PathList;

// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	// Fills a list (PathList) of all valid adjacent pathnodes
	uint FindWalkableAdjacents(PathList& list_to_fill) const;
	// Calculates this tile score
	int Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const iPoint& destination);

	// -----------
	int g;
	int h;
	iPoint pos;
	const PathNode* parent; // needed to reconstruct the path in the end
};

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------
struct PathList
{
	// Looks for a node in this list and returns it's list node or NULL
	p2List_item<PathNode>* Find(const iPoint& point) const;

	// Returns the Pathnode with lowest score in this list or NULL if empty
	p2List_item<PathNode>* GetNodeLowestScore() const;

	// -----------
	// The list itself, note they are not pointers!
	p2List<PathNode> list;
};



#endif // __j1PATHFINDING_H__
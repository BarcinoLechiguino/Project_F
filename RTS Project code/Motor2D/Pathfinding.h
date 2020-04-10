#pragma once
#ifndef __PATHFINDING_H__
#define __PATHFINDING_H__

#include "Module.h"
#include "Point.h"
#include <vector>
#include "EntityManager.h"

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255
#define OCCUPIED 2
#define WALKABLE 1
#define NON_WALKABLE 0
// --------------------------------------------------
// Recommended reading:
// Intro: http://www.raywenderlich.com/4946/introduction-to-a-pathfinding
// Details: http://theory.stanford.edu/~amitp/GameProgramming/
// --------------------------------------------------

class PathFinding : public Module
{
public:

	PathFinding();

	~PathFinding();															// Destructor

	bool CleanUp();															// Called before quitting

	void SetMap(uint width, uint height, uchar* data);						// Sets up the walkability map

	int CreatePath(const iPoint& origin, const iPoint& destination);		// Main function to request a path from A to B

	const std::vector<iPoint> GetLastPath() const;							// To request all tiles involved in the last generated path

	bool CheckBoundaries(const iPoint& pos) const;							// Utility: return true if pos is inside the map boundaries

	bool IsWalkable(const iPoint& pos) const;								// Utility: returns true is the tile is walkable

	bool IsOccupied(const iPoint& pos) const;								// Utility: returns true is the tile is walkable

	bool IsNonWalkable(const iPoint& pos) const;

	uchar GetTileAt(const iPoint& pos) const;								// Utility: return the walkability value of a tile

	void FindNearbyWalkable(const iPoint& pos, std::vector<Dynamic_Object*> units_selected) ;				//Finds close tiles to send units to

	bool ChangeWalkability(const iPoint& pos, uchar walkability);			//Change walkability of a tile in map array

private:

	
	uint width;																// Width of the map.
	uint height;															// Height of the map.

	uchar* map;																// All map walkability values [0..255]

	std::vector<iPoint> last_path;
};

struct PathList;															// forward declaration

// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	uint FindWalkableAdjacents(PathList& list_to_fill) const;				// Fills a list (PathList) of all valid adjacent pathnodes

	int Score() const;														// Calculates this tile score
	
	int CalculateF(const iPoint& destination);								// Calculate the F for a specific destination tile

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
	//std::vector<PathNode>::iterator Find(const iPoint& point);
	//std::vector<PathNode>::iterator GetNodeLowestScore();

	std::list<PathNode>::iterator Find(const iPoint& point);
	std::list<PathNode>::iterator GetNodeLowestScore();

	// -----------
	// The list itself, note they are not pointers!
	std::list<PathNode> list;
	//std::vector<PathNode> list;
};



#endif // __PATHFINDING_H__
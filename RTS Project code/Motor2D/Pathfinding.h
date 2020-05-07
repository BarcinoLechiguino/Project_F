#pragma once
#ifndef __PATHFINDING_H__
#define __PATHFINDING_H__

#include <map>
#include "Module.h"
#include "Point.h"

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255
#define OCCUPIED 2
#define WALKABLE 1
#define NON_WALKABLE 0

class Entity;
class DynamicObject;

// --------------------------------------------------
// Recommended reading:
// Intro: http://www.raywenderlich.com/4946/introduction-to-a-pathfinding
// Details: http://theory.stanford.edu/~amitp/GameProgramming/
// --------------------------------------------------

class PathFinding : public Module
{
public:

	PathFinding();

	~PathFinding();																				// Destructor

	bool CleanUp();																				// Called before quitting

public:
	void SetMap(uint width, uint height, uchar* data);											// Sets up the walkability map

	int CreatePath(const iPoint& origin, const iPoint& destination);							// Main function to request a path from A to B

	const std::vector<iPoint> GetLastPath() const;												// To request all tiles involved in the last generated path

	bool CheckBoundaries(const iPoint& pos) const;												// Utility: return true if pos is inside the map boundaries

	uchar GetTileAt(const iPoint& pos) const;													// Utility: return the walkability value of a tile

	bool IsWalkable(const iPoint& pos) const;													// Utility: Returns true if the tile is walkable.
	bool IsNonWalkable(const iPoint& pos) const;												// Utility: Returns true if the tile is non-walkable.
	bool IsOccupied(const iPoint& pos) const;													// Utility: Returns true if the tile is occupied.
	bool IsOccupiedByEnemyEntity(const iPoint& pos) const;										// Utility: Returns true if the tile is occupied by an enemy entity.

	bool PathIsAccessible(const iPoint& origin, const iPoint& destination) const;				// Utility: Returns true if the given position is accessible.

	iPoint FindNearbyPoint(iPoint pos);

	int DistanceInTiles(iPoint tile_1,iPoint tile_2);

	void MoveOrder(const iPoint& pos, std::vector<DynamicObject*> units_selected) ;	//Finds close tiles to send units to
	void AttackOrder(const iPoint& pos, std::vector<DynamicObject*> units_selected);		//Finds tiles in range of target

	bool ChangeWalkability(iPoint pos, Entity* entity, uchar walkability);						//Change walkability of a tile in map array

private:
	uint width;																					// Width of the map.
	uint height;																				// Height of the map.

	uchar* map;																					// All map walkability values [0..255]

	std::vector<iPoint> last_path;
};

struct PathList;																				// forward declaration

// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	uint FindWalkableAdjacents(PathList& list_to_fill) const;									// Fills a list (PathList) of all valid adjacent pathnodes
	bool NodeIsAccessible(const iPoint& pos) const;												// Utility: Returns true if the given node position is accessible.

	int Score() const;																			// Calculates this tile score
	
	int CalculateF(const iPoint& destination);													// Calculate the F for a specific destination tile


	// -----------
	int g;
	int h;
	iPoint pos;
	const PathNode* parent;																		// needed to reconstruct the path in the end
};

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------
struct PathList
{
	std::list<PathNode>::iterator Find(const iPoint& point);
	std::list<PathNode>::iterator GetNodeLowestScore();

	//PathNode* Find(const iPoint& point);

	// -----------
	// The list itself, note they are not pointers!
	std::list<PathNode> list;
	//std::map<int, PathNode> list;
};



#endif // __PATHFINDING_H__
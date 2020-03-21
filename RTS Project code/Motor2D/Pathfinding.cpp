#include "p2Defs.h"
#include "p2Log.h"
#include "App.h"
#include "PathFinding.h"
#include "Map.h"
#include "Brofiler\Brofiler.h"

PathFinding::PathFinding() : Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name = ("pathfinding");
}

// Destructor
PathFinding::~PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	//last_path.Clear();
	last_path.clear();													//Clears all elements in the last_path array/vector.
	last_path.shrink_to_fit();											//Frees all unused allocated memory.

	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	//return t != INVALID_WALK_CODE && t > 0;
	if (t > 0 && t != INVALID_WALK_CODE)
	{
		return true;
	}
	return false;
}

// Utility: return the walkability value of a tile
uchar PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

bool PathFinding::ChangeWalkability(const iPoint& pos, uchar walkability)
{
	if (map != NULL)
	{
		map[(pos.y * App->map->data.width) + pos.x] = walkability;
	}

	return true;
}

// To request all tiles involved in the last generated path
const std::vector<iPoint>* PathFinding::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
//std::vector<PathNode>::const_iterator* PathList::Find(const iPoint& point) const
//{	
//	// std::iterators allow to access their data/pointer directly by using ->. No need to go to iterator->data->var/ iterator->_Ptr->var.
//	for (std::vector<PathNode>::const_iterator item = list.cbegin(); item != list.cend(); ++item)
//	{
//		if (item->pos == point)
//		{
//			return &item;
//		}
//	}
//
//	return nullptr;
//}

//std::vector<PathNode>::iterator PathList::Find(const iPoint& point)
//{
//	// std::iterators allow to access their data/pointer directly by using ->. No need to go to iterator->data->var/ iterator->_Ptr->var.
//	for (std::vector<PathNode>::iterator item = list.begin(); item != list.end(); ++item)
//	{
//		if (item->pos == point)
//		{
//			return item;
//		}
//	}
//
//	return list.end();
//}

std::list<PathNode>::iterator PathList::Find(const iPoint& point)
{
	// std::iterators allow to access their data/pointer directly by using ->. No need to go to iterator->data->var/ iterator->_Ptr->var.
	for (std::list<PathNode>::iterator item = list.begin(); item != list.end(); ++item)
	{
		if (item->pos == point)
		{
			return item;
		}
	}

	return list.end();
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
//std::vector<PathNode>::const_iterator* PathList::GetNodeLowestScore() const
//{
//	//std::vector<PathNode>::const_iterator* ret = nullptr;
//	std::vector<PathNode>::const_iterator ret = list.end();
//
//	int min = 65535;
//
//	// This loop should go from end to begin. Make a tmp list with std::reverse list?
//	for (std::vector<PathNode>::const_iterator item = list.cbegin(); item != list.cend(); item++)
//	{
//		if (item->Score() < min)
//		{
//			min = item->Score();
//			ret = item;
//		}
//	}
//
//	if (ret == list.end())
//	{
//		return nullptr;
//	}
//	else
//	{
//		return &ret;
//	}
//}

//std::vector<PathNode>::iterator PathList::GetNodeLowestScore()
//{
//	std::vector<PathNode>::iterator ret = list.end();
//
//	int min = 65535;
//
//	// This loop should go from end to begin. Make a tmp list with std::reverse list?
//	for (std::vector<PathNode>::iterator item = list.begin(); item != list.end(); item++)
//	{
//		if (item->Score() < min)
//		{
//			min = item->Score();
//			ret = item;
//		}
//	}
//
//	return ret;
//}

std::list<PathNode>::iterator PathList::GetNodeLowestScore()
{
	std::list<PathNode>::iterator ret = list.end();

	int min = 65535;

	// This loop should go from end to begin. Make a tmp list with std::reverse list?
	for (std::list<PathNode>::iterator item = list.begin(); item != list.end(); item++)
	{
		if (item->Score() < min)
		{
			min = item->Score();
			ret = item;
		}
	}

	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;

	uint before = list_to_fill.list.size();

	// north
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// North east
	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// North west
	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// South east
	cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// South west
	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	return list_to_fill.list.size();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;

	int distance_X = abs(pos.x - destination.x);
	int distance_Y = abs(pos.y - destination.y);

	h = (distance_X + distance_Y) * min(distance_X, distance_Y);

	return g + h;
 }

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	BROFILER_CATEGORY("CreatePath", Profiler::Color::SlateGray)
	
	int ret = -1;		//Revise, Should be 1?										            	//The value returned by the function. Done to improve readability.
																					            
	if (IsWalkable(origin) == false || IsWalkable(destination) == false)			            	//IsWalkable() checks if origin and destination are walkable tiles. IsWalkable calls GetTile(), which returns the walkability value only if the given tile is inside the map's boundaries.
	{																				            
		return ret = -1;															            
	}																				            
																					            
	PathList open;																	            	//Declares/Creates the open list (frontier queue).
	PathList closed;																            	//Declares/Creates the closed list (visited list).
																					            
	PathNode origin_node(0, 0, origin, nullptr);									            		//Declares/Creates a node that will store the data of the origin tile. G, H, position and parent are passed as arguments. 
																					            
	//open.list.add(origin_node);													            		//Adds the current node storing the data of the origin tile to the open (frontier) list.
	open.list.push_back(origin_node);												            			//Adds the current node storing the data of the origin tile to the open (frontier) list.
																					            
																					            
	while (open.list.size() != 0)								            						//While the list is not empty. If the count is higher than 0 that means the list is not empty.
	{																				            
		//std::vector<PathNode>::iterator lowest_node = open.GetNodeLowestScore();

		//closed.list.push_back(*lowest_node);
		//std::vector<PathNode>::iterator current_node = prev(closed.list.end()) /*closed.Find(lowest_node->pos)*/;			//As lowest_node will be pushed to the back of the vector, it will always be at the last position.

		//open.list.erase(lowest_node);												            		//Deletes from the open (frontier) list the node with the lowest score, as it has been the one chosen to be moved to.
		//open.list.shrink_to_fit();

		std::list<PathNode>::iterator lowest_node = open.GetNodeLowestScore();

		closed.list.push_back(*lowest_node);
		std::list<PathNode>::iterator current_node = prev(closed.list.end());							//As lowest_node will be pushed to the back of the vector, it will always be at the last position.

		open.list.erase(lowest_node);												            		//Deletes from the open (frontier) list the node with the lowest score, as it has been the one chosen to be moved to.

		if (current_node->pos == destination)			            								//If destination is in the closed list (visited) and the position of the current node is the same as destination's.
		{																			            
			last_path.clear();														            	//Sets the last_path dynArray count of number of elements to 0. Clears the dynArray.
			last_path.shrink_to_fit();												            	//Frees unused allocated memory.																	            
			
			PathNode last_node = (*current_node);

			for (const PathNode* path_node = &last_node; path_node != nullptr; path_node = path_node->parent)
			{
				last_path.push_back(path_node->pos);
			}

			std::reverse(last_path.begin(), last_path.end());						            	//Flips the vector's elements. The first element of the array will be the origin tile and the destination tile the last one.
			ret = last_path.size();													            	//Returns the amount of steps the path has.

			break;
		}

		PathList neighbours;																//Declares a list that will store the Walkable Adjacent nodes of a given node (current_node).
		
		current_node->FindWalkableAdjacents(neighbours);												//Fills the neighbours list with the walkable adjacent nodes of current_node.
		//std::vector<PathNode>::iterator neighbour_iterator = neighbours.list.begin();					//Declares a list item pointer that will iterate the neighbours list.
		std::list<PathNode>::iterator neighbour_iterator = neighbours.list.begin();					//Declares a list item pointer that will iterate the neighbours list.

		int i = 0;
		for (neighbour_iterator; neighbour_iterator != neighbours.list.end(); ++neighbour_iterator)
		{
			PathNode neighbour = *neighbour_iterator;
			
			if (closed.Find(neighbour.pos) == closed.list.end())													//If the neighbour being iterated is not in the closed list (.Find() returns NULL when the item requested is not found).
			{
				if (open.Find(neighbour.pos) != open.list.end())												//If the neighbour being iterated is already in the open list.
				{
					neighbour.CalculateF(destination);													//Calculates the F (F = G + H) of the neighbour being iterated. As G is recalculated (taking into account this new path), it can be compared with the same node in the open list (old path), if it's in it.

					if ((neighbour.g) < (open.Find(neighbour.pos)->g))								//Compares Gs (total flat movement cost) between the neigbour being iterated and the same neighbour in the list.
					{
						open.Find(neighbour.pos)->parent = neighbour.parent;						//Updates the parent of the neighbour in the list with the parent of the neighbour being iterated. 
					}
				}
				else
				{
					neighbour.CalculateF(destination);													//Calculates the F (F = G + H) of the neighbour being iterated. Sets both G and H for this tile/node for a specific path.
					open.list.push_back(neighbour);														//Adds the neighbour being iterated to the open list.
				}
			}
		}

		neighbours.list.clear();																		//Clears the neighbours list so the elements are not accumulated from node to node (tile to tile).
		//neighbours.list.shrink_to_fit();
	}

	return ret;
}
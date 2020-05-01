#include "Brofiler\Brofiler.h"

#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Map.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Dynamic_Object.h"
#include "Static_Object.h"

#include "PathFinding.h"


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

	last_path.clear();
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
	return (pos.x >= 0 && pos.x < (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: return the walkability value of a tile
uchar PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
	{
		return map[(pos.y * width) + pos.x];
	}

	return 0;
}

// Utility: returns true is the tile is walkable
bool PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);

	if (t == WALKABLE)
	{
		return true;
	}

	return false;
}

bool PathFinding::IsNonWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);

	if (t == NON_WALKABLE)
	{
		return true;
	}

	return false;
}

bool PathFinding::IsOccupied(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);

	if (t == OCCUPIED)
	{
		return true;
	}

	return false;
}

bool PathFinding::IsOccupiedByEnemyEntity(const iPoint& pos) const
{
	bool ret = false;
	
	Entity* entity = App->entity_manager->GetEntityAt(pos);

	if (entity != nullptr)
	{
		if (App->entity_manager->IsEnemyEntity(entity))
		{
			ret = true;
		}
	}

	return ret;
}

bool PathFinding::PathIsAccessible(const iPoint& origin, const iPoint& destination) const
{
	return (/*!IsOccupiedByEnemyEntity(destination) &&*/ (IsNonWalkable(origin) || IsNonWalkable(destination) || IsOccupied(destination)));
}

bool PathFinding::ChangeWalkability(iPoint pos, Entity* entity, uchar walkability)
{
	if (map != nullptr)
	{
		if (abs(pos.x) > App->map->data.width || abs(pos.y) > App->map->data.height)
		{
			LOG("THIS NUMBER IS BANANAS!");
			return false;
		}
		else
		{
			if (abs(pos.x) > App->map->data.width || abs(pos.y) > App->map->data.height)
			{
				LOG("THIS NUMBER IS BANANAS!");
				return false;
			}
			else
			{
				map[(pos.y * App->map->data.width) + pos.x] = walkability;
			}
		}

		if (App->entity_manager->IsBuilding(entity) || App->entity_manager->IsResource(entity))
		{
			Static_Object* item = (Static_Object*)entity;

			for (int y = 0; y != item->tiles_occupied_y; ++y)
			{
				for (int x = 0; x != item->tiles_occupied_x; ++x)
				{
					int pos_y = pos.y + y;
					int pos_x = pos.x + x;

					map[(pos_y * App->map->data.width) + pos_x] = walkability;
				}
			}
		}
	}

	return true;
}

// To request all tiles involved in the last generated path
const std::vector<iPoint> PathFinding::GetLastPath() const
{
	return last_path;
}

iPoint PathFinding::FindNearbyPoint(iPoint pos)
{
	iPoint ret(0, 0);
	
	PathList frontier;
	PathList visited;

	PathNode origin_node(0, 0, pos, nullptr);
	
	frontier.list.push_back(origin_node);

	std::list<PathNode>::iterator current_node = frontier.list.begin();

	PathList neighbours;

	while (frontier.list.size())
	{
		current_node->FindWalkableAdjacents(neighbours);																//Fill starting node

		std::list<PathNode>::iterator item = neighbours.list.begin();

		for (; item != neighbours.list.end(); ++item)
		{
			PathNode neighbour = *item;																					// For Readability

			if (visited.Find(neighbour.pos) == visited.list.end())														//if not in visited
			{
				if (App->pathfinding->IsWalkable(neighbour.pos))
				{
					ret = neighbour.pos;
					return ret;
				}

				if (App->pathfinding->IsWalkable(neighbour.pos) || App->pathfinding->IsOccupied(neighbour.pos))
				{
					frontier.list.push_back(neighbour);
				}
			}
		}

		neighbours.list.clear();

		visited.list.push_back((*current_node));

		current_node++;
	}

	return ret;
}

void PathFinding::MoveOrder(const iPoint& pos, std::vector<Dynamic_Object*> units_selected)
{
	if (units_selected.size() != 0)
	{
		std::vector<Dynamic_Object*>::iterator units = units_selected.begin();

		if ((*units)->target == nullptr)
		{
			if (!(*units)->GiveNewTargetTile(pos))
			{
				return;
			}

			units++;
		}

		PathList frontier;
		PathList visited;

		PathNode origin_node(0, 0, pos, nullptr);
		frontier.list.push_back(origin_node);

		std::list<PathNode>::iterator current_node = frontier.list.begin();

		PathList neighbours;

		while (frontier.list.size() != 0 && units != units_selected.end())
		{
			current_node = frontier.list.begin();

			current_node->FindWalkableAdjacents(neighbours);																//Fill starting node

			std::list<PathNode>::iterator item = neighbours.list.begin();

			for (; item != neighbours.list.end() && units != units_selected.end(); ++item)
			{
				PathNode neighbour = *item;																					// For Readability

				if (visited.Find(neighbour.pos) == visited.list.end())														//if not in visited
				{
					if (App->pathfinding->IsWalkable(neighbour.pos))
					{
						(*units)->GiveNewTargetTile(neighbour.pos);															// Fix targeting.

						units++;
					}

					if (App->pathfinding->IsWalkable(neighbour.pos) || App->pathfinding->IsOccupied(neighbour.pos))													// App->pathfinding->IsWalkable(neighbour.pos) || App->pathfinding->IsOccupied(neighbour.pos)
					{
						frontier.list.push_back(neighbour);
					}
				}
			}

			frontier.list.erase(current_node);
			neighbours.list.clear();
			visited.list.push_back((*current_node));
			current_node++;
		}
	}
}

/*iPoint PathFinding::FindNearbyPoint(iPoint pos)
{
	iPoint ret(0, 0);


	PathList frontier;
	PathList visited;

	PathNode origin_node(0, 0, pos, nullptr);

	frontier.list.insert(std::make_pair(0, origin_node));

	std::map<int, PathNode>::iterator current_node = frontier.list.begin();

	PathList neighbours;

	while (frontier.list.size())
	{
		current_node->second.FindWalkableAdjacents(neighbours);																//Fill starting node

		std::map<int, PathNode>::iterator item = neighbours.list.begin();

		for (; item != neighbours.list.end(); ++item)
		{
			PathNode neighbour = item->second;																				// For Readability

			if (visited.Find(neighbour.pos) == nullptr)																		//if not in visited
			{
				if (App->pathfinding->IsWalkable(neighbour.pos))
				{
					ret = neighbour.pos;
					return ret;
				}

				if (App->pathfinding->IsWalkable(neighbour.pos) || App->pathfinding->IsOccupied(neighbour.pos))
				{
					frontier.list.insert(std::make_pair((neighbour.g + neighbour.h), neighbour));
				}
			}
		}

		neighbours.list.clear();

		visited.list.insert(std::make_pair((current_node->second.g + current_node->second.h), current_node->second));

		current_node++;
	}

	return ret;
}

void PathFinding::FindNearbyWalkable(const iPoint& pos, std::vector<Dynamic_Object*> units_selected)
{
	if (units_selected.size() != 0)
	{
		std::vector<Dynamic_Object*>::iterator units = units_selected.begin();

		if ((*units)->target == nullptr)
		{
			if (!(*units)->GiveNewTargetTile(pos))
			{
				return;
			}

			units++;
		}

		PathList frontier;
		PathList visited;

		PathNode origin_node(0, 0, pos, nullptr);

		frontier.list.insert(std::make_pair(0, origin_node));

		std::map<int, PathNode>::iterator current_node = frontier.list.begin();

		PathList neighbours;

		while (frontier.list.size() != 0 && units != units_selected.end())
		{
			current_node->second.FindWalkableAdjacents(neighbours);															//Fill starting node

			std::map<int, PathNode>::iterator item = neighbours.list.begin();

			for (; item != neighbours.list.end() && units != units_selected.end(); ++item)
			{
				PathNode neighbour = item->second;																					// For Readability

				if (visited.Find(neighbour.pos) == nullptr)																	//if not in visited
				{
					if (App->pathfinding->IsWalkable(neighbour.pos))
					{
						(*units)->GiveNewTargetTile(neighbour.pos);															// Fix targeting.

						units++;
					}

					if (App->pathfinding->IsWalkable(neighbour.pos) || App->pathfinding->IsOccupied(neighbour.pos))
					{
						frontier.list.insert(std::make_pair((neighbour.g + neighbour.h), neighbour));
					}
				}
			}

			neighbours.list.clear();

			visited.list.insert(std::make_pair((current_node->second.g + current_node->second.h), current_node->second));

			current_node++;
		}
	}
}*/

// Looks for a node in this list and returns it's list node or NULL
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

/*PathNode* PathList::Find(const iPoint& point)
{
	std::map<int, PathNode>::iterator item = list.begin();

	for (; item != list.end(); ++item)
	{
		if (item->second.pos == point)
		{
			return &item->second;
		}
	}

	return nullptr;
}*/

// Returns the Pathnode with lowest score in this list or NULL if empty
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

// Convenient constructors
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// Fills a list (PathList) of all valid adjacent pathnodes
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;

	uint before = list_to_fill.list.size();

	//A magic number is given as the F (g + h) because two elements with the same key cannot coexist in the same std::map.

	// north
	cell.create(pos.x, pos.y - 1);
	if (NodeIsAccessible(cell))
	{
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

		//list_to_fill.list.insert(std::make_pair( 1, PathNode(-1, -1, cell, this)));
	}

	// south
	cell.create(pos.x, pos.y + 1);
	if (NodeIsAccessible(cell))
	{
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

		//list_to_fill.list.insert(std::make_pair(2, PathNode(-1, -1, cell, this)));
	}

	// east
	cell.create(pos.x + 1, pos.y);
	if (NodeIsAccessible(cell))
	{
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

		//list_to_fill.list.insert(std::make_pair(3, PathNode(-1, -1, cell, this)));
	}

	// west
	cell.create(pos.x - 1, pos.y);
	if (NodeIsAccessible(cell))
	{
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

		//list_to_fill.list.insert(std::make_pair(4, PathNode(-1, -1, cell, this)));
	}

	// North east
	cell.create(pos.x + 1, pos.y - 1);
	if (NodeIsAccessible(cell))
	{
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

		//list_to_fill.list.insert(std::make_pair(5, PathNode(-1, -1, cell, this)));
	}

	// North west
	cell.create(pos.x - 1, pos.y - 1);
	if (NodeIsAccessible(cell))
	{
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

		//list_to_fill.list.insert(std::make_pair(6, PathNode(-1, -1, cell, this)));
	}

	// South east
	cell.create(pos.x + 1, pos.y + 1);
	if (NodeIsAccessible(cell))
	{
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

		//list_to_fill.list.insert(std::make_pair(7, PathNode(-1, -1, cell, this)));
	}

	// South west
	cell.create(pos.x - 1, pos.y + 1);
	if (NodeIsAccessible(cell))
	{
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

		//list_to_fill.list.insert(std::make_pair(8, PathNode(-1, -1, cell, this)));
	}

	//LOG("Assigned Parent: Pos: %d, %d", this->pos.x, this->pos.y);

	return list_to_fill.list.size();
}

bool PathNode::NodeIsAccessible(const iPoint& pos) const
{
	return (App->pathfinding->IsWalkable(pos) || App->pathfinding->IsOccupied(pos));
}

// Calculates this tile score
int PathNode::Score() const
{
	return g + h;
}

// Calculate the F for a specific destination tile
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;

	int distance_X = abs(pos.x - destination.x);
	int distance_Y = abs(pos.y - destination.y);

	h = (distance_X + distance_Y) * min(distance_X, distance_Y);

	return g + h;
 }

// Actual A* algorithm: return number of steps in the creation of the path or -1 
int PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	BROFILER_CATEGORY("CreatePath", Profiler::Color::SlateGray)
	
	int ret = -1;		//Revise, Should be 1?										            //The value returned by the function. Done to improve readability.
																					            
	if (IsNonWalkable(origin) || IsNonWalkable(destination) || IsOccupied(destination))			//IsWalkable() checks if origin and destination are walkable tiles. IsWalkable calls GetTile(), which returns the walkability value only if the given tile is inside the map's boundaries.
	{																				            
		return ret;															            
	}																				            
																					            
	PathList open;																	            //Declares/Creates the open list (frontier queue).
	PathList closed;																            //Declares/Creates the closed list (visited list).

	PathNode origin_node(0, 0, origin, nullptr);									            //Declares/Creates a node that will store the data of the origin tile. G, H, position and parent are passed as arguments. 

	open.list.push_back(origin_node);												            //Adds the current node storing the data of the origin tile to the open (frontier) list.
																					            
	while (open.list.size() != 0)								            					//While the list is not empty. If the count is higher than 0 that means the list is not empty.
	{
		std::list<PathNode>::iterator lowest_node = open.GetNodeLowestScore();

		closed.list.push_back((*lowest_node));

		std::list<PathNode>::iterator current_node = prev(closed.list.end());					//As lowest_node will be pushed to the back of the vector, it will always be at the last position.

		open.list.erase(lowest_node);												            //Deletes from the open (frontier) list the node with the lowest score, as it has been the one chosen to be moved to.

		if (current_node->pos == destination)			            							//If destination is in the closed list (visited) and the position of the current node is the same as destination's.
		{																			            
			last_path.clear();														            //Sets the last_path dynArray count of number of elements to 0. Clears the dynArray.																	            
			
			PathNode last_node = (*current_node);
			const PathNode* path_node = &last_node;

			for (; path_node != nullptr; path_node = path_node->parent)
			{
				last_path.push_back(path_node->pos);
			}

			std::reverse(last_path.begin(), last_path.end());						            //Flips the vector's elements. The first element of the array will be the origin tile and the destination tile the last one.
			ret = last_path.size();													            //Returns the amount of steps the path has.

			break;
		}

		PathList neighbours;																	//Declares a list that will store the Walkable Adjacent nodes of a given node (current_node).
		
		current_node->FindWalkableAdjacents(neighbours);										//Fills the neighbours list with the walkable adjacent nodes of current_node.

		std::list<PathNode>::iterator neighbour_item = neighbours.list.begin();				//Declares a list item pointer that will iterate the neighbours list.

		for (; neighbour_item != neighbours.list.end(); ++neighbour_item)
		{
			PathNode neighbour = *neighbour_item;
			
			if (closed.Find(neighbour.pos) == closed.list.end())								//If the neighbour being iterated is not in the closed list (.Find() returns NULL when the item requested is not found).
			{
				if (open.Find(neighbour.pos) != open.list.end())
				{
					neighbour.CalculateF(destination);											//Calculates the F (F = G + H) of the neighbour being iterated. As G is recalculated (taking into account this new path), it can be compared with the same node in the open list (old path), if it's in it.

					if ((neighbour.g) < (open.Find(neighbour.pos)->g))							//Compares Gs (total flat movement cost) between the neigbour being iterated and the same neighbour in the list.
					{
						open.Find(neighbour.pos)->parent = neighbour.parent;					//Updates the parent of the neighbour in the list with the parent of the neighbour being iterated.
					}
				}
				else
				{
					neighbour.CalculateF(destination);											//Calculates the F (F = G + H) of the neighbour being iterated. Sets both G and H for this tile/node for a specific path.
					open.list.push_back(neighbour);												//Adds the neighbour being iterated to the open list.
				}
			}
		}

		neighbours.list.clear();																//Clears the neighbours list so the elements are not accumulated from node to node (tile to tile).

		if (closed.list.size() > 500)
		{
			ret = -2;
			return ret;
		}
	}

	return ret;
}


// --- LIST TO MAP CREATE PATH
/*int PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	BROFILER_CATEGORY("CreatePath", Profiler::Color::SlateGray)

		int ret = -1;																			//The value returned by the function. Done to improve readability.

	if (IsNonWalkable(origin) || IsNonWalkable(destination) || IsOccupied(destination))			//IsWalkable() checks if origin and destination are walkable tiles. IsWalkable calls GetTile(), which returns the walkability value only if the given tile is inside the map's boundaries.
	{
		return ret;
	}

	PathList open;																	            //Declares/Creates the open list (frontier queue).
	PathList closed;																            //Declares/Creates the closed list (visited list).

	PathNode lowest_node;
	PathNode current_node;
	//PathNode* current_node;

	PathNode origin_node(0, 0, origin, nullptr);									            //Declares/Creates a node that will store the data of the origin tile. G, H, position and parent are passed as arguments. 

	open.list.insert(std::make_pair(0, origin_node));											//Adds the current node storing the data of the origin tile to the open (frontier) list.

	while (open.list.size() != 0)								            					//While the list is not empty. If the count is higher than 0 that means the list is not empty.
	{	
		PathNode lowest_node = open.list.begin()->second;										// First element will always have the lowest F (?)
		
		PathNode current_node = lowest_node;
		
		closed.list.insert(std::make_pair((lowest_node.g + lowest_node.h), lowest_node));
		
		//PathNode current_node = prev(closed.list.end())->second;

		open.list.erase(open.list.begin());

		if (current_node.pos == destination)			            							//If destination is in the closed list (visited) and the position of the current node is the same as destination's.
		{
			last_path.clear();														            //Sets the last_path dynArray count of number of elements to 0. Clears the dynArray.
			last_path.shrink_to_fit();

			const PathNode* path_node = &current_node;

			for (; path_node != nullptr; path_node = path_node->parent)
			{
				last_path.push_back(path_node->pos);
			}

			std::reverse(last_path.begin(), last_path.end());						            //Flips the vector's elements. The first element of the array will be the origin tile and the destination tile the last one.
			ret = last_path.size();													            //Returns the amount of steps the path has.

			break;
		}

		PathList neighbours;																	//Declares a list that will store the Walkable Adjacent nodes of a given node (current_node).

		current_node.FindWalkableAdjacents(neighbours);											//Fills the neighbours list with the walkable adjacent nodes of current_node.

		std::map<int, PathNode>::iterator neighbour_item = neighbours.list.begin();

		for (; neighbour_item != neighbours.list.end(); ++neighbour_item)
		{
			PathNode neighbour = neighbour_item->second;

			if (closed.Find(neighbour.pos) == nullptr)											//If the neighbour being iterated is not in the closed list (.Find() returns NULL when the item requested is not found).
			{
				if (open.Find(neighbour.pos) != nullptr)										//If the neighbour being iterated is already in the open list.
				{
					neighbour.CalculateF(destination);											//Calculates the F (F = G + H) of the neighbour being iterated. As G is recalculated (taking into account this new path), it can be compared with the same node in the open list (old path), if it's in it.

					if ((neighbour.g) < (open.Find(neighbour.pos)->g))							//Compares Gs (total flat movement cost) between the neigbour being iterated and the same neighbour in the list.
					{
						open.Find(neighbour.pos)->parent = neighbour.parent;					//Updates the parent of the neighbour in the list with the parent of the neighbour being iterated.
					}
				}
				else
				{
					neighbour.CalculateF(destination);											//Calculates the F (F = G + H) of the neighbour being iterated. Sets both G and H for this tile/node for a specific path.
					open.list.insert(std::make_pair((neighbour.g + neighbour.h), neighbour));	//Adds the neighbour being iterated to the open list.
					//open.list[neighbour.g + neighbour.h] = neighbour;							//Adds the neighbour being iterated to the open list.
				}
			}
		}

		// --- LIST TO MAP V.02 ---
		//lowest_node = open.list.begin()->second;

		//closed.list[lowest_node.g + lowest_node.h] = lowest_node;
		////closed.list.insert(std::make_pair((lowest_node.g + lowest_node.h), lowest_node));

		//current_node = &closed.list[lowest_node.g + lowest_node.h];

		//open.list.erase(open.list.begin());

		//if (current_node->pos == destination)			            							//If destination is in the closed list (visited) and the position of the current node is the same as destination's.
		//{
		//	last_path.clear();														            //Sets the last_path dynArray count of number of elements to 0. Clears the dynArray.
		//	last_path.shrink_to_fit();

		//	const PathNode* path_node = current_node;

		//	for (; path_node != nullptr; path_node = path_node->parent)
		//	{
		//		last_path.push_back(path_node->pos);
		//	}

		//	std::reverse(last_path.begin(), last_path.end());						            //Flips the vector's elements. The first element of the array will be the origin tile and the destination tile the last one.
		//	ret = last_path.size();													            //Returns the amount of steps the path has.

		//	break;
		//}

		//PathList neighbours;																	//Declares a list that will store the Walkable Adjacent nodes of a given node (current_node).

		//current_node->FindWalkableAdjacents(neighbours);										//Fills the neighbours list with the walkable adjacent nodes of current_node.

		//std::map<int, PathNode>::iterator neighbour_item = neighbours.list.begin();

		//for (; neighbour_item != neighbours.list.end(); ++neighbour_item)
		//{
		//	PathNode neighbour = neighbour_item->second;

		//	if (closed.Find(neighbour.pos) == nullptr)											//If the neighbour being iterated is not in the closed list (.Find() returns NULL when the item requested is not found).
		//	{
		//		if (open.Find(neighbour.pos) != nullptr)										//If the neighbour being iterated is already in the open list.
		//		{
		//			neighbour.CalculateF(destination);											//Calculates the F (F = G + H) of the neighbour being iterated. As G is recalculated (taking into account this new path), it can be compared with the same node in the open list (old path), if it's in it.

		//			if ((neighbour.g) < (open.Find(neighbour.pos)->g))							//Compares Gs (total flat movement cost) between the neigbour being iterated and the same neighbour in the list.
		//			{
		//				open.Find(neighbour.pos)->parent = neighbour.parent;					//Updates the parent of the neighbour in the list with the parent of the neighbour being iterated.
		//			}
		//		}
		//		else
		//		{
		//			neighbour.CalculateF(destination);											//Calculates the F (F = G + H) of the neighbour being iterated. Sets both G and H for this tile/node for a specific path.
		//			open.list.insert(std::make_pair((neighbour.g + neighbour.h), neighbour));	//Adds the neighbour being iterated to the open list.
		//			//open.list[neighbour.g + neighbour.h] = neighbour;							//Adds the neighbour being iterated to the open list.
		//		}
		//	}
		//}

		neighbours.list.clear();																//Clears the neighbours list so the elements are not accumulated from node to node (tile to tile).

		if (closed.list.size() > 500)
		{
			ret = -2;
			return ret;
		}
	}

	return ret;
}*/
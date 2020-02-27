#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"
#include "Brofiler\Brofiler.h"

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name.create("pathfinding");
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.Clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;				//Revise. Should erase?
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const p2DynArray<iPoint>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	p2List_item<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	p2List_item<PathNode>* ret = NULL;
	int min = 65535;

	p2List_item<PathNode>* item = list.end;
	while (item)
	{
		if (item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
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
	uint before = list_to_fill.list.count();

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// North east
	cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// North west
	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// South east
	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// South west
	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	return list_to_fill.list.count();
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
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	BROFILER_CATEGORY("CreatePath", Profiler::Color::SlateGray)
	
	int ret = -1;		//Revise, Should be 1?															//The value returned by the function. Done to improve readability.
	
	if (IsWalkable(origin) == false || IsWalkable(destination) == false)	//IsWalkable() checks if origin and destination are walkable tiles. IsWalkable calls GetTile(), which returns the walkability value only if the given tile is inside the map's boundaries.
	{
		return ret = -1;
	}

	PathList open;																		//Declares/Creates the open list (frontier queue).
	PathList closed;																	//Declares/Creates the closed list (visited list).

	PathNode origin_node(0, 0, origin, NULL);											//Declares/Creates a node that will store the data of the origin tile. G, H, position and parent are passed as arguments. 

	open.list.add(origin_node);															//Adds the current node storing the data of the origin tile to the open (frontier) list.

	while (open.list.count() != 0)														//While the list is not empty. If the count is higher than 0 that means the list is not empty.
	{
		p2List_item<PathNode>* lowestNode = open.GetNodeLowestScore();					//Gets the node with the lowest score (F = G + H) in the open list. Coded like this to improve readability. 
		p2List_item<PathNode>* current_node = closed.list.add(lowestNode->data);		//Assigns current_node the data members of the node/tile with the lowest score. Done to improve readability.

		open.list.del(lowestNode);														//Deletes from the open (frontier) list the node with the lowest score, as it has been the one chosen to be moved to.

		if (current_node->data.pos == destination)										//If destination is in the closed list (visited) and the position of the current node is the same as destination's.
		{
			last_path.Clear();															//Sets the last_path dynArray count of number of elements to 0. Clears the dynArray.

			const PathNode* path_node = &current_node->data;							//Declares a node with the data members of the current_node (current position, parent, cost...). Improves readability.

			while (path_node != NULL)													//While path_node is not NULL (path_node contains data)
			{
				last_path.PushBack(path_node->pos);										//Adds to last_path dynArray the current path_node tile (position data members).
				//LOG("Path_node at: (%d, %d)", path_node->pos.x, path_node->pos.y);
				path_node = path_node->parent;											//Sets the data members of the current path_node as the data members of the parent node. (Backtracks one node/tile)
			}

			last_path.Flip();															//Flips the dynArray members so the first element of the array is the origin tile and the destination tile the last.
			ret = last_path.Count();													//Returns the amount of steps the path has.
			//LOG("Number of steps: %d", last_path.Count());
			break;
		}

		PathList neighbours;															//Declares a list that will store the Walkable Adjacent nodes of a given node (current_node).
		current_node->data.FindWalkableAdjacents(neighbours);							//Fills the neighbours list with the walkable adjacent nodes of current_node.

		p2List_item<PathNode>* neighbour_iterator = neighbours.list.start;											//Declares a list item pointer that will iterate the neighbours list.

		while (neighbour_iterator != NULL)																			//If neighbour_iterator pointer is not NULL.
		{
			if (closed.Find(neighbour_iterator->data.pos) == NULL)													//If the neighbour being iterated is not in the closed list (.Find() returns NULL when the item requested is not found).
			{
				if (open.Find(neighbour_iterator->data.pos) != NULL)												//If the neighbour being iterated is already in the open list.
				{
					neighbour_iterator->data.CalculateF(destination);												//Calculates the F (F = G + H) of the neighbour being iterated. As G is recalculated (taking into account this new path), it can be compared with the same node in the open list (old path), if it's in it.

					if (neighbour_iterator->data.g < open.Find(neighbour_iterator->data.pos)->data.g)				//Compares Gs (total flat movement cost) between the neigbour being iterated and the same neighbour in the list.
					{
						open.Find(neighbour_iterator->data.pos)->data.parent = neighbour_iterator->data.parent;		//Updates the parent of the neighbour in the list with the parent of the neighbour being iterated. 
					}
				}
				else
				{
					neighbour_iterator->data.CalculateF(destination);												//Calculates the F (F = G + H) of the neighbour being iterated. Sets both G and H for this tile/node for a specific path.
					open.list.add(neighbour_iterator->data);														//Adds the neighbour being iterated to the open list.
				}
			}

			neighbour_iterator = neighbour_iterator->next;															//Iterates the list. Advances to the next node/element in the list.
		}
		neighbours.list.clear();																					//Clears the neighbours list so the elements are not accumulated from node to node (tile to tile).
	}

	return ret;
}

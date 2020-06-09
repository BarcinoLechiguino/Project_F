#ifndef __QUADTREE__H_
#define __QUADTREE__H_

#include "Dependencies/SDL/include/SDL_rect.h"
#include "Definitions.h"
#include "Point.h"

class QuadTree
{
public:
	QuadTree(SDL_Rect  boundary, uint level, uint max_levels);
	~QuadTree();
	
	bool Intersects(SDL_Rect& rect);	//Weather or not a given rectangle intersects with current node's boundary
	bool ContainsPoint(iPoint point);	//Weather or not a point is inside node's boundary

public:
	bool divided;		//Divided is a boolean which defines weather the current node is divided in 4 already or not.

	uint max_levels;	//Max_levels is an integer which defines the amount of times a quadtree can subdivide and level is the current level the tree is in.
	uint level;			//Boundary is the starting rectangle for the tree.In our case it should be the size of the map.

	SDL_Rect boundary;	//Next up is the children quadtree used for the tiles to draw the map.
};
#endif // !__QUADTREE__H_
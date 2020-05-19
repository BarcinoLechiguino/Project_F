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
	
	bool Intersects(SDL_Rect& rect);

	bool ContainsPoint(iPoint point);

public:
	
	bool divided;

	uint max_levels;
	uint level;

	SDL_Rect boundary;

	

};

#endif // !__QUADTREE__H_
#ifndef __TILEQUADTREE__H_
#define __TILEQUADTREE__H_

#include "Quadtree.h"
#include <vector>
#include "Point.h"

struct TileData
{
	TileData(uint id, iPoint position) : id(id), position(position) {};

	uint id;
	iPoint position;
};

class TileQuadTree : public QuadTree
{
public:

	TileQuadTree(SDL_Rect quadtree, uint level, uint max_levels);

	void Subdivide();

	void InsertTile(iPoint position, uint id);

	void DrawQuadtree();

public:

	std::vector<TileData> tiles;

	uint max_tiles; //Max tiles in tree

	TileQuadTree* northWest;
	TileQuadTree* northEast;
	TileQuadTree* southWest;
	TileQuadTree* southEast;

	int i = 0;
};

#endif // !__TILEQUADTREE__H_
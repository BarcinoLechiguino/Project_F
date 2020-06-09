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

	void Subdivide();							//Divide node's space in 4 subnodes
	void InsertTile(iPoint position, uint id);	//Insert a tile in corresponding node
	void DrawQuadtree();						//Draw lines of the quadtree for debug
	
public:

	std::vector<TileData> tiles; //Tiles inside this node

	uint max_tiles; //Max tiles in node

	//Quadtree childs
	TileQuadTree* northWest;
	TileQuadTree* northEast;
	TileQuadTree* southWest;
	TileQuadTree* southEast;
};

#endif // !__TILEQUADTREE__H_
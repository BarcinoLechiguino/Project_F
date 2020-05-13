#ifndef __ENTITYQUADTREE__H_
#define __ENTITYQUADTREE__H_

#include "Quadtree.h"
#include <vector>
#include "Point.h"
class Entity;

class EntityQuadTree : public QuadTree
{
public:

	EntityQuadTree(SDL_Rect quadtree, uint level, uint max_levels);

	void Subdivide();

	void InsertEntity(Entity* entity);

	void DrawQuadtree();

	std::vector<Entity*> QueryEntities(SDL_Rect section);

public:

	std::vector<Entity*> entities;

	uint max_entities; //Max tiles in tree

	EntityQuadTree* northWest;
	EntityQuadTree* northEast;
	EntityQuadTree* southWest;
	EntityQuadTree* southEast;

	int i = 0;
};

#endif // !__ENTITYQUADTREE__H_
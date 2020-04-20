#include "Quadtree.h"
#include "Point.h"

QuadTree::QuadTree(SDL_Rect boundary, uint level, uint max_levels) : boundary(boundary),level(level),max_levels(max_levels)
{
	divided = false;

}

QuadTree::~QuadTree()
{
	
}

bool QuadTree::Intersects(SDL_Rect& rect)
{
	return !(rect.x + rect.w <= boundary.x ||
		rect.x >= boundary.x + boundary.w ||
		rect.y >= boundary.y + boundary.h ||
		rect.y + rect.h <= boundary.y);
}

bool QuadTree::ContainsPoint(iPoint point)
{
	return(point.x >= boundary.x &&
		point.x <= boundary.x + boundary.w &&
		point.y >= boundary.y &&
		point.y <= boundary.y + boundary.h);
}
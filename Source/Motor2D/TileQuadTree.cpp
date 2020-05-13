#include "TileQuadTree.h"
#include "Quadtree.h"
#include "Window.h"
#include "Application.h"
#include "Render.h"

TileQuadTree::TileQuadTree(SDL_Rect quadtree, uint level, uint max_levels) : QuadTree(quadtree,level,max_levels)
{
	divided = false;
	boundary = quadtree;

	max_tiles = 4;

	northWest = nullptr;
	northEast = nullptr;
	southWest = nullptr;
	southEast = nullptr;
}

void TileQuadTree::Subdivide()
{
	if (level < max_levels && !divided)
	{
		northWest = new TileQuadTree(SDL_Rect{ boundary.x , boundary.y , boundary.w / 2 , boundary.h / 2 }, level +1, max_levels);
		northEast = new TileQuadTree(SDL_Rect{ boundary.x + boundary.w / 2 , boundary.y , boundary.w / 2 , boundary.h / 2 }, level +1, max_levels);
		southWest = new TileQuadTree(SDL_Rect{ boundary.x , boundary.y + boundary.h / 2 , boundary.w / 2 , boundary.h / 2 }, level +1, max_levels);
		southEast = new TileQuadTree(SDL_Rect{ boundary.x + boundary.w / 2 , boundary.y + boundary.h / 2 , boundary.w / 2 , boundary.h / 2 }, level +1, max_levels);

		divided = true;
	}
}

void TileQuadTree::InsertTile(iPoint position, uint id)
{

	if (!ContainsPoint(position))
	{
		return;
	}

	if(tiles.size() < max_tiles && !divided)
	{
		tiles.push_back(TileData(id, position));
		return;
	}

	if (level == max_levels)
	{
		tiles.push_back(TileData(id, position));
		return;
	}

	if (!divided)
	{
		Subdivide();
	}

	northWest->InsertTile(position, id);
	northEast->InsertTile(position, id);
	southWest->InsertTile(position, id);
	southEast->InsertTile(position, id);

}

void TileQuadTree::DrawQuadtree()
{
	int screen_w;
	int screen_h;
	App->win->GetWindowSize(screen_w, screen_h);

	if (Intersects(SDL_Rect{ -App->render->camera.x,-App->render->camera.y ,screen_w, screen_h } ) )
	{
		if (level == 0)
		{
			App->render->DrawLine(boundary.x, boundary.y, boundary.x, boundary.y + boundary.h, 255, 255, 255);
			App->render->DrawLine(boundary.x, boundary.y, boundary.x + boundary.w, boundary.y, 255, 255, 255);
			App->render->DrawLine(boundary.x, boundary.y + boundary.h, boundary.x + boundary.w, boundary.y + boundary.h, 255, 255, 255);
			App->render->DrawLine(boundary.x + boundary.w, boundary.y, boundary.x + boundary.w, boundary.y + boundary.h, 255, 255, 255);
		}

		if (divided)
		{
			App->render->DrawLine(boundary.x + boundary.w / 2, boundary.y, boundary.x + boundary.w / 2, boundary.y + boundary.h, 255, 255, 255);
			App->render->DrawLine(boundary.x, boundary.y + boundary.h / 2, boundary.x + boundary.w, boundary.y + boundary.h / 2, 255, 255, 255);

			if (level < max_levels )
			{
				northWest->DrawQuadtree();
				northEast->DrawQuadtree();
				southWest->DrawQuadtree();
				southEast->DrawQuadtree();
			}
		}
	}
}
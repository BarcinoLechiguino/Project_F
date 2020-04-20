#include "EntityQuadTree.h"
#include "Quadtree.h"
#include "Window.h"
#include "Application.h"
#include "Render.h"
#include "Entity.h"

#include "Brofiler\Brofiler.h"

EntityQuadTree::EntityQuadTree(SDL_Rect quadtree, uint level, uint max_levels) : QuadTree(quadtree, level, max_levels)
{
	divided = false;
	boundary = quadtree;

	max_entities = 4;

	northWest = nullptr;
	northEast = nullptr;
	southWest = nullptr;
	southEast = nullptr;
}

void EntityQuadTree::Subdivide()
{
	if (level < max_levels && !divided)
	{
		northWest = new EntityQuadTree(SDL_Rect{ boundary.x , boundary.y , boundary.w / 2 , boundary.h / 2 }, level + 1, max_levels);
		northEast = new EntityQuadTree(SDL_Rect{ boundary.x + boundary.w / 2 , boundary.y , boundary.w / 2 , boundary.h / 2 }, level + 1, max_levels);
		southWest = new EntityQuadTree(SDL_Rect{ boundary.x , boundary.y + boundary.h / 2 , boundary.w / 2 , boundary.h / 2 }, level + 1, max_levels);
		southEast = new EntityQuadTree(SDL_Rect{ boundary.x + boundary.w / 2 , boundary.y + boundary.h / 2 , boundary.w / 2 , boundary.h / 2 }, level + 1, max_levels);

		divided = true;
	}
}

void EntityQuadTree::InsertEntity(Entity* entity)
{

	if (!ContainsPoint(iPoint(entity->pixel_position.x, entity->pixel_position.y) ) )
	{
		return;
	}

	if (entities.size() < max_entities && !divided)
	{
		entities.push_back(entity);
		return;
	}

	if (level == max_levels)
	{
		entities.push_back(entity);
		return;
	}

	if (!divided)
	{
		Subdivide();
	}

	northWest->InsertEntity(entity);
	northEast->InsertEntity(entity);
	southWest->InsertEntity(entity);
	southEast->InsertEntity(entity);

}

void EntityQuadTree::DrawQuadtree()
{
	BROFILER_CATEGORY("Draw Entity Quadtree", Profiler::Color::FireBrick);

	int screen_w;
	int screen_h;
	App->win->GetWindowSize(screen_w, screen_h);

	if (Intersects(SDL_Rect{ -App->render->camera.x,-App->render->camera.y ,screen_w, screen_h }))
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

			if (level < max_levels)
			{
				northWest->DrawQuadtree();
				northEast->DrawQuadtree();
				southWest->DrawQuadtree();
				southEast->DrawQuadtree();
			}
		}
	}
}

std::vector<Entity*> EntityQuadTree::QueryEntities(SDL_Rect section)
{
	std::vector<Entity*> found;

	if (!Intersects(section))
	{

	}


	return found;
}
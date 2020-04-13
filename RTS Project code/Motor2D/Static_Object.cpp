#include "Application.h"
#include "Map.h"
#include "Pathfinding.h"
#include "EntityManager.h"

#include "Static_Object.h"


Static_Object::Static_Object(int x, int y, ENTITY_TYPE type) : Entity(x, y, type)
{
	
}

bool Static_Object::Awake(pugi::xml_node&)
{
	return true;
}

bool Static_Object::Start()
{

	for (int x_tile = tile_position.x; x_tile < tile_position.x + tiles_occupied_x; ++x_tile)
	{
		for (int y_tile = tile_position.y; y_tile < tile_position.y + tiles_occupied_y; ++y_tile)
		{
			App->pathfinding->ChangeWalkability(iPoint(x_tile, y_tile), 0);
		}
	}

	return true;
}

bool Static_Object::PreUpdate()
{
	

	return true;
}

bool Static_Object::Update(float dt, bool doLogic)
{
	return true;
}

bool Static_Object::PostUpdate()
{
	return true;
}

bool Static_Object::CleanUp()
{
	return true;
}
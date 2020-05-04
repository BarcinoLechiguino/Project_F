#include "Application.h"
#include "Map.h"
#include "Pathfinding.h"
#include "EntityManager.h"

#include "StaticObject.h"


StaticObject::StaticObject(int x, int y, ENTITY_TYPE type, int level) : Entity(x, y, type, level)
{
	
}

bool StaticObject::Awake(pugi::xml_node&)
{
	return true;
}

bool StaticObject::Start()
{
	App->pathfinding->ChangeWalkability(tile_position, this, NON_WALKABLE);
	
	//for (int x_tile = tile_position.x; x_tile < tile_position.x + tiles_occupied_x; ++x_tile)
	//{
	//	for (int y_tile = tile_position.y; y_tile < tile_position.y + tiles_occupied_y; ++y_tile)
	//	{
	//		
	//		//App->entity_manager->ChangeEntityMap(iPoint(x_tile, y_tile), this);						//Adds the generated entity to entity_map.
	//	}
	//}

	return true;
}

bool StaticObject::PreUpdate()
{
	

	return true;
}

bool StaticObject::Update(float dt, bool doLogic)
{
	return true;
}

bool StaticObject::PostUpdate()
{
	return true;
}

bool StaticObject::CleanUp()
{
	return true;
}

void StaticObject::Draw()
{
	return;
}
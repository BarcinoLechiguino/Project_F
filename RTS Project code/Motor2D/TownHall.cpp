#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Textures.h"
#include "Pathfinding.h"
#include <vector>
#include "Point.h"

#include "EntityManager.h"

TownHall::TownHall(int x, int y, ENTITY_TYPE type) : Static_Object(x,y,type)
{
	entity_sprite = App->tex->Load("maps/town_hall_holder.png");
	
	pixel_position = App->map->MapToWorld(x, y);

	tiles_occupied_x = 3;
	tiles_occupied_y = 3;

	for (int x_tile = x ; x_tile < x + tiles_occupied_x ; ++x_tile)
	{
		for (int y_tile = y ; y_tile < y + tiles_occupied_y ; ++y_tile)
		{
			App->pathfinding->ChangeWalkability( iPoint(x_tile,y_tile) , 0);
		}
	}
}

bool TownHall::Awake(pugi::xml_node&)
{
	return true;
}

bool TownHall::Start()
{
	
	
	return true;
}

bool TownHall::PreUpdate()
{

	return true;
}

bool TownHall::Update(float dt, bool doLogic)
{
	if (!App->pathfinding->IsWalkable(iPoint(2, 2)) )
	{
		LOG("2 2 is unwalkable");
	}
	App->render->Blit(this->entity_sprite, pixel_position.x - 54, pixel_position.y, nullptr);

	return true;
}

bool TownHall::PostUpdate()
{
	return true;
}

bool TownHall::CleanUp()
{
	return true;
}
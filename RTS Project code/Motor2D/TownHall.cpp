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
	//entity_sprite = App->tex->Load("maps/town_hall_holder.png");
	entity_sprite = App->tex->Load("maps/hall.png");
	
	hall_rect = {0,0,155,138};

	pixel_position.x = App->map->MapToWorld(x, y).x;
	pixel_position.y = App->map->MapToWorld(x, y).y;

	tiles_occupied_x = 3;
	tiles_occupied_y = 3;

}

bool TownHall::Awake(pugi::xml_node&)
{
	return true;
}

bool TownHall::PreUpdate()
{

	return true;
}

bool TownHall::Update(float dt, bool doLogic)
{
	App->render->Blit(entity_sprite, pixel_position.x - 51, pixel_position.y - 20, &hall_rect);

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
#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Textures.h"
#include "Pathfinding.h"

#include "EntityManager.h"

Rock::Rock(int x, int y, ENTITY_TYPE type) : Static_Object(x,y,type)
{
	entity_sprite = App->tex->Load("maps/debug_tile.png");
	
	pixel_position.x = App->map->MapToWorld(x, y).x;
	pixel_position.y = App->map->MapToWorld(x, y).y;

	tiles_occupied_x = 1;
	tiles_occupied_y = 1;

	selection_collider = { (int)pixel_position.x + 20, (int)pixel_position.y + 20 , 35, 25 };

	ore = 20;

	gather_time = 1;
}

bool Rock::Awake(pugi::xml_node&)
{
	return true;
}


bool Rock::PreUpdate()
{
	return true;
}

bool Rock::Update(float dt, bool doLogic)
{
	App->render->Blit(entity_sprite, pixel_position.x, pixel_position.y, nullptr);

	return true;
}

bool Rock::PostUpdate()
{
	return true;
}

bool Rock::CleanUp()
{
	return true;
}
#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Textures.h"
#include "Pathfinding.h"

#include "EntityManager.h"

Barracks::Barracks(int x, int y, ENTITY_TYPE type) : Static_Object(x, y, type)
{
	entity_sprite = App->tex->Load("maps/debug_barracks_tile.png");

	pixel_position.x = App->map->MapToWorld(x, y).x;
	pixel_position.y = App->map->MapToWorld(x, y).y;

	tiles_occupied_x = 2;
	tiles_occupied_y = 2;
}

bool Barracks::Awake(pugi::xml_node&)
{
	return true;
}

bool Barracks::PreUpdate()
{

	return true;
}

bool Barracks::Update(float dt, bool doLogic)
{
	App->render->Blit(entity_sprite, pixel_position.x - 27, pixel_position.y, nullptr);

	return true;
}

bool Barracks::PostUpdate()
{
	return true;
}

bool Barracks::CleanUp()
{
	return true;
}
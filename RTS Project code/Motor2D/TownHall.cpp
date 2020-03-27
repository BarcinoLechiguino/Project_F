#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Textures.h"
#include "Pathfinding.h"

#include "EntityManager.h"

TownHall::TownHall(int x, int y, ENTITY_TYPE type) : Static_Object(x,y,type)
{
	entity_sprite = App->tex->Load("maps/debug_tile.png");
	
	pixel_position = App->map->MapToWorld(x, y);

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
	App->pathfinding->ChangeWalkability(tile_position, 1);

	return true;
}

bool TownHall::Update(float dt, bool doLogic)
{
	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y, nullptr);

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
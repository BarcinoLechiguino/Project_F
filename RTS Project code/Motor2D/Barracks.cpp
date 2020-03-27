#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Textures.h"
#include "Pathfinding.h"

#include "EntityManager.h"

Barracks::Barracks(int x, int y, ENTITY_TYPE type) : Static_Object(x, y, type)
{
	entity_sprite = App->tex->Load("maps/debug_tile.png");

	pixel_position = App->map->MapToWorld(x, y);

}

bool Barracks::Awake(pugi::xml_node&)
{
	return true;
}

bool Barracks::Start()
{
	return true;
}

bool Barracks::PreUpdate()
{
	App->pathfinding->ChangeWalkability(tile_position, 1);

	return true;
}

bool Barracks::Update(float dt, bool doLogic)
{
	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y, nullptr);

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
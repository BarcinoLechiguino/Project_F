#include "Rock.h"
#include "App.h"
#include "Render.h"
#include "Map.h"
#include "Textures.h"
#include "j1Pathfinding.h"


Rock::Rock(int x, int y, ENTITY_TYPE type) : Static_Object(x,y,type)
{
	entity_sprite = App->tex->Load("maps/debug_tile.png");
	
	pixel_position = App->map->MapToWorld(x, y);

}

bool Rock::Awake(pugi::xml_node&)
{
	return true;
}

bool Rock::Start()
{
	return true;
}

bool Rock::PreUpdate()
{
	App->pathfinding->ChangeWalkability(tile_position, 1);

	return true;
}

bool Rock::Update(float dt, bool doLogic)
{
	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y, nullptr);

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
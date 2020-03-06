#include "j1Rock.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Textures.h"


j1Rock::j1Rock(int x, int y, ENTITY_TYPE type) : j1Static_Object(x,y,type)
{
	entity_sprite = App->tex->Load("maps/debug_tile.png");
	
	pixel_position = App->map->MapToWorld(x, y);
}

bool j1Rock::Awake(pugi::xml_node&)
{
	return true;
}

bool j1Rock::Start()
{
	return true;
}

bool j1Rock::PreUpdate()
{
	return true;
}

bool j1Rock::Update(float dt, bool doLogic)
{
	App->render->Blit(this->entity_sprite, pixel_position.x,pixel_position.y,nullptr);

	return true;
}

bool j1Rock::PostUpdate()
{
	return true;
}

bool j1Rock::CleanUp()
{
	return true;
}
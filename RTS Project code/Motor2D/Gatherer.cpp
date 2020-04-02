#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Input.h"
#include "Pathfinding.h"

#include "EntityManager.h"

Gatherer::Gatherer(int x, int y, ENTITY_TYPE type) : Dynamic_Object(x, y, type)
{
	entity_sprite = App->tex->Load("maps/debug_gatherer_tile.png");

	

	selectable_unit = true;
	path_full = false;

	speed = 500;
}

Gatherer::~Gatherer()
{

}

bool Gatherer::Awake(pugi::xml_node&)
{
	return true;
}

bool Gatherer::Start()
{
	return true;
}

bool Gatherer::PreUpdate()
{
	return true;
}

bool Gatherer::Update(float dt, bool doLogic)
{
	//MoveInput();

	/*if ( path_full )
	{
		target_tile = entity_path.back();
	}*/

	//LOG("target_tile %d %d", target_tile.x, target_tile.y);

	HandleMovement(dt);

	selection_collider.x = pixel_position.x + 10;
	selection_collider.y = pixel_position.y + 10;

	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y, nullptr);

	App->render->DrawQuad(selection_collider, 255, 255, 0, 100);

	return true;
}

bool Gatherer::PostUpdate()
{
	return true;
}

bool Gatherer::CleanUp()
{
	return true;
}

void Gatherer::OnCollision(Collider* C1, Collider* C2)
{

}
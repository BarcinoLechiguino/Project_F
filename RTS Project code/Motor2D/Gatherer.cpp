#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Input.h"
#include "Pathfinding.h"

#include "EntityManager.h"

Gatherer::Gatherer(int x, int y, ENTITY_TYPE type) : Dynamic_Object(x, y, type)
{
	entity_sprite = App->tex->Load("textures/Spritesheets/Entities/gatherer2_lowres.png");

	entity_sprite_section = { 52,0,52,49 };

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

	//change section according to pathing. 
	switch (this->unit_state) {
	case entity_state::PATHING_DOWN:
		entity_sprite_section = { 39,49,38,45 };
		break;
	case entity_state::PATHING_RIGHT:
		entity_sprite_section = { 123,49,43,42 };
		break;
	case entity_state::PATHING_LEFT:
		entity_sprite_section = { 78,49,43,42 };
		break;
	case entity_state::PATHING_UP:
		entity_sprite_section = { 0,49,39,42 };
		break;
	case entity_state::PATHING_DOWN_RIGHT:
		entity_sprite_section = { 52,0,52,49 };
		break;
	case entity_state::PATHING_DOWN_LEFT:
		entity_sprite_section = { 0,0,52,49 };
		break;
	case entity_state::PATHING_UP_RIGHT:
		entity_sprite_section = { 104,0,52,49 };
		break;
	case entity_state::PATHING_UP_LEFT:
		entity_sprite_section = { 156,0,52,49 };
		break;
	}


	selection_collider.x = pixel_position.x + 10;
	selection_collider.y = pixel_position.y + 10;

	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y - 14, &entity_sprite_section);


	//App->render->DrawQuad(selection_collider, 255, 255, 0, 100);

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
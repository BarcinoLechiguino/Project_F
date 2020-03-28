#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Input.h"
#include "Pathfinding.h"

#include "EntityManager.h"

Gatherer::Gatherer(int x, int y, ENTITY_TYPE type) : Dynamic_Object(x, y, type)
{
	entity_sprite = App->tex->Load("maps/debug_gatherer_tile.png");

	pixel_position.x = App->map->MapToWorld(x, y).x;
	pixel_position.y = App->map->MapToWorld(x, y).y;

	speed = 100;

	target_tile = tile_position;
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
	MoveInput();

	if (target_tile != tile_position)
	{
		Move(dt);
	}

	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y, nullptr);

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

void Gatherer::Move(float dt)
{
	

	switch (unit_state)
	{
		case entity_state::PATHING_DOWN_LEFT:

			pixel_position.x -= speed * 0.58 * dt;
			pixel_position.y += speed * 0.42 * dt;

			if (pixel_position.x <= target_position.x || pixel_position.y >= target_position.y)
			{
				pixel_position = target_position;
				tile_position = target_tile;
				unit_state = entity_state::IDLE;
			}

		break;

		case entity_state::PATHING_DOWN_RIGHT:
			
			pixel_position.x += speed * 0.58 * dt;
			pixel_position.y += speed * 0.42 * dt;

			if (pixel_position.x >= target_position.x || pixel_position.y >= target_position.y)
			{
				pixel_position = target_position;
				tile_position = target_tile;
				unit_state = entity_state::IDLE;
			}

		break;

		case entity_state::PATHING_UP_LEFT:

			pixel_position.x -= speed * 0.58 * dt;
			pixel_position.y -= speed * 0.42 * dt;

			if (pixel_position.x <= target_position.x || pixel_position.y <= target_position.y)
			{
				pixel_position = target_position;
				tile_position = target_tile;
				unit_state = entity_state::IDLE;
			}

		break;

		case entity_state::PATHING_UP_RIGHT:

			pixel_position.x += speed * 0.58 * dt;
			pixel_position.y -= speed * 0.42 * dt;

			if (pixel_position.x >= target_position.x || pixel_position.y <= target_position.y)
			{
				pixel_position = target_position;
				tile_position = target_tile;
				unit_state = entity_state::IDLE;
			}

		break;


	}

} 

void Gatherer::MoveInput()
{
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN && unit_state == entity_state::IDLE)
	{
		if (App->pathfinding->IsWalkable(iPoint(tile_position.x, tile_position.y + 1)))
		{
			target_tile = iPoint(tile_position.x, tile_position.y + 1);

			unit_state = entity_state::PATHING_DOWN_LEFT;
		}
		target_position.x = App->map->MapToWorld(target_tile.x, target_tile.y).x;
		target_position.y = App->map->MapToWorld(target_tile.x, target_tile.y).y;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN && unit_state == entity_state::IDLE)
	{
		if (App->pathfinding->IsWalkable(iPoint(tile_position.x + 1, tile_position.y)))
		{
			target_tile = iPoint(tile_position.x + 1, tile_position.y);

			unit_state = entity_state::PATHING_DOWN_RIGHT;
		}
		target_position.x = App->map->MapToWorld(target_tile.x, target_tile.y).x;
		target_position.y = App->map->MapToWorld(target_tile.x, target_tile.y).y;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN && unit_state == entity_state::IDLE)
	{
		if (App->pathfinding->IsWalkable(iPoint(tile_position.x - 1, tile_position.y)))
		{
			target_tile = iPoint(tile_position.x - 1, tile_position.y);

			unit_state = entity_state::PATHING_UP_LEFT;
		}
		target_position.x = App->map->MapToWorld(target_tile.x, target_tile.y).x;
		target_position.y = App->map->MapToWorld(target_tile.x, target_tile.y).y;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && unit_state == entity_state::IDLE)
	{
		if (App->pathfinding->IsWalkable(iPoint(tile_position.x, tile_position.y - 1)))
		{
			target_tile = iPoint(tile_position.x, tile_position.y - 1);

			unit_state = entity_state::PATHING_UP_RIGHT;
		}
		target_position.x = App->map->MapToWorld(target_tile.x, target_tile.y).x;
		target_position.y = App->map->MapToWorld(target_tile.x, target_tile.y).y;
	}
}
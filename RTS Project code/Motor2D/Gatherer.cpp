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

	speed = 200;

	target_tile = tile_position;

	selection_collider = { (int)pixel_position.x, (int)pixel_position.y, 40, 30 };
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

	selection_collider.x = pixel_position.x;
	selection_collider.y = pixel_position.y;

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

void Gatherer::Move(float dt)
{
	bool target_reached = false;

	switch (unit_state)
	{
		case entity_state::PATHING_DOWN_LEFT:

			pixel_position.x -= speed * 0.58 * dt;
			pixel_position.y += speed * 0.42 * dt;

			if (pixel_position.x <= target_position.x || pixel_position.y >= target_position.y)
			{
				target_reached = true;
			}

		break;

		case entity_state::PATHING_DOWN_RIGHT:
			
			pixel_position.x += speed * 0.58 * dt;
			pixel_position.y += speed * 0.42 * dt;

			if (pixel_position.x >= target_position.x || pixel_position.y >= target_position.y)
			{
				target_reached = true;
			}

		break;

		case entity_state::PATHING_UP_LEFT:

			pixel_position.x -= speed * 0.58 * dt;
			pixel_position.y -= speed * 0.42 * dt;

			if (pixel_position.x <= target_position.x || pixel_position.y <= target_position.y)
			{
				target_reached = true;
			}

		break;

		case entity_state::PATHING_UP_RIGHT:

			pixel_position.x += speed * 0.58 * dt;
			pixel_position.y -= speed * 0.42 * dt;

			if (pixel_position.x >= target_position.x || pixel_position.y <= target_position.y)
			{
				target_reached = true;
			}

		break;

		case entity_state::PATHING_UP:

			pixel_position.y -= speed  * dt;

			if ( pixel_position.y <= target_position.y)
			{
				target_reached = true;
			}

			break;

		case entity_state::PATHING_DOWN:

			pixel_position.y += speed * dt;

			if (pixel_position.y >= target_position.y)
			{
				target_reached = true;
			}

			break;

		case entity_state::PATHING_RIGHT:

			pixel_position.x += speed * dt;

			if (pixel_position.x >= target_position.x )
			{
				target_reached = true;
			}

			break;

		case entity_state::PATHING_LEFT:

			pixel_position.x -= speed * dt;

			if (pixel_position.x <= target_position.x)
			{
				target_reached = true;
			}

			break;

	}
	if (target_reached)
	{
		pixel_position = target_position;
		tile_position = target_tile;
		unit_state = entity_state::IDLE;
	}
} 

void Gatherer::MoveInput()
{
	bool key_was_pressed = false;
	iPoint target_tile_buffer;
	entity_state old_state = unit_state;

	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN && unit_state == entity_state::IDLE)
	{
		target_tile_buffer = iPoint(tile_position.x, tile_position.y + 1);
		unit_state = entity_state::PATHING_DOWN_LEFT;
		
		key_was_pressed = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN && unit_state == entity_state::IDLE)
	{
		target_tile_buffer = iPoint(tile_position.x + 1, tile_position.y);
		unit_state = entity_state::PATHING_DOWN_RIGHT;
		key_was_pressed = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN && unit_state == entity_state::IDLE)
	{
		target_tile_buffer = iPoint(tile_position.x - 1, tile_position.y);

		unit_state = entity_state::PATHING_UP_LEFT;
		key_was_pressed = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN && unit_state == entity_state::IDLE)
	{
		target_tile_buffer = iPoint(tile_position.x, tile_position.y - 1);
		unit_state = entity_state::PATHING_UP_RIGHT;
		key_was_pressed = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && unit_state == entity_state::IDLE)
	{
		target_tile_buffer = iPoint(tile_position.x - 1, tile_position.y - 1);
		unit_state = entity_state::PATHING_UP;
		key_was_pressed = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN && unit_state == entity_state::IDLE)
	{
		target_tile_buffer = iPoint(tile_position.x +1, tile_position.y - 1);
		unit_state = entity_state::PATHING_RIGHT;
		key_was_pressed = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN && unit_state == entity_state::IDLE)
	{
		target_tile_buffer = iPoint(tile_position.x +1, tile_position.y + 1);
		unit_state = entity_state::PATHING_DOWN;
		key_was_pressed = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN && unit_state == entity_state::IDLE)
	{
		target_tile_buffer = iPoint(tile_position.x -1, tile_position.y + 1);
		unit_state = entity_state::PATHING_LEFT;
		key_was_pressed = true;
	}

	if (key_was_pressed)
	{
		if (App->pathfinding->IsWalkable(target_tile_buffer))
		{
			target_tile = target_tile_buffer;

			target_position.x = App->map->MapToWorld(target_tile.x, target_tile.y).x;
			target_position.y = App->map->MapToWorld(target_tile.x, target_tile.y).y;
		}
		else
		{
			unit_state = old_state;
		}
		key_was_pressed = false;
	}
}
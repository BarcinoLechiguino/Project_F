#include "Application.h"
#include "Pathfinding.h"
#include "Map.h"

#include "EntityManager.h"


Dynamic_Object::Dynamic_Object(int x, int y, ENTITY_TYPE type) : Entity(x, y, type)
{

	pixel_position.x = App->map->MapToWorld(x, y).x;
	pixel_position.y = App->map->MapToWorld(x, y).y;

	selection_collider = { (int)pixel_position.x, (int)pixel_position.y, 40, 30 };

	target_tile = tile_position;
}

bool Dynamic_Object::Awake(pugi::xml_node&)
{
	return true;
}

bool Dynamic_Object::Start()
{
	return true;
}

bool Dynamic_Object::PreUpdate()
{
	return true;
}

bool Dynamic_Object::Update(float dt, bool doLogic)
{
	

	return true;
}

bool Dynamic_Object::PostUpdate()
{
	return true;
}

bool Dynamic_Object::CleanUp()
{
	return true;
}

void Dynamic_Object::GiveNewTarget(iPoint new_target)
{
	entity_path.clear();

	App->pathfinding->CreatePath(tile_position, new_target);
	entity_path = App->pathfinding->GetLastPath();

	path_full = true;

	target_tile = entity_path.back();
	current_path_tile = entity_path.begin();

	int pos = 0;
	for (std::vector<iPoint>::iterator item = entity_path.begin(); item != entity_path.end(); ++item)
	{
		pos++;
		LOG("Path tile: %d pos: %d %d", pos, (*item).x, (*item).y);
	}

	LOG("target_tile %d %d", entity_path.back().x, entity_path.back().y);
}

void Dynamic_Object::HandleMovement(float dt)
{

	switch (path_state)
	{
	case pathfind_state::IDLE:

		if (path_full && tile_position != target_tile)
		{
			path_state = pathfind_state::WAITING_NEXT_TILE;

			current_path_tile = entity_path.begin();
		}

		break;

	case pathfind_state::WALKING:

		Move(dt);

		break;

	case pathfind_state::WAITING_NEXT_TILE:

		//Check if unit is already in target_tile
		if (target_tile == tile_position)
		{
			entity_path.clear();

			path_full = false;

			path_state = pathfind_state::IDLE;
			unit_state = entity_state::IDLE;

			

			break;
		}

		//Calculate next tile and decide direction

		advance(current_path_tile, 1);

		next_tile = (*current_path_tile);

		LOG("Next tile %d %d", next_tile.x, next_tile.y);

		next_tile_position = App->map->MapToWorld(next_tile.x, next_tile.y);

		path_state = pathfind_state::WALKING;

		if (next_tile.x == tile_position.x + 1)
		{
			if (next_tile.y == tile_position.y + 1)
			{
				unit_state = entity_state::PATHING_DOWN;
			}
			else if (next_tile.y == tile_position.y - 1)
			{
				unit_state = entity_state::PATHING_RIGHT;
			}
			else
			{
				unit_state = entity_state::PATHING_DOWN_RIGHT;
			}
		}
		else if (next_tile.x == tile_position.x - 1)
		{
			if (next_tile.y == tile_position.y + 1)
			{
				unit_state = entity_state::PATHING_LEFT;
			}
			else if (next_tile.y == tile_position.y - 1)
			{
				unit_state = entity_state::PATHING_UP;
			}
			else
			{
				unit_state = entity_state::PATHING_UP_LEFT;
			}
		}
		else
		{
			if (next_tile.y == tile_position.y + 1)
			{
				unit_state = entity_state::PATHING_DOWN_LEFT;
			}
			else if (next_tile.y == tile_position.y - 1)
			{
				unit_state = entity_state::PATHING_UP_RIGHT;
			}
		}

		break;
	}

}

void Dynamic_Object::Move(float dt)
{
	bool next_reached = false;

	switch (unit_state)
	{
	case entity_state::PATHING_DOWN_LEFT:

		pixel_position.x -= speed * 0.58 * dt;
		pixel_position.y += speed * 0.42 * dt;

		if (pixel_position.x <= next_tile_position.x || pixel_position.y >= next_tile_position.y)
		{
			next_reached = true;
		}

		break;

	case entity_state::PATHING_DOWN_RIGHT:

		pixel_position.x += speed * 0.58 * dt;
		pixel_position.y += speed * 0.42 * dt;

		if (pixel_position.x >= next_tile_position.x || pixel_position.y >= next_tile_position.y)
		{
			next_reached = true;
		}

		break;

	case entity_state::PATHING_UP_LEFT:

		pixel_position.x -= speed * 0.58 * dt;
		pixel_position.y -= speed * 0.42 * dt;

		if (pixel_position.x <= next_tile_position.x || pixel_position.y <= next_tile_position.y)
		{
			next_reached = true;
		}

		break;

	case entity_state::PATHING_UP_RIGHT:

		pixel_position.x += speed * 0.58 * dt;
		pixel_position.y -= speed * 0.42 * dt;

		if (pixel_position.x >= next_tile_position.x || pixel_position.y <= next_tile_position.y)
		{
			next_reached = true;
		}

		break;

	case entity_state::PATHING_UP:

		pixel_position.y -= speed * dt;

		if (pixel_position.y <= next_tile_position.y)
		{
			next_reached = true;
		}

		break;

	case entity_state::PATHING_DOWN:

		pixel_position.y += speed * dt;

		if (pixel_position.y >= next_tile_position.y)
		{
			next_reached = true;
		}

		break;

	case entity_state::PATHING_RIGHT:

		pixel_position.x += speed * dt;

		if (pixel_position.x >= next_tile_position.x)
		{
			next_reached = true;
		}

		break;

	case entity_state::PATHING_LEFT:

		pixel_position.x -= speed * dt;

		if (pixel_position.x <= next_tile_position.x)
		{
			next_reached = true;
		}

		break;

	}
	if (next_reached)
	{
		pixel_position.x = next_tile_position.x;
		pixel_position.y = next_tile_position.y;

		tile_position = next_tile;
		unit_state = entity_state::IDLE;
		path_state = pathfind_state::WAITING_NEXT_TILE;
	}
}
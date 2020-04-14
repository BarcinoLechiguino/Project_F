#include "Application.h"
#include "Map.h"
#include "Pathfinding.h"
#include "EntityManager.h"

#include "Dynamic_Object.h"


Dynamic_Object::Dynamic_Object(int x, int y, ENTITY_TYPE type) : Entity(x, y, type)
{

	pixel_position.x = App->map->MapToWorld(x, y).x;
	pixel_position.y = App->map->MapToWorld(x, y).y;

	selection_collider = { (int)pixel_position.x + 20, (int)pixel_position.y + 20 , 35, 25 };

	target_tile = tile_position;
	next_tile = tile_position;

	ChangeOccupiedTile(tile_position);
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
	//New Path using the next tile if it's going to one
	App->pathfinding->CreatePath(next_tile, new_target);

	entity_path.clear();
	entity_path = App->pathfinding->GetLastPath();

	path_full = true;

	//Change target and current tile to get next
	target_tile = entity_path.back();
	current_path_tile = entity_path.begin();

	ChangeOccupiedTile(target_tile);

	//int pos = 0;
	//for (std::vector<iPoint>::iterator item = entity_path.begin(); item != entity_path.end(); ++item)
	//{
	//	pos++;
	//	LOG("Path tile: %d pos: %d %d", pos, (*item).x, (*item).y);
	//}

	//LOG("target_tile %d %d", entity_path.back().x, entity_path.back().y);
}

void Dynamic_Object::ChangeOccupiedTile(iPoint new_occupied_tile)
{
	App->pathfinding->ChangeWalkability(occupied_tile, WALKABLE);

	occupied_tile = new_occupied_tile;

	App->pathfinding->ChangeWalkability(new_occupied_tile, OCCUPIED);
}

void Dynamic_Object::HandleMovement(float dt)
{

	switch (path_state)
	{
	case PATHFINDING_STATE::IDLE:

		if (path_full && tile_position != target_tile)
		{
			path_state = PATHFINDING_STATE::WAITING_NEXT_TILE;

			current_path_tile = entity_path.begin();
		}

		break;

	case PATHFINDING_STATE::WALKING:

		Move(dt);

		break;

	case PATHFINDING_STATE::WAITING_NEXT_TILE:

		//Check if unit is already in target_tile
		if (target_tile == tile_position)
		{
			entity_path.clear();

			path_full = false;

			path_state = PATHFINDING_STATE::IDLE;
			unit_state = ENTITY_STATE::IDLE;

			break;
		}

		//Calculate next tile and decide direction

		advance(current_path_tile, 1);

		next_tile = (*current_path_tile);

		//LOG("Next tile %d %d", next_tile.x, next_tile.y);

		next_tile_position = App->map->MapToWorld(next_tile.x, next_tile.y);

		path_state = PATHFINDING_STATE::WALKING;

		if (next_tile.x == tile_position.x + 1)
		{
			if (next_tile.y == tile_position.y + 1)
			{
				unit_state = ENTITY_STATE::PATHING_DOWN;
			}
			else if (next_tile.y == tile_position.y - 1)
			{
				unit_state = ENTITY_STATE::PATHING_RIGHT;
			}
			else
			{
				unit_state = ENTITY_STATE::PATHING_DOWN_RIGHT;
			}
		}
		else if (next_tile.x == tile_position.x - 1)
		{
			if (next_tile.y == tile_position.y + 1)
			{
				unit_state = ENTITY_STATE::PATHING_LEFT;
			}
			else if (next_tile.y == tile_position.y - 1)
			{
				unit_state = ENTITY_STATE::PATHING_UP;
			}
			else
			{
				unit_state = ENTITY_STATE::PATHING_UP_LEFT;
			}
		}
		else
		{
			if (next_tile.y == tile_position.y + 1)
			{
				unit_state = ENTITY_STATE::PATHING_DOWN_LEFT;
			}
			else if (next_tile.y == tile_position.y - 1)
			{
				unit_state = ENTITY_STATE::PATHING_UP_RIGHT;
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
	case ENTITY_STATE::PATHING_DOWN_LEFT:
		
		pixel_position.x -= speed * 0.803f * dt;
		pixel_position.y += speed * 0.59f * dt;

		if (pixel_position.x <= next_tile_position.x || pixel_position.y >= next_tile_position.y)
		{
			next_reached = true;
		}

	break;

	case ENTITY_STATE::PATHING_DOWN_RIGHT:

		pixel_position.x += speed * 0.803f * dt;
		pixel_position.y += speed * 0.59f * dt;

		if (pixel_position.x >= next_tile_position.x || pixel_position.y >= next_tile_position.y)
		{
			next_reached = true;
		}

	break;

	case ENTITY_STATE::PATHING_UP_LEFT:

		pixel_position.x -= speed * 0.803f * dt;
		pixel_position.y -= speed * 0.59f * dt;

		if (pixel_position.x <= next_tile_position.x || pixel_position.y <= next_tile_position.y)
		{
			next_reached = true;
		}

	break;

	case ENTITY_STATE::PATHING_UP_RIGHT:

		pixel_position.x += speed * 0.803f * dt;
		pixel_position.y -= speed * 0.59f * dt;

		if (pixel_position.x >= next_tile_position.x || pixel_position.y <= next_tile_position.y)
		{
			next_reached = true;
		}

	break;

	case ENTITY_STATE::PATHING_UP:

		pixel_position.y -= speed * dt;

		if (pixel_position.y <= next_tile_position.y)
		{
			next_reached = true;
		}

	break;

	case ENTITY_STATE::PATHING_DOWN:

		pixel_position.y += speed * dt;

		if (pixel_position.y >= next_tile_position.y)
		{
			next_reached = true;
		}

	break;

	case ENTITY_STATE::PATHING_RIGHT:

		pixel_position.x += speed * dt;

		if (pixel_position.x >= next_tile_position.x)
		{
			next_reached = true;
		}

	break;

	case ENTITY_STATE::PATHING_LEFT:

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

		iPoint tmp = tile_position;

		tile_position = next_tile;

		App->entity_manager->ChangeEntityMap(tile_position, this);				// ENTITY MAP UPDATE
		App->entity_manager->ChangeEntityMap(tmp, this, true);

		unit_state = ENTITY_STATE::IDLE;
		path_state = PATHFINDING_STATE::WAITING_NEXT_TILE;
	}
}
#include "Application.h"
#include "Map.h"
#include "Pathfinding.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Audio.h"

#include "Player.h"

#include "DynamicObject.h"


DynamicObject::DynamicObject(int x, int y, ENTITY_TYPE type, int level) : Entity(x, y, type, level)
{
	pixel_position.x = (float)App->map->MapToWorld(x, y).x;
	pixel_position.y = (float)App->map->MapToWorld(x, y).y;

	selection_collider = { (int)pixel_position.x + 20, (int)pixel_position.y + 20 , 35, 25 };

	speed_x_factor = 0.803f;							// Get from config.xml Magic
	speed_y_factor = 0.59f;								// Get from config.xml

	target_tile = tile_position;
	next_tile = tile_position;
	//occupied_tile = tile_position;


	tiles_occupied.x = 1;								// TMP Placeholder.
	tiles_occupied.y = 1;								// Might be used later on for units bigger than 1x1.

	looked_for_tile = false;

	path_state = PATHFINDING_STATE::IDLE;

	ChangeOccupiedTile(tile_position);
}

bool DynamicObject::Awake(pugi::xml_node&)
{
	return true;
}

bool DynamicObject::Start()
{	
	return true;
}

bool DynamicObject::PreUpdate()
{
	return true;
}

bool DynamicObject::Update(float dt, bool do_logic)
{
	return true;
}

bool DynamicObject::PostUpdate()
{
	return true;
}

bool DynamicObject::CleanUp()
{
	return true;
}

void DynamicObject::Draw()
{
	return;
}

void DynamicObject::InitUnitSpriteSections()
{
	return;
}

void DynamicObject::UpdateUnitSpriteSection()
{
	return;
}

void DynamicObject::UpdateUnitOrientation()
{
	return;
}

void DynamicObject::SetEntityTargetByProximity(const iPoint& target_position)
{
	return;
}

void DynamicObject::GetShortestPathWithinAttackRange()
{
	return;
}

void DynamicObject::SetGatheringTarget(const iPoint& tile_position)
{
	return;
}

void DynamicObject::PathToGatheringTarget()
{
	return;
}

bool DynamicObject::TargetIsInRange()
{
	return true;
}

void DynamicObject::ChaseTarget()
{
	return;
}

void DynamicObject::DealDamage()
{
	return;
}

bool DynamicObject::GiveNewTargetTile(const iPoint& new_target_tile)
{
	//New Path using the next tile if it's going to one
	if (App->pathfinding->CreatePath(next_tile, new_target_tile) == -2)
	{
		return false;
	}

	entity_path.clear();
	entity_path = App->pathfinding->GetLastPath();

	path_full = true;

	target_tile = entity_path.back();
	current_path_tile = entity_path.begin();

	ChangeOccupiedTile(target_tile);

	return true;
}

void DynamicObject::ChangeOccupiedTile(iPoint new_occupied_tile)
{
	//LOG("x %d and y %d", new_occupied_tile.x, new_occupied_tile.y);
	
	App->pathfinding->ChangeWalkability(occupied_tile, this, WALKABLE);
	
	occupied_tile = new_occupied_tile;

	App->pathfinding->ChangeWalkability(new_occupied_tile, this, OCCUPIED);
}

void DynamicObject::HandleMovement(float dt)
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

			App->entity_manager->ChangeEntityMap(tile_position, this);

			if (occupied_tile != tile_position)							// In case target enters attack_range and the unit ends the pathfinding.
			{
				ChangeOccupiedTile(tile_position);
			}

		}
		else
		{
			//Calculate next tile and decide direction
			advance(current_path_tile, 1);

			next_tile = (*current_path_tile);

			App->entity_manager->ChangeEntityMap(tile_position, this, true);
			App->entity_manager->ChangeEntityMap(next_tile, this);

			//LOG("Next tile %d %d", next_tile.x, next_tile.y);

			next_tile_position = App->map->MapToWorld(next_tile.x, next_tile.y);
			path_state = PATHFINDING_STATE::WALKING;

			SetEntityState();

			//Move so that it doesn't stutter
			Move(dt);
		}
		break;
	}
}

void DynamicObject::HandleFx()
{
	if (path_full && type == ENTITY_TYPE::GATHERER)
	{
		if (!fx_playing)
		{
			channel = App->audio->PlayFx(App->entity_manager->gatherer_movement_fx, -1);
			fx_playing = true;
		}
	}
	else
	{
		fx_playing = false;
		Mix_FadeOutChannel(channel, 400);
	}
}

void DynamicObject::SetEntityState()
{
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
}

void DynamicObject::Move(float dt)
{
	bool next_reached = false;

	switch (unit_state)
	{
	case ENTITY_STATE::PATHING_DOWN_LEFT:
		
		pixel_position.x -= speed * speed_x_factor * dt;
		pixel_position.y += speed * speed_y_factor * dt;

		if (pixel_position.x <= next_tile_position.x || pixel_position.y >= next_tile_position.y)
		{
			next_reached = true;
		}

	break;

	case ENTITY_STATE::PATHING_DOWN_RIGHT:

		pixel_position.x += speed * speed_x_factor * dt;
		pixel_position.y += speed * speed_y_factor * dt;

		if (pixel_position.x >= next_tile_position.x || pixel_position.y >= next_tile_position.y)
		{
			next_reached = true;
		}

	break;

	case ENTITY_STATE::PATHING_UP_LEFT:

		pixel_position.x -= speed * speed_x_factor * dt;
		pixel_position.y -= speed * speed_y_factor * dt;

		if (pixel_position.x <= next_tile_position.x || pixel_position.y <= next_tile_position.y)
		{
			next_reached = true;
		}

	break;

	case ENTITY_STATE::PATHING_UP_RIGHT:

		pixel_position.x += speed * speed_x_factor * dt;
		pixel_position.y -= speed * speed_y_factor * dt;

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
		pixel_position.x = (float)next_tile_position.x;
		pixel_position.y = (float)next_tile_position.y;


		App->entity_manager->ChangeEntityMap(tile_position, this, true);		// ENTITY MAP UPDATE
		
		tile_position = next_tile;

		App->entity_manager->ChangeEntityMap(tile_position, this);				// ENTITY MAP UPDATE

		unit_state = ENTITY_STATE::IDLE;
		path_state = PATHFINDING_STATE::WAITING_NEXT_TILE;
	}
}

void DynamicObject::DataMapSafetyCheck()
{
	if (!path_full)
	{
		if (App->pathfinding->GetTileAt(tile_position) != OCCUPIED)
		{
			App->pathfinding->ChangeWalkability(tile_position, this, OCCUPIED);
		}

		if (App->entity_manager->GetEntityAt(tile_position) != this)
		{
			App->entity_manager->ChangeEntityMap(tile_position, this);
		}
	}
}

Entity* DynamicObject::GetTarget()
{
	return target;
}

int DynamicObject::GetAttackRange()
{
	return attack_range;
}

////--- ALTERNATIVE VERSION
//if (tile_position.x > next_tile.x && tile_position.y > next_tile.y)					// next_tile is (--x , --y)
//{
//	unit_state = ENTITY_STATE::PATHING_UP;
//	return;
//}
//
//if (tile_position.x < next_tile.x && tile_position.y < next_tile.y)					// next_tile is (++x , ++y)
//{
//	unit_state = ENTITY_STATE::PATHING_DOWN;
//	return;
//}
//
//if (tile_position.x < next_tile.x && tile_position.y > next_tile.y)					// next_tile is (--x , ++y)
//{
//	unit_state = ENTITY_STATE::PATHING_RIGHT;
//	return;
//}
//
//if (tile_position.x > next_tile.x && tile_position.y < next_tile.y)					// next_tile is (++x, --y)
//{
//	unit_state = ENTITY_STATE::PATHING_LEFT;
//	return;
//}
//
//if (tile_position.x == next_tile.x && tile_position.y > next_tile.y)					// next_tile is (== , --y)
//{
//	unit_state = ENTITY_STATE::PATHING_UP_RIGHT;
//	return;
//}
//
//if (tile_position.x > next_tile.x && tile_position.y == next_tile.y)					// next tile is (--x, ==)
//{
//	unit_state = ENTITY_STATE::PATHING_UP_LEFT;
//	return;
//}
//
//if (tile_position.x < next_tile.x && tile_position.y == next_tile.y)					// next tile is (++x, ==)
//{
//	unit_state = ENTITY_STATE::PATHING_DOWN_RIGHT;
//	return;
//}
//
//if (tile_position.x == next_tile.x && tile_position.y < next_tile.y)					// next tile is (==, ++y)
//{
//	unit_state = ENTITY_STATE::PATHING_DOWN_LEFT;
//	return;
//}
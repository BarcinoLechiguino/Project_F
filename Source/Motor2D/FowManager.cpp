#include "Brofiler/Brofiler.h"

#include <map>

#include "Point.h"
#include "Application.h"
#include "Textures.h"
#include "Input.h"
#include "Map.h"
#include "Player.h"

#include "FowManager.h"

FowManager::FowManager() : visibility_map(nullptr), debug_visibility_map(nullptr), visibility_map_debug_buffer(nullptr), fow_tex(nullptr), scouting_trail(true), fow_debug(false)
{

}

FowManager::~FowManager()
{
	// Release the map here
}

bool FowManager::Awake(pugi::xml_node& config)
{
	return true;
}

bool FowManager::Start()
{
	InitFowManager();
	
	frontier_debug_tex = App->tex->Load("maps/occupied_tile.png");
	line_of_sight_debug_tex = App->tex->Load("maps/occupied_by_entity_tile.png");

	return true;
}

bool FowManager::PreUpdate()
{
	return true;
}

bool FowManager::Update(float dt)
{
	if (App->player->CurrentlyInGameplayScene())
	{
		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_STATE::KEY_REPEAT)		// FOW DEBUG TOOLS
		{
			App->map->DrawMapGrid();

			DebugFrontier();
			DebugLineOfSight();
		}

		if (!fow_debug)
		{
			UpdateEntitiesFowManipulation();
		}

		UpdateEntitiesVisibility();
	}
	
	return true;
}

bool FowManager::PostUpdate()
{
	return true;
}

bool FowManager::CleanUp()
{
	return true;
}

void FowManager::SetVisibilityMap(const int& width, const int& height)
{
	uint map_size = width * height;

	visibility_map_width = width;
	visibility_map_height = height;
	
	if (visibility_map != nullptr || debug_visibility_map != nullptr)
	{
		ClearVisibilityMapContainers();
	}
	
	visibility_map = new uchar[map_size];
	memset(visibility_map, UNEXPLORED, map_size);									// Will set the state of all the tiles in the container to UNEXPLORED.

	debug_visibility_map = new uchar[map_size];
	memset(debug_visibility_map, VISIBLE, map_size);								// Will set the state of all the tiles in the container to VISIBLE. (For debug purposes).
}

void FowManager::ChangeVisibilityMap(const iPoint& tile_position, const uchar& visibility)
{
	if (CheckMapBoundaries(tile_position))
	{
		visibility_map[(tile_position.y * visibility_map_width) + tile_position.x] = visibility;
	}
}

void FowManager::SwapVisibilityMaps()
{
	if (visibility_map != debug_visibility_map)
	{
		visibility_map_debug_buffer = visibility_map;					// A copy of the current visibility map is kept so it can be swapped later.
		visibility_map = debug_visibility_map;							// The visibility_map and the debug_visibility_map are swapped.
	}
	else
	{
		visibility_map = visibility_map_debug_buffer;					// If the visibility map is different than the copy in the buffer, then the map is swapped back to normal.
	}

	/*if (fow_debug)
	{

	}
	else
	{
		if (visibility_map_debug_buffer != nullptr)
		{

		}
	}*/
}

void FowManager::ResetVisibilityMap()
{
	SetVisibilityMap(visibility_map_width, visibility_map_width);

	if (fow_debug)
	{
		fow_debug = false;
	}
}

void FowManager::ClearVisibilityMapContainers()
{
	if (debug_visibility_map != nullptr)
	{
		if (debug_visibility_map != visibility_map)
		{
			delete[] debug_visibility_map;
			debug_visibility_map = nullptr;
		}
		else
		{
			debug_visibility_map = nullptr;
		}
	}
	
	if (visibility_map != nullptr)
	{
		if (visibility_map_debug_buffer != visibility_map)
		{
			delete[] visibility_map_debug_buffer;
			visibility_map_debug_buffer = nullptr;
		}
		else
		{
			visibility_map_debug_buffer = nullptr;
		}

		delete[] visibility_map;
		visibility_map = nullptr;
	}
}

bool FowManager::CheckMapBoundaries(const iPoint& tile_position)
{
	return (tile_position.x >= 0 && tile_position.x < visibility_map_width
			&& tile_position.y >= 0 && tile_position.y < visibility_map_height);
}

uchar FowManager::GetVisibilityAt(const iPoint& tile_position)
{	
	if (CheckMapBoundaries(tile_position))
	{
		return visibility_map[(tile_position.y * visibility_map_width) + tile_position.x];
	}

	//return INVALID_VISIBILITY_CODE;
	return VISIBLE;
}

SDL_Rect FowManager::GetFowTileRect(const uchar& visibility)
{
	SDL_Rect ret;

	ret.w = 64;
	ret.h = 64;
	ret.x = visibility * ret.w;												// TMP. Adapt to be able to use smoothed tiles.
	ret.y = 0;

	return ret;
}

FowEntity* FowManager::CreateFowEntity(const iPoint& tile_position, bool provides_visibility, bool is_static)
{
	FowEntity* fow_entity = nullptr;

	fow_entity = new FowEntity(tile_position, provides_visibility, is_static);

	if (fow_entity != nullptr)
	{
		fow_entities.push_back(fow_entity);
	}

	return fow_entity;
}

void FowManager::DeleteFowEntity(FowEntity* fow_entity_to_delete)
{
	std::vector<FowEntity*>::iterator item = fow_entities.begin();
	
	for (; item != fow_entities.end(); ++item)
	{
		if ((*item) == fow_entity_to_delete)
		{
			(*item)->CleanUp();

			RELEASE((*item));
			fow_entities.erase(item);

			break;
		}
	}
}

void FowManager::DestroyFowEntities()
{
	for (uint i = 0; i < fow_entities.size(); ++i)
	{
		fow_entities[i]->CleanUp();
		RELEASE(fow_entities[i]);
	}

	fow_entities.clear();
}

void FowManager::ClearFowEntityLineOfSight(FowEntity* fow_entity_to_clear)					// Mind fow tile conflicts.
{
	std::vector<iPoint>::iterator tile = fow_entity_to_clear->line_of_sight.begin();

	for (; tile != fow_entity_to_clear->line_of_sight.end(); ++tile)
	{
		tiles_to_reset.push_back((*tile));
		
		/*if (scouting_trail)
		{
			ChangeVisibilityMap((*tile), FOGGED);
		}
		else
		{
			ChangeVisibilityMap((*tile), UNEXPLORED);
		}*/
	}
}

std::vector<iPoint> FowManager::CreateRectangularFrontier(const int& width, const int& height, const iPoint& center)
{
	std::vector<iPoint> frontier;

	iPoint origin = { center.x - width, center.y - height };
	iPoint end = { center.x + width, center.y + height };

	for (int y = origin.y; y < end.y; ++y)
	{
		for (int x = origin.x; x < end.x; ++x)
		{
			iPoint fow_tile = { x, y };

			if (x == origin.x || y == origin.y || x == end.x - 1  || y == end.y - 1)					// Only the tiles at the border of the rectangle are stored in frontier.
			{
				frontier.push_back(fow_tile);
			}
			/*else
			{
				line_of_sight.pushback(fow_tile);
			}*/
		}
	}

	return frontier;
}

std::vector<iPoint> FowManager::CreateCircularFrontier(const uint& radius, const iPoint& center)
{
	std::vector<iPoint> frontier;

	iPoint origin = { center.x - (int)radius, center.y - (int)radius };
	iPoint end = { center.x + (int)radius, center.y + (int)radius };

	for (int y = origin.y; y <= end.y; ++y)
	{
		for (int x = origin.x; x <= end.x; ++x)
		{
			if (center.DistanceTo({x, y}) == radius)
			{
				frontier.push_back({ x, y });
			}
		}
	}

	return frontier;
}

std::vector<iPoint> FowManager::GetLineOfSight(const std::vector<iPoint>& frontier)
{
	std::vector<iPoint> line_of_sight;

	std::vector<iPoint>::const_iterator tile = frontier.cbegin();

	for (; tile != frontier.cend(); ++tile)
	{
		if ((*next(tile)).y == (*tile).y && ((*next(tile)).x - (*tile).x) > 1)			// If the next tile is in the same column and the x distance with is more than 1.
		{
			int width = (*next(tile)).x - (*tile).x;									// Get row width between the  current and the next tile.

			for (int i = 0; i < width; ++i)
			{
				line_of_sight.push_back({ (*tile).x + i ,(*tile).y });					// Push back each tile in the row to line_of_sight
			}
		}
		else
		{
			line_of_sight.push_back(*tile);
		}
	}

	return line_of_sight;
}

void FowManager::DebugLineOfSight()
{
	std::vector<FowEntity*>::iterator item = fow_entities.begin();

	for (; item != fow_entities.end(); ++item)
	{
		if ((*item)->provides_visibility)
		{
			std::vector<iPoint> LOS = (*item)->line_of_sight;

			for (int i = 0; i < LOS.size(); ++i)
			{
				iPoint draw_position = App->map->MapToWorld(LOS[i].x, LOS[i].y);
				App->render->Blit(line_of_sight_debug_tex, draw_position.x, draw_position.y, nullptr);
			}
		}
	}

}

void FowManager::DebugFrontier()
{
	std::vector<FowEntity*>::iterator item = fow_entities.begin();

	for (; item != fow_entities.end(); ++item)
	{
		if ((*item)->provides_visibility)
		{
			std::vector<iPoint> frontier = (*item)->frontier;

			for (int i = 0; i < frontier.size(); ++i)
			{
				iPoint draw_position = App->map->MapToWorld(frontier[i].x, frontier[i].y);
				App->render->Blit(frontier_debug_tex, draw_position.x, draw_position.y, nullptr);
			}
		}
	}
}

void FowManager::UpdateEntitiesVisibility()
{
	for (uint i = 0; i < fow_entities.size(); ++i)
	{
		if (GetVisibilityAt(fow_entities[i]->position) != VISIBLE)
		{
			fow_entities[i]->is_visible = false;
		}
		else
		{
			fow_entities[i]->is_visible = true;
		}
	}
}

void FowManager::UpdateEntitiesFowManipulation()
{
	BROFILER_CATEGORY("UpdateEntitiesFowManipulation", Profiler::Color::MediumSeaGreen)
	
	std::vector<FowEntity*>::iterator item = fow_entities.begin();
	
	std::vector<FowEntity*> static_entities;

	for (; item != fow_entities.end(); ++item)
	{
		if ((*item)->provides_visibility)
		{
			if ((*item)->visibility_initialized)
			{
				if ((*item)->has_moved)
				{
					UpdateEntityLineOfSight((*item));
				}
			}
			else
			{
				std::vector<iPoint>::iterator tile = (*item)->line_of_sight.begin();

				for (; tile < (*item)->line_of_sight.end(); ++tile)
				{
					ChangeVisibilityMap((*tile), VISIBLE);
				}

				(*item)->visibility_initialized = true;
			}
		}

		//Smooth the edges of the fow_tiles.
	}

	if (tiles_to_reset.size() != 0)														// To avoid tile conflicts, the tiles that are to be reset are iterated before the second check.
	{
		for (int i = 0; i < tiles_to_reset.size(); ++i)
		{
			if (scouting_trail)
			{
				ChangeVisibilityMap(tiles_to_reset[i], FOGGED);
			}
			else
			{
				ChangeVisibilityMap(tiles_to_reset[i], UNEXPLORED);
			}
		}

		tiles_to_reset.clear();
	}

	// Revise when to smooth the tile edges.
	item = fow_entities.begin();

	for (; item != fow_entities.end(); ++item)
	{
		if ((*item)->provides_visibility)
		{
			std::vector<iPoint>::iterator tile = (*item)->line_of_sight.begin();

			for (; tile < (*item)->line_of_sight.end(); ++tile)
			{
				ChangeVisibilityMap((*tile), VISIBLE);
			}
		}
	}
}

void FowManager::UpdateEntityLineOfSight(FowEntity* entity_to_update)
{	
	BROFILER_CATEGORY("UpdateEntityLineOfSight", Profiler::Color::Fuchsia)
	
	std::vector<iPoint> previous_line_of_sight = entity_to_update->line_of_sight;

	std::vector<iPoint>::iterator tile = entity_to_update->line_of_sight.begin();

	for (; tile != entity_to_update->line_of_sight.end(); ++tile)
	{
		(*tile) += entity_to_update->motion;

		ChangeVisibilityMap((*tile), VISIBLE);
	}

	entity_to_update->has_moved = false;

	
	std::vector<iPoint>::iterator previous_tile = previous_line_of_sight.begin();

	for (; previous_tile != previous_line_of_sight.end(); ++previous_tile)
	{
		if (!TileIsInsideLineOfSight((*previous_tile), entity_to_update->line_of_sight))
		{
			if (scouting_trail)
			{
				ChangeVisibilityMap((*previous_tile), FOGGED);
			}
			else
			{
				ChangeVisibilityMap((*previous_tile), UNEXPLORED);
			}
		}
	}
}

bool FowManager::TileIsInsideLineOfSight(const iPoint& tile_position, const std::vector<iPoint>& line_of_sight)
{
	bool ret = false;

	for (uint i = 0; i < line_of_sight.size(); ++i)
	{
		if (line_of_sight[i] == tile_position)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

void FowManager::InitFowManager()
{
	config.load_file("config.xml");

	pugi::xml_node fow = config.child("config").child("fog_of_war");

	fow_tex = App->tex->Load(fow.child("fow_tex").attribute("path").as_string());
}

// --- FOW_Entity methods ---
FowEntity::FowEntity() : position({ 0, 0 }), motion({ 0, 0 }), provides_visibility(false), is_static(false), visibility_initialized(false), has_moved(false)
{

}

FowEntity::FowEntity(const iPoint& position, const bool provides_visibility, const bool is_static) 
	: position(position)
	, motion({ 0, 0 })
	, provides_visibility(provides_visibility)
	, is_static(is_static)
	, visibility_initialized(false)
	, has_moved(false)
{

}

void FowEntity::CleanUp()
{
	if (!App->fow_manager->fow_debug)
	{
		if (provides_visibility)
		{
			App->fow_manager->ClearFowEntityLineOfSight(this);
		}
	}

	frontier.clear();
	line_of_sight.clear();
}

void FowEntity::SetPos(const iPoint& new_position)
{
	if (position != new_position)
	{
		motion = { new_position.x - position.x, new_position.y - position.y };

		position = new_position;

		has_moved = true;
	}
}

// ------------------------------
/*void FowManager::SetVisibilityMap(uint width, uint height)
{
	uint map_size = width * height;

	if (visibility_map != nullptr || debug_visibility_map != nullptr)
	{
		ClearVisibilityMapContainers();
	}

	visibility_map = new FOW_TILE_STATE[map_size];
	memset(visibility_map, 0, sizeof(FOW_TILE_STATE) * map_size);						// Will set the state of all the tiles in the container to UNEXPLORED.

	debug_visibility_map = new FOW_TILE_STATE[map_size];
	memset(debug_visibility_map, 1, sizeof(FOW_TILE_STATE) * map_size);					// Will set the state of all the tiles in the container to VISIBLE. (For debug purposes).
}*/
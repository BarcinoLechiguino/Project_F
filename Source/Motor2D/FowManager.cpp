#include "Brofiler/Brofiler.h"

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
	if (debug_visibility_map != nullptr && debug_visibility_map != visibility_map)
	{
		delete[] debug_visibility_map;
		debug_visibility_map = nullptr;
	}
	else
	{
		debug_visibility_map = nullptr;
	}

	if (visibility_map_debug_buffer != nullptr && visibility_map_debug_buffer != visibility_map)
	{
		delete[] visibility_map_debug_buffer;
		visibility_map_debug_buffer = nullptr;
	}
	else
	{
		visibility_map_debug_buffer = nullptr;
	}

	if (visibility_map != nullptr)
	{
		delete[] visibility_map;
		visibility_map = nullptr;
	}
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
		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_STATE::KEY_REPEAT)	// FOW DEBUG TOOLS
		{
			App->map->DrawMapGrid();

			DebugFrontier();
			DebugLineOfSight();
		}

		UpdateEntitiesFowManipulation();
		
		/*if (!fow_debug)
		{
			UpdateEntitiesFowManipulation();
		}
		else
		{
			DebugUpdateEntitiesFowManipulation();
		}*/

		UpdateEntitiesVisibility();
	}
	else
	{
		if (tiles_to_reset.size() != 0)											// Safety check in case tiles_to_reset is not empty when the scene transitions out of the gameplay scene.
		{
			tiles_to_reset.clear();
		}
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
		if (!fow_debug)
		{
			visibility_map[(tile_position.y * visibility_map_width) + tile_position.x] = visibility;
		}
		else
		{
			visibility_map_debug_buffer[(tile_position.y * visibility_map_width) + tile_position.x] = visibility;			// DEBUG TOOL
		}
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

FOW_SMOOTHING_STATE FowManager::GetSmoothingStateAt(const iPoint& tile_position)
{
	FOW_SMOOTHING_STATE smoothing_state = FOW_SMOOTHING_STATE::NONE;

	if (GetVisibilityAt(tile_position) /*!= UNEXPLORED*/  == VISIBLE)		// (?)
	{
		smoothing_state = GetInnerSmoothingStateAt(tile_position);
	}
	
	if (GetVisibilityAt(tile_position) == FOGGED)
	{
		smoothing_state = GetOuterSmoothingStateAt(tile_position);
	}

	return smoothing_state;
}

FOW_SMOOTHING_STATE FowManager::GetInnerSmoothingStateAt(const iPoint& tile_position)
{
	bool inner_corner_check = false;

	uchar index = 0;
	uchar tile_state;

	iPoint top_neighbour	= { tile_position.x, tile_position.y - 1 };					// Neighbour aware tile selection. The neighbours are iterated counter-clockwise.
	iPoint left_neighbour	= { tile_position.x - 1, tile_position.y };
	iPoint bottom_neighbour = { tile_position.x, tile_position.y + 1 };
	iPoint right_neighbour	= { tile_position.x + 1, tile_position.y };

	tile_state = GetVisibilityAt(top_neighbour);										// Checking the TOP neighbour.
	if (tile_state != VISIBLE)
	{
		index += 1;
	}

	tile_state = GetVisibilityAt(left_neighbour);										// Checking the LEFT neighbour.
	if (tile_state != VISIBLE)
	{
		index += 2;
	}

	tile_state = GetVisibilityAt(bottom_neighbour);										// Checking the BOTTOM neighbour.
	if (tile_state != VISIBLE)
	{
		index += 4;
	}

	tile_state = GetVisibilityAt(right_neighbour);										// Checking the RIGHT neigbour.
	if (tile_state != VISIBLE)
	{
		index += 8;
	}

	switch (index)
	{
	case 0:
		inner_corner_check = true;														// Tile might be a corner tile, so an additional check is made.
		break;
	case 1:
		return FOW_SMOOTHING_STATE::FOGGED_TOP;
		break;
	case 2:
		return FOW_SMOOTHING_STATE::FOGGED_LEFT;
		break;
	case 3:
		return FOW_SMOOTHING_STATE::FOGGED_INNER_TOP_LEFT;
		break;
	case 4:
		return FOW_SMOOTHING_STATE::FOGGED_BOTTOM;
		break;
	case 5:
		return FOW_SMOOTHING_STATE::NONE;												//TMP? (No tile for this state)
		break;
	case 6:
		return FOW_SMOOTHING_STATE::FOGGED_INNER_DOWN_LEFT;
		break;
	case 7:
		return FOW_SMOOTHING_STATE::FOGGED_LEFT_DEAD_END;
		//return FOW_SMOOTHING_STATE::FOGGED_LEFT;
		break;
	case 8:
		return FOW_SMOOTHING_STATE::FOGGED_RIGHT;
		break;
	case 9:
		return FOW_SMOOTHING_STATE::FOGGED_INNER_TOP_RIGHT;
		break;
	case 10:
		return FOW_SMOOTHING_STATE::NONE;												//TMP? (No tile for this state)
		break;
	case 11:
		return FOW_SMOOTHING_STATE::FOGGED_TOP_DEAD_END;
		//return FOW_SMOOTHING_STATE::FOGGED_TOP;
		break;
	case 12:
		return FOW_SMOOTHING_STATE::FOGGED_INNER_DOWN_RIGHT;
		break;
	case 13:
		return FOW_SMOOTHING_STATE::FOGGED_RIGHT_DEAD_END;
		//return FOW_SMOOTHING_STATE::FOGGED_RIGHT;
		break;
	case 14:
		return FOW_SMOOTHING_STATE::FOGGED_BOTTOM_DEAD_END;
		//return FOW_SMOOTHING_STATE::FOGGED_BOTTOM;
		break;
	case 15:
		return FOW_SMOOTHING_STATE::NONE;												// Maybe use ChangeVisibilityMap(FOGGED/UNEXPLORED)?
		break;
	}

	if (inner_corner_check)
	{
		iPoint top_right_neighbour		= { tile_position.x + 1, tile_position.y - 1 };		// Neighbour aware tile selection. The neighbours are iterated counter-clockwise.
		iPoint top_left_neighbour		= { tile_position.x - 1, tile_position.y - 1 };
		iPoint bottom_right_neighbour	= { tile_position.x + 1, tile_position.y + 1 };
		iPoint bottom_left_neighbour	= { tile_position.x - 1, tile_position.y + 1 };
		
		if (GetVisibilityAt(top_right_neighbour) == UNEXPLORED)								// Checking the TOP neighbour.
		{
			return FOW_SMOOTHING_STATE::FOGGED_OUTER_TOP_RIGHT;
		}
		else if (GetVisibilityAt(top_left_neighbour) == UNEXPLORED)							// Checking the LEFT neighbour.
		{
			return FOW_SMOOTHING_STATE::FOGGED_OUTER_TOP_LEFT;
		}
		else if (GetVisibilityAt(bottom_right_neighbour) == UNEXPLORED)						// Checking the BOTTOM neighbour.
		{
			return FOW_SMOOTHING_STATE::FOGGED_OUTER_DOWN_RIGHT;
		}
		else if (GetVisibilityAt(bottom_left_neighbour) == UNEXPLORED)						// Checking the RIGHT neighbour.
		{
			return FOW_SMOOTHING_STATE::FOGGED_OUTER_DOWN_LEFT;
		}
		else
		{
			return FOW_SMOOTHING_STATE::NONE;
		}
	}
}

FOW_SMOOTHING_STATE FowManager::GetOuterSmoothingStateAt(const iPoint& tile_position)
{
	bool outer_corner_check = false;
	
	uchar index = 0;
	uchar tile_state;

	iPoint top_neighbour	= { tile_position.x, tile_position.y - 1 };					// Neighbour aware tile selection. The neighbours are iterated counter-clockwise.
	iPoint left_neighbour	= { tile_position.x - 1, tile_position.y };
	iPoint bottom_neighbour = { tile_position.x, tile_position.y + 1 };
	iPoint right_neighbour	= { tile_position.x + 1, tile_position.y };

	tile_state = GetVisibilityAt(top_neighbour);										// Checking the TOP neighbour.
	if (tile_state == UNEXPLORED)
	{
		index += 1;
	}

	tile_state = GetVisibilityAt(left_neighbour);										// Checking the LEFT neighbour.
	if (tile_state == UNEXPLORED)
	{
		index += 2;
	}

	tile_state = GetVisibilityAt(bottom_neighbour);										// Checking the BOTTOM neighbour.
	if (tile_state == UNEXPLORED)
	{
		index += 4;
	}

	tile_state = GetVisibilityAt(right_neighbour);										// Checking the RIGHT neigbour.
	if (tile_state == UNEXPLORED)
	{
		index += 8;
	}

	switch (index)
	{
	case 0:
		outer_corner_check = true;														// A tile in the frontier might not be FOGGED, so more checks will be performed.
		break;
	case 1:
		return FOW_SMOOTHING_STATE::UNXTOFOG_TOP;
		break;
	case 2:
		return FOW_SMOOTHING_STATE::UNXTOFOG_LEFT;
		break;
	case 3:
		return FOW_SMOOTHING_STATE::UNXTOFOG_INNER_TOP_LEFT;
		break;
	case 4:
		return FOW_SMOOTHING_STATE::UNXTOFOG_BOTTOM;
		break;
	case 5:
		return FOW_SMOOTHING_STATE::NONE;												// TMP? (No tile for this state)
		break;
	case 6:
		return FOW_SMOOTHING_STATE::UNXTOFOG_INNER_BOTTOM_LEFT;
		break;
	case 7:
		return FOW_SMOOTHING_STATE::UNXTOFOG_LEFT_DEAD_END;
		break;
	case 8:
		return FOW_SMOOTHING_STATE::UNXTOFOG_RIGHT;
		break;
	case 9:
		return FOW_SMOOTHING_STATE::UNXTOFOG_INNER_TOP_RIGHT;
		break;
	case 10:
		return FOW_SMOOTHING_STATE::NONE;												// TMP? (No tile for this state)
		break;
	case 11:
		return FOW_SMOOTHING_STATE::UNXTOFOG_TOP_DEAD_END;
		break;
	case 12:
		return FOW_SMOOTHING_STATE::UNXTOFOG_INNER_BOTTOM_RIGHT;
		break;
	case 13:
		return FOW_SMOOTHING_STATE::UNXTOFOG_RIGHT_DEAD_END;
		break;
	case 14:
		return FOW_SMOOTHING_STATE::UNXTOFOG_LEFT_DEAD_END;
		break;
	case 15:
		return FOW_SMOOTHING_STATE::NONE;												// Maybe use ChangeVisibilityMap(FOGGED/UNEXPLORED)?
		break;
	}

	if (outer_corner_check)
	{
		iPoint top_right_neighbour		= { tile_position.x + 1, tile_position.y - 1 };
		iPoint top_left_neighbour		= { tile_position.x - 1, tile_position.y - 1 };
		iPoint bottom_right_neighbour	= { tile_position.x + 1, tile_position.y + 1 };
		iPoint bottom_left_neighbour	= { tile_position.x - 1, tile_position.y + 1 };
		
		if (GetVisibilityAt(top_right_neighbour) == UNEXPLORED)								// Checking the TOP neighbour.
		{
			return FOW_SMOOTHING_STATE::UNXTOFOG_OUTER_TOP_RIGHT;
		}
		else if (GetVisibilityAt(top_left_neighbour) == UNEXPLORED)							// Checking the LEFT neighbour.
		{
			return FOW_SMOOTHING_STATE::UNXTOFOG_OUTER_TOP_LEFT;
		}
		else if (GetVisibilityAt(bottom_right_neighbour) == UNEXPLORED)						// Checking the BOTTOM neighbour.
		{
			return FOW_SMOOTHING_STATE::UNXTOFOG_OUTER_BOTTOM_RIGHT;
		}
		else if (GetVisibilityAt(bottom_left_neighbour) == UNEXPLORED)						// Checking the RIGHT neighbour.
		{
			return FOW_SMOOTHING_STATE::UNXTOFOG_OUTER_BOTTOM_LEFT;
		}
		else
		{
			return FOW_SMOOTHING_STATE::NONE;
		}
	}
}

SDL_Rect FowManager::GetFowTileRect(const uchar& visibility_state)
{
	SDL_Rect ret;

	ret.w = 64;
	ret.h = 64;
	ret.x = visibility_state * ret.w;												// TMP. Adapt to be able to use smoothed tiles.
	ret.y = 0;

	return ret;
}

FowEntity* FowManager::CreateFowEntity(const iPoint& tile_position, bool provides_visibility)
{
	FowEntity* fow_entity = nullptr;

	fow_entity = new FowEntity(tile_position, provides_visibility);

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

void FowManager::ClearFowEntityLineOfSight(FowEntity* fow_entity_to_clear)					// Mind fow frontier_tile conflicts.
{
	std::vector<iPoint>::iterator tile = fow_entity_to_clear->line_of_sight.begin();

	if (!fow_debug)
	{
		for (; tile != fow_entity_to_clear->line_of_sight.end(); ++tile)
		{
			tiles_to_reset.push_back((*tile));													// All the tiles in the line_of_sight of the destroyed entity will be set to be FOGGED or UNEXPLORED.
		}
	}
	else
	{
		for (; tile != fow_entity_to_clear->line_of_sight.end(); ++tile)
		{
			debug_tiles_to_reset.push_back((*tile));													// All the tiles in the line_of_sight of the destroyed entity will be set to be FOGGED or UNEXPLORED.
		}
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
		if ((*next(tile)).y == (*tile).y && ((*next(tile)).x - (*tile).x) > 1)			// If the next frontier_tile is in the same column and the x distance with is more than 1.
		{
			int width = (*next(tile)).x - (*tile).x;									// Get row width between the  current and the next frontier_tile.

			for (int i = 0; i < width; ++i)
			{
				line_of_sight.push_back({ (*tile).x + i ,(*tile).y });					// Push back each frontier_tile in the row to line_of_sight
			}
		}
		else
		{
			line_of_sight.push_back(*tile);
		}
	}

	return line_of_sight;
}

void FowManager::DebugUpdateEntitiesFowManipulation()
{
	std::vector<FowEntity*>::iterator item = fow_entities.begin();

	for (; item != fow_entities.end(); ++item)
	{
		if ((*item)->provides_visibility)
		{
			if ((*item)->has_moved)
			{
				std::vector<iPoint>::iterator tile = (*item)->line_of_sight.begin();

				for (; tile != (*item)->line_of_sight.end(); ++tile)
				{
					(*tile) += (*item)->motion;															// The frontier_tile is updated with the fow_entity's motion.
				}

				
				tile = (*item)->frontier.begin();

				for (; tile != (*item)->frontier.end(); ++tile)
				{
					(*tile) += (*item)->motion;
				}
			}
		}
	}
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

	for (; item != fow_entities.end(); ++item)
	{
		if ((*item)->provides_visibility)
		{
			if ((*item)->has_moved)
			{
				UpdateEntityLineOfSight((*item));
			}
		}
	}

	
	UpdateTilesToReset();


	item = fow_entities.begin();															// The fow_entities vector is iterated again to avoid any visibility errors.

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

		//Smooth the edges of the fow_tiles.
		SmoothEntityFrontierEdges((*item));
	}
}

void FowManager::UpdateEntityLineOfSight(FowEntity* entity_to_update)
{
	BROFILER_CATEGORY("UpdateEntityLineOfSight", Profiler::Color::Fuchsia)

	std::vector<iPoint>::iterator tile = entity_to_update->line_of_sight.begin();

	for (; tile != entity_to_update->line_of_sight.end(); ++tile)
	{
		if (scouting_trail)																				// The current frontier_tile is set to FOGGED or UNEXPLORED.
		{
			ChangeVisibilityMap((*tile), FOGGED);
		}
		else
		{
			ChangeVisibilityMap((*tile), UNEXPLORED);
		}

		(*tile) += entity_to_update->motion;															// The frontier_tile is updated with the fow_entity's motion.

		ChangeVisibilityMap((*tile), VISIBLE);															// The new current frontier_tile is set to VISIBLE.
	}

	entity_to_update->has_moved = false;
}

void FowManager::UpdateTilesToReset()
{
	if (tiles_to_reset.size() != 0)														// To avoid frontier_tile conflicts, the tiles that are to be reset are iterated before the second check.
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

	if (debug_tiles_to_reset.size() != 0)												// Will only store tiles when an entity that provided visibility is destroyed in fow_debug mode.
	{
		for (int i = 0; i < debug_tiles_to_reset.size(); ++i)
		{
			if (GetVisibilityAt(debug_tiles_to_reset[i]) == UNEXPLORED)					// If the given tile in visibility_map is UNEXPLORED. (In fow_debug mode debug_visibility_map is used)
			{
				ChangeVisibilityMap(debug_tiles_to_reset[i], UNEXPLORED);
			}
			else
			{
				ChangeVisibilityMap(debug_tiles_to_reset[i], FOGGED);
			}
		}
		
		debug_tiles_to_reset.clear();
	}
}

void FowManager::SmoothEntityFrontierEdges(FowEntity* fow_entity)
{
	std::vector<iPoint> previous_frontier = fow_entity->frontier;
	
	std::vector<iPoint>::iterator frontier_tile = fow_entity->frontier.begin();

	for (; frontier_tile != fow_entity->frontier.end(); ++frontier_tile)
	{
		(*frontier_tile) += fow_entity->motion;
	}

	SmoothEntityInnerFrontierEdges(fow_entity->frontier);

	SmoothEntityOuterFrontierEdges(previous_frontier);
}

void FowManager::SmoothEntityInnerFrontierEdges(std::vector<iPoint> inner_frontier)
{
	std::vector<iPoint> inner_corners;

	std::vector<iPoint>::iterator tile = inner_frontier.begin();

	for (; tile != inner_frontier.end(); ++tile)
	{
		uchar index = 0;
		uchar tile_state;

		iPoint top_neighbour	= { (*tile).x, (*tile).y - 1 };								// Neighbour aware tile selection. The neighbours are iterated counter-clockwise.
		iPoint left_neighbour	= { (*tile).x - 1, (*tile).y };
		iPoint bottom_neighbour	= { (*tile).x, (*tile).y + 1 };
		iPoint right_neighbour	= { (*tile).x + 1, (*tile).y };

		tile_state = GetVisibilityAt(top_neighbour);										// Checking the TOP neighbour.
		if (tile_state != VISIBLE)
		{
			index += 1;
		}

		tile_state = GetVisibilityAt(left_neighbour);										// Checking the LEFT neighbour.
		if (tile_state != VISIBLE)
		{
			index += 2;
		}

		tile_state = GetVisibilityAt(bottom_neighbour);										// Checking the BOTTOM neighbour.
		if (tile_state != VISIBLE)
		{
			index += 4;
		}

		tile_state = GetVisibilityAt(right_neighbour);										// Checking the RIGHT neigbour.
		if (tile_state != VISIBLE)
		{
			index += 8;
		}

		switch (index)
		{
		case 0:
			inner_corners.push_back((*tile));												// A tile in the frontier might not be FOGGED, so more checks will be performed.
			break;
		case 1:

			break;
		case 2:

			break;
		case 3:

			break;
		case 4:

			break;
		case 5:

			break;
		case 6:

			break;
		case 7:

			break;
		case 8:

			break;
		case 9:

			break;
		case 10:

			break;
		case 11:

			break;
		case 12:

			break;
		case 13:

			break;
		case 14:

			break;
		case 15:

			break;
		}
	}

	std::vector<iPoint>::iterator corner_tile = inner_corners.begin();

	for (; corner_tile != inner_corners.end(); ++corner_tile)
	{
		iPoint top_neighbour	= { (*corner_tile).x, (*corner_tile).y - 1 };
		iPoint left_neighbour	= { (*corner_tile).x - 1, (*corner_tile).y };
		iPoint bottom_neighbour = { (*corner_tile).x, (*corner_tile).y + 1 };
		iPoint right_neighbour	= { (*corner_tile).x + 1, (*corner_tile).y };

		if (GetVisibilityAt(top_neighbour) == UNEXPLORED)									// Checking the TOP neighbour.
		{

		}
		else if (GetVisibilityAt(left_neighbour) == UNEXPLORED)								// Checking the LEFT neighbour.
		{

		}
		else if (GetVisibilityAt(bottom_neighbour) == UNEXPLORED)							// Checking the BOTTOM neighbour.
		{

		}
		else if (GetVisibilityAt(right_neighbour) == UNEXPLORED)							// Checking the RIGHT neighbour.
		{

		}
		else
		{
			continue;
		}
	}
}

void FowManager::SmoothEntityOuterFrontierEdges(std::vector<iPoint> outer_frontier)
{
	std::vector<iPoint> outer_corners;

	std::vector<iPoint>::iterator tile = outer_frontier.begin();

	for (; tile != outer_frontier.end(); ++tile)
	{
		
	}
		

	std::vector<iPoint>::iterator corner_tile = outer_corners.begin();

	for (; corner_tile != outer_corners.end(); ++corner_tile)
	{
		iPoint top_neighbour	= { (*corner_tile).x, (*corner_tile).y - 1 };
		iPoint left_neighbour	= { (*corner_tile).x - 1, (*corner_tile).y };
		iPoint bottom_neighbour = { (*corner_tile).x, (*corner_tile).y + 1 };
		iPoint right_neighbour	= { (*corner_tile).x + 1, (*corner_tile).y };

		if (GetVisibilityAt(top_neighbour) == UNEXPLORED)									// Checking the TOP neighbour.
		{

		}
		else if (GetVisibilityAt(left_neighbour) == UNEXPLORED)								// Checking the LEFT neighbour.
		{

		}
		else if (GetVisibilityAt(bottom_neighbour) == UNEXPLORED)							// Checking the BOTTOM neighbour.
		{

		}
		else if (GetVisibilityAt(right_neighbour) == UNEXPLORED)							// Checking the RIGHT neighbour.
		{

		}
		else
		{
			continue;
		}
	}
}

void FowManager::InitFowManager()
{
	config.load_file("config.xml");

	pugi::xml_node fow = config.child("config").child("fog_of_war");

	fow_tex = App->tex->Load(fow.child("fow_tex").attribute("path").as_string());
}

// --- FOW_Entity methods ---
FowEntity::FowEntity() : position({ 0, 0 }), motion({ 0, 0 }), is_visible(false), provides_visibility(false), has_moved(false)
{

}

FowEntity::FowEntity(const iPoint& position, const bool provides_visibility) 
	: position(position)
	, motion({ 0, 0 })
	, is_visible(false)																	// TMP. Check if it is worth it to make it a constructor parameter.
	, provides_visibility(provides_visibility)
	, has_moved(false)
{

}

void FowEntity::CleanUp()
{
	if (provides_visibility)
	{
		App->fow_manager->ClearFowEntityLineOfSight(this);
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
	else
	{
		motion = { 0, 0 };
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
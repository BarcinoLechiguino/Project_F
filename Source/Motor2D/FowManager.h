#ifndef __FOW_MANAGER_H__
#define __FOW_MANAGER_H__

#include "Module.h"

//#define INVALID_VISIBILITY_CODE 255									// If an unsigned char (uchar) goes into the negatives, its value will be set to 255 (max possible for an uchar).
#define UNEXPLORED 0
#define	FOGGED 1
#define VISIBLE 255														// Cannot be 1 because that would alter the neighbour aware tile selection algorithm.

enum class FOW_SMOOTHING_STATE
{
	NONE,
	UNKNOWN,

	// UNEXPLORED SMOOTHING
	UNEXPLORED_TOP,
	UNEXPLORED_BOTTOM,
	UNEXPLORED_RIGHT,
	UNEXPLORED_LEFT,

	UNEXPLORED_INNER_TOP_RIGHT,				// Inner Top-Right Corner
	UNEXPLORED_INNER_TOP_LEFT,				// Inner Top-Left Corner
	UNEXPLORED_INNER_BOTTOM_RIGHT,			// Inner Bottom-Right Corner
	UNEXPLORED_INNER_BOTTOM_LEFT,			// Inner Bottom-Left Corner

	UNEXPLORED_TOP_DEAD_END,
	UNEXPLORED_BOTTOM_DEAD_END,
	UNEXPLORED_RIGHT_DEAD_END,
	UNEXPLORED_LEFT_DEAD_END,

	UNEXPLORED_OUTER_TOP_RIGHT,				// Outer Top-Right Corner
	UNEXPLORED_OUTER_TOP_LEFT,				// Outer Top-Left Corner
	UNEXPLORED_OUTER_BOTTOM_RIGHT,			// Outer Bottom-Right Corner
	UNEXPLORED_OUTER_BOTTOM_LEFT,			// Outer Bottom-Left Corner

	// FOGGED SMOOTHING
	FOGGED_TOP,
	FOGGED_BOTTOM,
	FOGGED_RIGHT,
	FOGGED_LEFT,

	FOGGED_INNER_TOP_RIGHT,					// Inner Top-Right Corner
	FOGGED_INNER_TOP_LEFT,					// Inner Top-Left Corner
	FOGGED_INNER_DOWN_RIGHT,				// Inner Bottom-Right Corner
	FOGGED_INNER_DOWN_LEFT,					// Inner Bottom-Left Corner

	FOGGED_TOP_DEAD_END,
	FOGGED_BOTTOM_DEAD_END,
	FOGGED_RIGHT_DEAD_END,
	FOGGED_LEFT_DEAD_END,

	FOGGED_OUTER_TOP_RIGHT,					// Outer Top-Right Corner
	FOGGED_OUTER_TOP_LEFT,					// Outer Top-Left Corner
	FOGGED_OUTER_DOWN_RIGHT,				// Outer Bottom-Right Corner
	FOGGED_OUTER_DOWN_LEFT,					// Outer Bottom-Left Corner
	
	// UNEXPLORED TO FOGGED SMOOTHING
	UNXTOFOG_TOP,
	UNXTOFOG_BOTTOM,
	UNXTOFOG_RIGHT,
	UNXTOFOG_LEFT,

	UNXTOFOG_INNER_TOP_RIGHT,				// Inner Top-Right Corner
	UNXTOFOG_INNER_TOP_LEFT,				// Inner Top-Left Corner
	UNXTOFOG_INNER_BOTTOM_RIGHT,			// Inner Bottom-Right Corner
	UNXTOFOG_INNER_BOTTOM_LEFT,				// Inner Bottom-Left Corner

	UNXTOFOG_TOP_DEAD_END,
	UNXTOFOG_BOTTOM_DEAD_END,
	UNXTOFOG_RIGHT_DEAD_END,
	UNXTOFOG_LEFT_DEAD_END,

	UNXTOFOG_OUTER_TOP_RIGHT,				// Outer Top-Right Corner
	UNXTOFOG_OUTER_TOP_LEFT,				// Outer Top-Left Corner
	UNXTOFOG_OUTER_BOTTOM_RIGHT,			// Outer Bottom-Right Corner
	UNXTOFOG_OUTER_BOTTOM_LEFT,				// Outer Bottom-Left Corner
};

struct FowEntity
{
	FowEntity();
	FowEntity(const iPoint& position, const bool provides_visibility);

	void CleanUp();

	void SetPos(const iPoint& new_position);

	iPoint position;
	iPoint motion;

	bool is_visible;
	bool provides_visibility;
	bool has_moved;

	std::vector<iPoint> frontier;
	std::vector<iPoint> line_of_sight;
};

class FowManager : public Module
{
public:
	FowManager();
	~FowManager();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

public:
	void SetVisibilityMap(const int& width, const int& height);													// Will create the the visibility_map along the debug_visibility_map.
	void SwapVisibilityMaps();																					// Will swap the visibility_map with the debug_visibility_map.
	void ResetVisibilityMap();																					// Will reset the all map containers and set them back to their original state.
	void ClearVisibilityMapContainers();																		// Will delete all visibility map containers. (visibility_map...)

	uchar GetVisibilityAt(const iPoint& tile_position);															// Will return the current visibility state of the given FOW tile.
	
	FOW_SMOOTHING_STATE GetSmoothingStateAt(const iPoint& tile_position);
	FOW_SMOOTHING_STATE GetInnerSmoothingStateAt(const iPoint& tile_position);
	FOW_SMOOTHING_STATE GetOuterSmoothingStateAt(const iPoint& tile_position);

	SDL_Rect GetFowTileRect(const uchar& visibility_state);														// Will get the corresponding FOW tile section rect for a given visibility state.

	FowEntity* CreateFowEntity(const iPoint& tile_position, bool provides_visibility);
	void DeleteFowEntity(FowEntity* fow_entity_to_delete);														// Will delete the given FowEntity from the fow_entities vector.
	void DestroyFowEntities();																					// Will delete all fow_entities and will clear the fow_entities vector.

	void ClearFowEntityLineOfSight(FowEntity* fow_entity_to_clear);												// Will add the tiles inside the line_of_sight of an entity to tiles_to_reset.

	std::vector<iPoint> CreateRectangularFrontier(const int& width, const int& height, const iPoint& center);	// Will create a rectangular frontier outside which visibility won't be provided.
	std::vector<iPoint> CreateCircularFrontier(const uint& radius, const iPoint& center);						// Will create a circular frontier which visibility won't be provided.
	std::vector<iPoint> GetLineOfSight(const std::vector<iPoint>& frontier);									// Will fill the tiles inside a given frontier and return a vector with them.

private:
	void InitFowManager();

	void UpdateEntitiesVisibility();															// Will update the is_visible bool of all FowEntities by checking the fow_state of their tile_pos.
	
	void UpdateEntitiesFowManipulation();														// Will update all the tiles affected by the visibility provided by all FowEntities.
	void UpdateEntityLineOfSight(FowEntity* entity_to_update);									// Will update the line_of_sight of a given fow_entity. Only applicable to non-static entities.
	void UpdateTilesToReset();																	// Will update the tiles that have been set to be reset after an entity has been destroyed.

	void SmoothEntityFrontierEdges(FowEntity* fow_entity);										//
	void SmoothEntityInnerFrontierEdges(std::vector<iPoint> inner_frontier);					//
	void SmoothEntityOuterFrontierEdges(std::vector<iPoint> outer_frontier);					//

	void ChangeVisibilityMap(const iPoint& tile_position, const uchar& visibility);
	bool CheckMapBoundaries(const iPoint& tile_position);

	void DebugUpdateEntitiesFowManipulation();													// Will update all entities  line_of_sight and frontier position in fow_debug mode.
	void DebugLineOfSight();
	void DebugFrontier();

public:
	SDL_Texture* fow_tex;

	bool scouting_trail;

	bool fow_debug;

private:
	pugi::xml_document config;
	
	int visibility_map_width;
	int visibility_map_height;

	uchar* visibility_map;										// 2D container that will store the FOW state of each tile in the map.
	uchar* debug_visibility_map;								// Visibility map used for debug pursposes.	All the tiles will be VISIBLE.
	uchar* visibility_map_debug_buffer;							// Buffer that will allow to swap between the visibility_map and debug_visibility_without issue.

	FOW_SMOOTHING_STATE smoothing_state;
	
	std::vector<FowEntity*> fow_entities;
	std::vector<iPoint> tiles_to_reset;							// Will store the tiles that need to be set back to FOGGED or UNEXPLORED when an Entity that provided visibility is destroyed.
	std::vector<iPoint> debug_tiles_to_reset;					// Will store the tiles that need to be set back to UNEXPLORED when an Entity that provided visibility is destroyed in debug mode.

	SDL_Texture* frontier_debug_tex;
	SDL_Texture* line_of_sight_debug_tex;
};

#endif // !__FOW_MANAGER_H__
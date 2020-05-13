#ifndef __FOW_MANAGER_H__
#define __FOW_MANAGER_H__

#include "Module.h"

//#define INVALID_VISIBILITY_CODE 255									// If an unsigned char (uchar) goes into the negatives, its value will be set to 255 (max possible for an uchar).
#define UNEXPLORED 0
#define	FOGGED 1
#define VISIBLE 255														// Cannot be 1 because that would alter the neighbour aware tile selection algorithm.

enum class FOW_SMOOTHING_STATE
{

};

enum class UNEXPLORED_SMOOTHING_STATE
{

};

enum class FOGGED_SMOOTHING_STATE
{

};

enum class UNEXPLORED_TO_FOGGED_SMOOTHING_STATE
{

};

struct FowEntity
{
	FowEntity();
	FowEntity(const iPoint& position, const bool provides_visibility, const bool is_static);

	void CleanUp();

	void SetPos(const iPoint& new_position);

	iPoint position;
	iPoint motion;

	bool is_visible;
	bool is_static;
	bool provides_visibility;
	bool visibility_initialized;
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
	void SetVisibilityMap(const int& width, const int& height);									// Will create the the visibility_map along the debug_visibility_map.
	void SwapVisibilityMaps();																	// Will swap the visibility_map with the debug_visibility_map.
	void ResetVisibilityMap();																	// Will reset the visibility map and both the other containers and set it back to the first state.
	void ClearVisibilityMapContainers();														// Will delete all visibility map containers. (visibility_map...)

	uchar GetVisibilityAt(const iPoint& tile_position);											// Will return the current visibility state of the given FOW tile.
	
	SDL_Rect GetFowTileRect(const uchar& visibility);											// Will get the corresponding FOW sprite section rect for the given visibility state.

	FowEntity* CreateFowEntity(const iPoint& tile_position, bool provides_visibility, bool is_static);
	void DeleteFowEntity(FowEntity* fow_entity_to_delete);										// Will delete the given FowEntity from the fow_entities vector.
	void DestroyFowEntities();																	// Will delete all fow_entities and will clear the fow_entities vector.
	void ClearFowEntityLineOfSight(FowEntity* fow_entity_to_clear);

	std::vector<iPoint> CreateRectangularFrontier(const int& width, const int& height, const iPoint& center);	// Will create a rectangular frontier outside which visibility won't be provided.
	
	std::vector<iPoint> CreateCircularFrontier(const uint& radius, const iPoint& center);		// Will create a circular frontier where a given entity will stop providing visibility.
	std::vector<iPoint> GetLineOfSight(const std::vector<iPoint>& frontier);					// Will fill the tiles inside a given frontier and return the vector with all those tiles.

private:
	void InitFowManager();

	void UpdateEntitiesVisibility();															// Will update the is_visible bool of all FowEntities by checking the fow_state of their tile_pos.
	void UpdateEntitiesFowManipulation();														// Will update all the tiles affected by the visibility provided by all FowEntities.
	void UpdateEntityLineOfSight(FowEntity* entity_to_update);									// Will update the line_of_sight of a given fow_entity. Only applicable to non-static entities.

	void ChangeVisibilityMap(const iPoint& tile_position, const uchar& visibility);
	bool CheckMapBoundaries(const iPoint& tile_position);

	bool TileIsInsideLineOfSight(const iPoint& tile_position, const std::vector<iPoint>& line_of_sight);		// TilesInsideTilesToUpdate could be merged with this one.

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

	SDL_Texture* frontier_debug_tex;
	SDL_Texture* line_of_sight_debug_tex;
};

#endif // !__FOW_MANAGER_H__
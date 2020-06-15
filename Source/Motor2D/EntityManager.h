#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

//#include <algorithm>
#include "Module.h"
#include "Point.h"

#include "EntityQuadTree.h"

#define MAX_ENTITIES 400
#define MAX_ENEMIES 200

struct SDL_Texture;

class Entity;
enum class ENTITY_TYPE;

class EntityManager : public Module
{
public:
	EntityManager();
	~EntityManager();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void DrawEntities();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:																					// -------------- ENTITY CREATION & DESTRUCTION METHODS --------------
	Entity* CreateEntity(ENTITY_TYPE type, int x = 0, int y = 0, int level = 1);		// Crates a new entity depending on the ENTITY_TYPE passed as argument. 
	void DestroyEntities();																// Calls the CleanUp() method of each entity and then it clears the entities list.
	void DeleteEntity(Entity* entity_to_delete);										// Will delete and destroy the entity passed as argument.

	void LoadEntityTextures();															// Will load all entity textures.
	void UnLoadEntityTextures();														// Will unload and delete all entity textures.
	void LoadEntityAudios();															// Will load all entity sfxs.
	void UnLoadEntityAudios();															// Will unload all entity sfxs.

public:																					// -------------- ENTITY GET TEXTURE METHODS --------------
	SDL_Texture* GetGathererTexture() const;
	SDL_Texture* GetScoutTexture() const;
	SDL_Texture* GetInfantryTexture() const;
	SDL_Texture* GetHeavyTexture() const;
	SDL_Texture* GetEnemyGathererTexture() const;
	SDL_Texture* GetEnemyScoutTexture() const;
	SDL_Texture* GetEnemyInfantryTexture() const;
	SDL_Texture* GetEnemyHeavyTexture() const;
	
	SDL_Texture* GetTownHallTexture() const;
	SDL_Texture* GetBarracksTexture() const;
	SDL_Texture* GetWallTexture() const;
	SDL_Texture* GetEnemyTownHallTexture() const;
	SDL_Texture* GetEnemyBarracksTexture() const;
	SDL_Texture* GetEnemyWallTexture() const;

	SDL_Texture* GetRockTexture() const;
	SDL_Texture* GetTreeTexture() const;
	SDL_Texture* GetObeliskTexture() const;

	SDL_Texture* GetBoulderTexture() const;

	SDL_Texture* GetCenterPointTexture() const;

public:																					// -------------- ENTITY CONDITIONAL CHECK METHODS --------------
	bool IsAllyEntity(Entity* entity);													// Method that will return true if the entity passed as argument is a enemy entity.
	bool IsEnemyEntity(Entity* entity);													// Method that will return true if the entity passed as argument is a enemy entity.

	bool IsUnit(Entity* entity);														// Method that will return true if the entity passed as argument is a unit. (Expand to IsAllyUnit())
	bool IsAllyUnit(Entity* entity);													// Temporal methods, might be unnecessary down the line.
	bool IsEnemyUnit(Entity* entity);													// Temporal methods, might be unnecessary down the line.

	bool IsGatherer(Entity* enitity);													// Method that will return true if the entity passed as argument is an Ally Gatherer Unit.
	bool IsScout(Entity* enitity);														// Method that will return true if the entity passed as argument is an Ally Scout Unit.
	bool IsInfantry(Entity* entity);													// Method that will return true if the entity passed as argument is an Ally Infantry Unit.
	bool IsHeavy(Entity* enitity);														// Method that will return true if the entity passed as argument is an Ally Heavy Unit.
	
	bool IsBuilding(Entity* entity);													// Method that will return true if the entity passed as argument is a building entity.
	bool IsResource(Entity* entity);													// Method that will return true if the entity passed as argument is a resource entity.
	bool IsObstacle(Entity* entity);													// Method that will return true if the entity passed as argument is an obstacle entity.
	
	bool InViewport(Entity* entity);													// Method that will return true if the entity passed as argument is currently inside the viewport.

public:																								// -------------- ENTITY MAP METHODS -------------- 
	void SetEntityMap(int width, int height);														// Method that will allocate the necessary memory for the entity_map.
	void ChangeEntityMap(const iPoint& tile_position, Entity* entity, bool set_to_null = false);	// Method that will modify the entity map when a unit or building is spawned.
	void OrderEntities();

	void SetDeadTarget(Entity* entity);

	bool CheckEntityMapBoundaries(const iPoint& tile_position) const;								// Method that will return true if the tile passed as argument is inside the entity_map.

	Entity* GetEntityAt(const iPoint& tile_position) const;											// Method that will return whichever entity is at the given position.
	iPoint GetEntityPos(Entity* entity);

	bool CheckTileAvailability(const iPoint& tile_position, Entity* entity);						// Method that will return true whenever the space required by an entity is available.
	bool CheckSingleTileAvailability(const iPoint& tile_position);									// Method that will return true if the given tile is not occupied by any entity.

public:
	SDL_Texture*					gatherer_tex;
	SDL_Texture*					scout_tex;
	SDL_Texture*					infantry_tex;
	SDL_Texture*					heavy_tex;
	SDL_Texture*					enemy_gatherer_tex;
	SDL_Texture*					enemy_scout_tex;
	SDL_Texture*					enemy_infantry_tex;
	SDL_Texture*					enemy_heavy_tex;
	
	SDL_Texture*					townhall_tex;
	SDL_Texture*					barracks_tex;
	SDL_Texture*					wall_tex;
	SDL_Texture*					enemy_townhall_tex;
	SDL_Texture*					enemy_barracks_tex;
	SDL_Texture*					enemy_wall_tex;

	SDL_Texture*					rock_tex;
	SDL_Texture*					tree_tex;
	SDL_Texture*					obelisk_tex;

	SDL_Texture*					boulder_tex;
	
	SDL_Texture*					center_point_debug;
	SDL_Texture*					select_circle_tex;
	SDL_Texture*					select_townhall_tex;
	SDL_Texture*					select_barracks_tex;

	uint							gatherer_moving_fx;
	uint							gatherer_gathering_fx;
	uint							gatherer_gathering_finished_fx;
	uint							infantry_moving_fx;
	uint							infantry_shooting_fx;
	uint							heavy_shooting_fx;
	uint							townhall_clicked_fx;
	uint							barracks_clicked_fx;
	uint							building_constructing_fx;
	uint							building_constructing_finished_fx;
	uint							building_recruiting_finished_fx;
	uint							building_upgrading_finished_fx;
	uint							explosion_units_die_fx;
	

	std::vector<Entity*>			entities;	
	std::vector<Entity*>			entities_in_screen;
	
	Entity**						entity_map;			//Array that will be used to keep track at which position are all entities at all times.
	int								entity_map_width;	//Width of the entity_map.
	int								entity_map_height;	//Height of the entity_map.

	float							accumulated_time;	//Accumulates dt as time goes on.
	float							cycle_length;		//How much time needs to pass / be accumulated before running a cycle. 
	bool							do_logic;			//Keeps track whether or not the entity needs to do it's logic (pathfinding...)
	
	bool							debug_center_point;

	uint							resource_data;
	uint							resource_electricity;
	uint							resource_bits;
	
	uint							kill_count;
};
#endif // __ENTITY_MANAGER_H__
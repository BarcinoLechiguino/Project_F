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

public:
	Entity* CreateEntity(ENTITY_TYPE type, int x = 0, int y = 0, int level = 1);		//Crates a new entity depending on the ENTITY_TYPE passed as argument. 
	void DestroyEntities();																//Calls the CleanUp() method of each entity and then it clears the entities list.
	void DeleteEntity(Entity* entity_to_delete);

	void LoadEntityTextures();
	void UnLoadEntityTextures();
	void LoadEntityAudios();
	void UnLoadEntityAudios();

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
	SDL_Texture* GetBitsTexture() const;

	SDL_Texture* GetCenterPointTexture() const;

	bool IsAllyEntity(Entity* entity);													//Method that will return true if the entity passed as argument is a enemy entity.
	bool IsEnemyEntity(Entity* entity);													//Method that will return true if the entity passed as argument is a enemy entity.

	bool IsUnit(Entity* entity);														//Method that will return true if the entity passed as argument is a unit. (Expand to IsAllyUnit())
	bool IsAllyUnit(Entity* entity);													// Temporal methods, might be unnecessary down the line.
	bool IsEnemyUnit(Entity* entity);													// Temporal methods, might be unnecessary down the line.

	bool IsGatherer(Entity* enitity);
	bool IsScout(Entity* enitity);
	bool IsInfantry(Entity* entity);
	bool IsHeavy(Entity* enitity);
	
	bool IsBuilding(Entity* entity);													//Method that will return true if the entity passed as argument is a building.
	bool IsResource(Entity* entity);													//Method that will return true if the entity passed as argument is a resource.
	
	bool InViewport(Entity* entity);

	void SetEntityMap(int width, int height);											//Method that will allocate the necessary memory for the entity_map.
	void ChangeEntityMap(const iPoint& pos, Entity* entity, bool set_to_null = false);	//Method that will modify the entity map when a unit or building is spawned.
	void OrderEntities();

	bool CheckEntityMapBoundaries(const iPoint& pos) const;								//Method that will check whether or not the position passed as argument is inside the entity_map's bounds.

	Entity* GetEntityAt(const iPoint& pos) const;										//Method that will return whichever entity is at the given position.
	iPoint GetEntityPos(Entity* entity);

	bool CheckTileAvailability(const iPoint& pos, Entity* entity);						//Method that will return true whenever the space required by an entity to spawn is available.

	void OnCollision(Collider* C1, Collider* C2);

public:
	pugi::xml_document				config_file;
	pugi::xml_node					config;

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
	SDL_Texture*					bits_tex;
	
	SDL_Texture*					center_point_debug;

	uint							gatherer_movement_fx;
	uint							gather_fx;
	uint							finished_gather_fx;
	uint							infantry_movement_fx;
	uint							infantry_shot_fx;
	uint							click_barracks_fx;
	uint							building_fx;
	uint							finished_building_fx;
	uint							finished_recruiting_fx;
	uint							finished_upgrading_fx;
	uint							click_townhall_fx;
	uint							heavy_shot_fx;

	
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
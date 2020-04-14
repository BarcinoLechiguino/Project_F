#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

//#include <algorithm>
#include "Module.h"
#include "Point.h"

#define MAX_ENEMIES 200


struct SDL_Texture;

class Entity;
enum class ENTITY_TYPE;

class Dynamic_Object;
class Static_Object;
class Gatherer;
class Infantry;
class Enemy;
class Rock;
class TownHall;
class Barracks;

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

public:

	Entity* CreateEntity(ENTITY_TYPE type, int x = 0, int y = 0);			//Crates a new entity depending on the ENTITY_TYPE passed as argument. 
	void DestroyEntities();													//Calls the CleanUp() method of each entity and then it clears the entities list.
	
	void LoadEntityTextures();
	void UnLoadEntityTextures();

	SDL_Texture* GetGathererTexture() const;
	SDL_Texture* GetInfantryTexture() const;
	SDL_Texture* GetEnemyTexture() const;
	SDL_Texture* GetTownHallTexture() const;
	SDL_Texture* GetBarracksTexture() const;
	SDL_Texture* GetRockTexture() const;

	bool IsUnit(Entity* entity);														//Method that will return true if the entity passed as argument is a unit.
	bool IsBuilding(Entity* entity);													//Method that will return true if the entity passed as argument is a building.

	void SetEntityMap(int width, int height, Entity* data);								//Method that will allocate the necessary memory for the entity_map.
	void ChangeEntityMap(const iPoint& pos, Entity* entity, bool set_to_null = false);	//Method that will modify the entity map when a unit or building is spawned.

	bool CheckBoundaries(const iPoint& pos) const;										//Method that will check whether or not the position passed as argument is inside the entity_map's bounds.

	Entity* GetEntityAt(const iPoint& pos) const;										//Method that will return whichever entity is at the given position.

	bool CheckTileAvailability(const iPoint& pos, Entity* entity);						//Method that will return true whenever the space required by an entity to spawn is available.

	void ClearAllEntityVectors();

	void OnCollision(Collider* C1, Collider* C2);

public:
	pugi::xml_document				config_file;
	pugi::xml_node					config;

	SDL_Texture*					gatherer_tex;
	SDL_Texture*					infantry_tex;
	SDL_Texture*					enemy_tex;
	SDL_Texture*					townhall_tex;
	SDL_Texture*					barracks_tex;
	SDL_Texture*					rock_tex;

	std::list<Entity*>				entities;	

	std::vector<Gatherer*>			gatherers;
	std::vector<Infantry*>			infantries;
	std::vector<Enemy*>				enemies;
	std::vector<TownHall*>			town_halls;
	std::vector<Barracks*>			barracks;
	std::vector<Rock*>				rocks;

	std::vector<Dynamic_Object*>	dynamic_objects;
	std::vector<Static_Object*>		static_objects;

	Entity**						entity_map;			//Array that will be used to keep track at which position are all entities at all times.
	int								entity_map_width;	//Width of the entity_map.
	int								entity_map_height;	//Height of the entity_map.

	float							accumulated_time;	//Accumulates dt as time goes on.
	float							cycle_length;		//How much time needs to pass / be accumulated before running a cycle. 
	bool							doLogic;			//Keeps track whether or not the entity needs to do it's logic (pathfinding...)
};
#endif // __ENTITY_MANAGER_H__
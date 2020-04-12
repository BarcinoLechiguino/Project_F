#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

#include <algorithm>
#include "Module.h"
#include "Textures.h"
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
	
	bool IsUnit(Entity* entity);
	bool IsBuilding(Entity* entity);

	void SetEntityMap(int width, int height, Entity* data);
	void ChangeEntityMap(const iPoint& pos, Entity* entity);

	bool CheckBoundaries(const iPoint& pos) const;

	Entity* GetEntityAt(const iPoint& pos) const;

	void OnCollision(Collider* C1, Collider* C2);

public:

	pugi::xml_node					config;
	
	std::list<Entity*>				entities;	

	std::vector<Rock*>				rocks;
	std::vector<Enemy*>				enemies;
	std::vector<Gatherer*>			gatherers;
	std::vector<TownHall*>			town_hall;
	std::vector<Barracks*>			barracks;
	std::vector<Infantry*>			infantries;

	std::vector<Dynamic_Object*>	dynamic_objects;

	Entity**						entity_map;
	int								entity_map_width;
	int								entity_map_height;

	float							accumulated_time;	//Accumulates dt as time goes on.
	float							cycle_length;		//How much time needs to pass / be accumulated before running a cycle. 
	bool							doLogic;			//Keeps track whether or not the entity needs to do it's logic (pathfinding...)
};
#endif // __ENTITY_MANAGER_H__
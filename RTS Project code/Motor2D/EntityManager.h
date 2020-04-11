#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

#include <list>
#include <algorithm>
#include "Module.h"
#include "Textures.h"

#include "Entity.h"		
#include "Dynamic_Object.h"
#include "Static_Object.h"

#include "Rock.h"
#include "Enemy.h"
#include "Gatherer.h"
#include "TownHall.h"
#include "Barracks.h"
#include "Infantry.h"

#define MAX_ENEMIES 200


struct SDL_Texture;



struct EnemyData
{
	ENTITY_TYPE		type;
	iPoint			position;
};

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
	
	void OnCollision(Collider* C1, Collider* C2);

public:

	pugi::xml_node					config;
	
	std::list<Entity*>				entities;	
	std::list<EntityData*>			entity_data_list;	//List of the position and ENTITY_TYPE data members of enemy entities.  Change for an array, its faster.

	std::vector<Rock*>				rocks;
	std::vector<Enemy*>				enemies;
	std::vector<Gatherer*>			gatherers;
	std::vector<TownHall*>			town_hall;
	std::vector<Barracks*>			barracks;
	std::vector<Infantry*>			infantries;

	std::vector<Dynamic_Object*>	dynamic_objects;

	char* entity_map;

	float							accumulated_time;	//Accumulates dt as time goes on.
	float							cycle_length;		//How much time needs to pass / be accumulated before running a cycle. 
	bool							doLogic;			//Keeps track whether or not the entity needs to do it's logic (pathfinding...)
};
#endif // __ENTITY_MANAGER_H__
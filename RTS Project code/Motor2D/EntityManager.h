#ifndef __j1ENTITY_MANAGER_H__
#define __j1ENTITY_MANAGER_H__

#include "Module.h"
#include "Entity.h"		
#include <list>
#include <algorithm>
#define MAX_ENEMIES 200

struct SDL_Texture;
class Enemy;


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
	void CreatePlayers();													//Creates P1 and P2. It is called in the j1Scene.cpp.
	//void AddItems(ENTITY_TYPE type, int x, int y);
	void DestroyEntities();													//Calls the CleanUp() method of each entity and then it clears the entities list.
	
	void OnCollision(Collider* C1, Collider* C2);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&);

public:

	pugi::xml_node			config;
	
	std::list<Entity*>	entities;	

	std::list<EntityData*>	entityData_list;	//List of the position and ENTITY_TYPE data members of enemy entities.  Change for an array, its faster.

	float					accumulated_time;	//Accumulates dt as time goes on.
	float					cycle_length;		//How much time needs to pass / be accumulated before running a cycle. 
	bool					doLogic;			//Keeps track whether or not the entity needs to do it's logic (pathfinding...)

	EnemyData				enemies[MAX_ENEMIES];	//Use this if entities need to be processed in an array instead of a list.
};
#endif // __j1ENTITY_MANAGER_H__
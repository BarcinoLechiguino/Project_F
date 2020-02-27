#ifndef __j1ENTITY_MANAGER_H__
#define __j1ENTITY_MANAGER_H__

#include "j1Module.h"
#include "j1Entity.h"		
#include "p2List.h"

#define MAX_ENEMIES 200

struct SDL_Texture;
class j1Entity;
class j1Player;				//Forward declaration. Avoid having to recompile everything when sth is changed in a file / class.
class j1Enemy;
class j1Player1;			//Temporal(?)
class j1Player2;


struct EnemyData
{
	ENTITY_TYPE		type;
	iPoint			position;
};

class j1EntityManager : public j1Module
{
public:
	j1EntityManager();

	~j1EntityManager();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

public:
	j1Entity* CreateEntity(ENTITY_TYPE type, int x = 0, int y = 0);			//Crates a new entity depending on the ENTITY_TYPE passed as argument. 
	void CreatePlayers();													//Creates P1 and P2. It is called in the j1Scene.cpp.
	void AddEnemy(ENTITY_TYPE type, int x, int y);							//Creates a new entityData pointer that will hold the information passed as an argument and will be added to the entityData_list.
	void SpawnEnemies();													//Iterates entityData_list and creates an enemy for each list item.
	//void AddItems(ENTITY_TYPE type, int x, int y);
	void DestroyEntities();													//Calls the CleanUp() method of each entity and then it clears the entities list.
	
	void OnCollision(Collider* C1, Collider* C2);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	pugi::xml_node			config;
	
	p2List<j1Entity*>		entities;			//List of entities. Each created entity will be added to this list and then iterated in the class methods (Update()...)
	j1Player*				player;				//Pointer to the j1Player class. Will represent P1. The pointer will be from the mother class and later it will be setted to player 1 (CreateEntity())
	j1Player*				player2;			//Pointer to the j1Player class. Will represent P2.

	p2List<EntityData*>		entityData_list;	//List of the position and ENTITY_TYPE data members of enemy entities.  Change for an array, its faster.

	float					accumulated_time;	//Accumulates dt as time goes on.
	float					cycle_length;		//How much time needs to pass / be accumulated before running a cycle. 
	bool					doLogic;			//Keeps track whether or not the entity needs to do it's logic (pathfinding...)

	EnemyData				enemies[MAX_ENEMIES];	//Use this if entities need to be processed in an array instead of a list.
};


#endif // __j1ENTITY_MANAGER_H__
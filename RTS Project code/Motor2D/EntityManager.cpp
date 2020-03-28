#include "p2Log.h"
#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Map.h"
#include "Collisions.h"
#include "Brofiler\Brofiler.h"

#include "EntityManager.h"

//#include "mmgr/mmgr.h"

EntityManager::EntityManager()	//Sets the j1Player1* pointers declared in the header to nullptr
{
	name = ("entities");
}

EntityManager::~EntityManager()
{

}

bool EntityManager::Awake(pugi::xml_node& config)
{
	this->config = config;

	cycle_length = config.child("enemies").child("update_cycle_length").attribute("length").as_float(); //Fix pathfinding so it works with doLogic

	//Iterates all entities and calls their Awake() methods.
	for (std::list<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end() ; entity_iterator++)
	{
		//entity_iterator->data->Awake(config.child(entity_iterator->data->name.GetString()));
	}

	return true;
}

bool EntityManager::Start()
{
	//Iterates all entities in the entities list and calls their Start() method.
	for (std::list<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		(*entity_iterator)->Start();
	}

	return true;
}

bool EntityManager::PreUpdate()
{
	for (std::list<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		(*entity_iterator)->PreUpdate();
	}

	return true;
}

bool EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManager Update", Profiler::Color::FireBrick);
	accumulated_time += dt;

	if (accumulated_time >= cycle_length) //Timer that will set doLogic to true 10 times per second (cycle_length = 0.1 sec).
	{
		doLogic = true;
	}

	//Calls the Update method of all entities. Passes dt and doLogic as arguments (mainly for pathfinding enemies).
	for (std::list<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		(*entity_iterator)->Update(dt, doLogic);
	}

	if (doLogic == true)				//Resets the doLogic timer.
	{
		doLogic = false;
		accumulated_time = 0;
	}

	return true;
}

bool EntityManager::PostUpdate()
{
	//Iterates all entities and calls their PostUpdate() methods.
	for (std::list<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		(*entity_iterator)->PostUpdate();
	}

	return true;
}

bool EntityManager::CleanUp()
{
	//Iterates all entities in the entities list and calls their CleanUp() method.
	for (std::list<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		(*entity_iterator)->CleanUp();
		RELEASE((*entity_iterator));
	}

	entities.clear();									//Deletes all items in the entities list and frees all allocated memory.

	return true;
}

void EntityManager::OnCollision(Collider* C1, Collider* C2)		//This OnCollision will manage the collisions of all entities and derive them to their respective OnCollision methods()
{
	for (std::list<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		if (C1 == (*entity_iterator)->collider)					//Will be run if there is a collision and any of the colliders are of the type PLAYER.
		{
			(*entity_iterator)->OnCollision(C1, C2);
			break;
		}
		else if (C2 == (*entity_iterator)->collider)
		{
			(*entity_iterator)->OnCollision(C2, C1);
			break;
		}
	}
}

// -------------------------------------- ENTITY MANAGING METHODS --------------------------------------
Entity* EntityManager::CreateEntity(ENTITY_TYPE type, int x, int y)
{
	//static_assert?

	Entity* ret = nullptr;

	switch (type)
	{

		case ENTITY_TYPE::ROCK:							
		ret = new Rock(x, y, type);				//Allocates memory for an entity from the j1Player module.
	
		break;

		case ENTITY_TYPE::ENEMY:
		ret = new Enemy(x, y, type);

		break;

		case ENTITY_TYPE::GATHERER:
		ret = new Gatherer(x, y, type);

		break;

		case ENTITY_TYPE::TOWNHALL:
		ret = new TownHall(x, y, type);

		break;

		case ENTITY_TYPE::BARRACKS:
		ret = new Barracks(x, y, type);

		break;
	}

	ret->type = type;
	ret->Start();

	if (ret != nullptr)									
	{
		entities.push_back(ret);								//Adds the generated entity to the entities list.
	}

	return ret;
}

void EntityManager::CreatePlayers()
{
	//player = (j1Player1*)CreateEntity(ENTITY_TYPE::PLAYER);
	//player2 = (j1Player2*)CreateEntity(ENTITY_TYPE::PLAYER2);
}

void EntityManager::DestroyEntities()
{
	BROFILER_CATEGORY("EntityManager PostUpdate", Profiler::Color::FireBrick);
	//Iterates all entities in the entities list and searches for the entity passed as argument, if it is inside the list and is found, it is then destroyed.
	LOG("There are %d entities in the entities list.", entities.size());
	
	for (std::list<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		
		(*entity_iterator)->CleanUp();			//Calls the CleanUp() method of the iterated entity (an enemy entity).
		RELEASE((*entity_iterator));			//Deletes the data buffer
		entities.erase(entity_iterator);		//Deletes the entity being iterated from the list.
		
		//break;
	}
}

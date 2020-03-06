#include "j1EntityManager.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1App.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1Window.h"
#include "Brofiler\Brofiler.h"
#include "j1Rock.h"



//#include "mmgr/mmgr.h"

j1EntityManager::j1EntityManager()	//Sets the j1Player1* pointers declared in the header to nullptr
{
	name = ("entities");
}

j1EntityManager::~j1EntityManager()
{

}

bool j1EntityManager::Awake(pugi::xml_node& config)
{
	this->config = config;

	cycle_length = config.child("enemies").child("update_cycle_length").attribute("length").as_float(); //Fix pathfinding so it works with doLogic

	//Iterates all entities and calls their Awake() methods.
	for (std::list<j1Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end() ; entity_iterator++)
	{
		//entity_iterator->data->Awake(config.child(entity_iterator->data->name.GetString()));
	}

	return true;
}

bool j1EntityManager::Start()
{
	//Iterates all entities in the entities list and calls their Start() method.
	for (std::list<j1Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		(*entity_iterator)->Start();
	}

	return true;
}

bool j1EntityManager::PreUpdate()
{
	for (std::list<j1Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		(*entity_iterator)->PreUpdate();
	}

	return true;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManager Update", Profiler::Color::FireBrick);
	accumulated_time += dt;

	if (accumulated_time >= cycle_length) //Timer that will set doLogic to true 10 times per second (cycle_length = 0.1 sec).
	{
		doLogic = true;
	}

	//Calls the Update method of all entities. Passes dt and doLogic as arguments (mainly for pathfinding enemies).
	for (std::list<j1Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
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

bool j1EntityManager::PostUpdate()
{
	//Iterates all entities and calls their PostUpdate() methods.
	for (std::list<j1Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		(*entity_iterator)->PostUpdate();
	}

	return true;
}

bool j1EntityManager::CleanUp()
{
	//Iterates all entities in the entities list and calls their CleanUp() method.
	for (std::list<j1Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		(*entity_iterator)->CleanUp();
		RELEASE((*entity_iterator));
	}

	entities.clear();									//Deletes all items in the entities list and frees all allocated memory.

	return true;
}

void j1EntityManager::OnCollision(Collider* C1, Collider* C2)		//This OnCollision will manage the collisions of all entities and derive them to their respective OnCollision methods()
{
	for (std::list<j1Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
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

bool j1EntityManager::Save(pugi::xml_node& data)
{
	

	/*pugi::xml_node mecha = data.append_child("mecha");
	pugi::xml_node alien = data.append_child("alien");

	for (std::list<j1Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		if ((*entity_iterator)->type == ENTITY_TYPE::MECHA)
			(*entity_iterator)->Save(mecha);
		if ((*entity_iterator)->type == ENTITY_TYPE::ALIEN)
			(*entity_iterator)->Save(alien);
	}

	for (int i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i].type == ENTITY_TYPE::MECHA) {
			pugi::xml_node position = mecha.append_child("position");
			position.append_attribute("x") = enemies[i].position.x;
			position.append_attribute("y") = enemies[i].position.y;
		}
		if (enemies[i].type == ENTITY_TYPE::ALIEN) {
			pugi::xml_node position = alien.append_child("position");
			position.append_attribute("x") = enemies[i].position.x;
			position.append_attribute("y") = enemies[i].position.y;
		}	
	}*/

	return true;
}

bool j1EntityManager::Load(pugi::xml_node& data)
{

	return true;
}


// -------------------------------------- ENTITY MANAGING METHODS --------------------------------------
j1Entity* j1EntityManager::CreateEntity(ENTITY_TYPE type, int x, int y)
{
	//static_assert?

	j1Entity* ret = nullptr;

	switch (type)
	{

		case ENTITY_TYPE::ROCK:							
		ret = new j1Rock(x, y, type);				//Allocates memory for an entity from the j1Player module.
	
		break;

	}

	ret->type = type;

	if (ret != nullptr)									
	{
		entities.push_back(ret);								//Adds the generated entity to the entities list.
	}

	return ret;
}

void j1EntityManager::CreatePlayers()
{
	//player = (j1Player1*)CreateEntity(ENTITY_TYPE::PLAYER);
	//player2 = (j1Player2*)CreateEntity(ENTITY_TYPE::PLAYER2);
}

void j1EntityManager::DestroyEntities()
{
	BROFILER_CATEGORY("EntityManager PostUpdate", Profiler::Color::FireBrick);
	//Iterates all entities in the entities list and searches for the entity passed as argument, if it is inside the list and is found, it is then destroyed.
	LOG("There are %d entities in the entities list.", entities.size());
	
	for (std::list<j1Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		
		(*entity_iterator)->CleanUp();			//Calls the CleanUp() method of the iterated entity (an enemy entity).
		RELEASE((*entity_iterator));			//Deletes the data buffer
		entities.erase(entity_iterator);		//Deletes the entity being iterated from the list.
			//break;
		
	}
}

#include "j1EntityManager.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1App.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1Player.h"
#include "j1Player1.h"
#include "j1Player2.h"
#include "j1Mecha.h"
#include "j1Alien.h"
#include "j1Coin.h"
#include "j1Window.h"
#include "Brofiler\Brofiler.h"

//#include "mmgr/mmgr.h"

j1EntityManager::j1EntityManager() : player(nullptr), player2(nullptr)	//Sets the j1Player1* pointers declared in the header to nullptr
{
	name.create("entities");
}

j1EntityManager::~j1EntityManager()
{
	//As the pointers have been set in the constructor, they must be destroyed / deleted in the destructor.
	delete player;
	delete player2;
}

bool j1EntityManager::Awake(pugi::xml_node& config)
{
	this->config = config;

	cycle_length = config.child("enemies").child("update_cycle_length").attribute("length").as_float(); //Fix pathfinding so it works with doLogic

	//Iterates all entities and calls their Awake() methods.
	for (p2List_item<j1Entity*>* entity_iterator = entities.start; entity_iterator; entity_iterator = entity_iterator->next)
	{
		//entity_iterator->data->Awake(config.child(entity_iterator->data->name.GetString()));
	}

	return true;
}

bool j1EntityManager::Start()
{
	//Iterates all entities in the entities list and calls their Start() method.
	for (p2List_item<j1Entity*>* entity_iterator = entities.start; entity_iterator != NULL; entity_iterator = entity_iterator->next)
	{
		entity_iterator->data->Start();
	}

	return true;
}

bool j1EntityManager::PreUpdate()
{
	SpawnEnemies();   					//Should this be here?

	for (p2List_item<j1Entity*>* entity_iterator = entities.start; entity_iterator != NULL; entity_iterator = entity_iterator->next)
	{
		entity_iterator->data->PreUpdate();
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
	for (p2List_item<j1Entity*>* entity_iterator = entities.start; entity_iterator != NULL; entity_iterator = entity_iterator->next)
	{
		entity_iterator->data->Update(dt, doLogic);
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
	for (p2List_item<j1Entity*>* entity_iterator = entities.start; entity_iterator != NULL; entity_iterator = entity_iterator->next)
	{
		entity_iterator->data->PostUpdate();
	}

	return true;
}

bool j1EntityManager::CleanUp()
{
	//Iterates all entities in the entities list and calls their CleanUp() method.
	for (p2List_item<j1Entity*>* entity_iterator = entities.start; entity_iterator != NULL; entity_iterator = entity_iterator->next)
	{
		entity_iterator->data->CleanUp();
		RELEASE(entity_iterator->data);
	}

	entities.clear();									//Deletes all items in the entities list and frees all allocated memory.

	player = NULL;										//Sets the j1Player* player pointer to NULL.
	player2 = NULL;

	return true;
}

void j1EntityManager::OnCollision(Collider* C1, Collider* C2)		//This OnCollision will manage the collisions of all entities and derive them to their respective OnCollision methods()
{
	for (p2List_item<j1Entity*>* entity_iterator = entities.start; entity_iterator != NULL; entity_iterator = entity_iterator->next)
	{
		if (C1 == entity_iterator->data->collider)					//Will be run if there is a collision and any of the colliders are of the type PLAYER.
		{
			entity_iterator->data->OnCollision(C1, C2);
			break;
		}
		else if (C2 == entity_iterator->data->collider)
		{
			entity_iterator->data->OnCollision(C2, C1);
			break;
		}
	}
}

bool j1EntityManager::Save(pugi::xml_node& data) const
{
	player->Save(data.append_child("player"));
	player2->Save(data.append_child("player2"));

	pugi::xml_node mecha = data.append_child("mecha");
	pugi::xml_node alien = data.append_child("alien");

	for (p2List_item<j1Entity*>* iterator = entities.start; iterator; iterator = iterator->next)
	{
		if (iterator->data->type == ENTITY_TYPE::MECHA)
			iterator->data->Save(mecha);
		if (iterator->data->type == ENTITY_TYPE::ALIEN)
			iterator->data->Save(alien);
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
	}

	return true;
}

bool j1EntityManager::Load(pugi::xml_node& data)
{
	if (player != nullptr)
	{
		player->Load(data);
	}

	if (player2 != nullptr)
	{
		player2->Load(data);
	}

	for (pugi::xml_node mecha = data.child("mecha").child("position"); mecha; mecha = mecha.next_sibling()) {
		iPoint mechapos = { mecha.attribute("x").as_int(), mecha.attribute("y").as_int() };
		AddEnemy(ENTITY_TYPE::MECHA, mechapos.x, mechapos.y );
	}

	for (pugi::xml_node alien = data.child("alien").child("position"); alien; alien = alien.next_sibling()) {
		iPoint alienpos = { alien.attribute("x").as_int(), alien.attribute("y").as_int() };
		AddEnemy(ENTITY_TYPE::ALIEN, alienpos.x, alienpos.y);
	}

	return true;
}


// -------------------------------------- ENTITY MANAGING METHODS --------------------------------------
j1Entity* j1EntityManager::CreateEntity(ENTITY_TYPE type, int x, int y)
{
	//static_assert?

	j1Entity* ret = nullptr;

	switch (type)
	{
	case ENTITY_TYPE::PLAYER:							//If the ENTITT_TYPE passed as argument is PLAYER.
		//ret = new j1Player(x, y, type);				//Allocates memory for an entity from the j1Player module.
		ret = new j1Player1(x, y, type);
		break;

	case ENTITY_TYPE::PLAYER2:
		ret = new j1Player2(x, y, type);				//REVISE THIS HERE. Check if we can pass only j1Player and thats it or if both can be ENTITY_TYPE player
		break;

	case ENTITY_TYPE::MECHA:							//If the ENTITT_TYPE passed as argument is a MECHA.
		ret = new j1Mecha(x, y, type);
		break;

	case ENTITY_TYPE::ALIEN:							//If the ENTITT_TYPE passed as argument is an ALIEN.
		ret = new j1Alien(x, y, type);
		break;

	case ENTITY_TYPE::COIN:								//If the ENTITY_TYPE passed as argument is a COIN.
		ret = new j1Coin(x, y, type);
		break;
	}
	//ret->type = type;

	if (ret != nullptr)									//If the j1Entity* pointer is not NULL.
	{
		entities.add(ret);								//Adds the generated entity to the entities list.
	}

	return ret;
}

void j1EntityManager::CreatePlayers()
{
	player = (j1Player1*)CreateEntity(ENTITY_TYPE::PLAYER);
	player2 = (j1Player2*)CreateEntity(ENTITY_TYPE::PLAYER2);
}

void j1EntityManager::AddEnemy(ENTITY_TYPE type, int x, int y)
{
	int i = 0;
	LOG("Adding enemy %d", i);
	i++;
	
	EntityData* data = new EntityData();

	data->position.x = x;
	data->position.y = y;
	data->type = type;

	entityData_list.add(data);
}

void j1EntityManager::SpawnEnemies()
{
	
	p2List_item<EntityData*>* enemy_iterator = entityData_list.start;

	int i = 0;
	for (enemy_iterator; enemy_iterator != NULL; enemy_iterator = enemy_iterator->next)												//Iterates the entityData_list.
	{
		j1Entity * enemy = nullptr;																									//Pointer that will be assigned to each enemy entity.

		switch (enemy_iterator->data->type)			//REVISE TYPE, maybe it will not work.
		{
		case ENTITY_TYPE::MECHA:
			enemy = new j1Mecha(enemy_iterator->data->position.x, enemy_iterator->data->position.y, enemy_iterator->data->type);	//Spawns a MECHA type enemy.
			break;

		case ENTITY_TYPE::ALIEN:
			enemy = new j1Alien(enemy_iterator->data->position.x, enemy_iterator->data->position.y, enemy_iterator->data->type);	//Spawns an ALIEN type enemy.
			break;

		case ENTITY_TYPE::COIN:
			enemy = new j1Coin(enemy_iterator->data->position.x, enemy_iterator->data->position.y, enemy_iterator->data->type);
			
			break;
		}

		if (enemy != NULL)							//Uncomment when entities can be spawned.
		{
			entities.add(enemy);																									//The entity is added to the entities list
			enemy->Start();																											//The entity's start method is called.
			//break;								//Unless this method is used to spawn a single entity at a time, this needs to be kept commented. 
		}

		RELEASE(enemy_iterator->data);				//Frees all memory allocated for the entity. AddEnemy() --> EntityData* data = new EntityData();
	}
 
	entityData_list.clear();						//Once all enemies have been spawned, the list is cleared.
}

//void j1EntityManager::AddItems(ENTITY_TYPE type, int x, int y)
//{
//	(j1Coin*)CreateEntity(type, x, y);
//	LOG("There are %d entities in the entities list at Coin creation.", entities.count());
//
//	/*int i = 0;
//	for (p2List_item<j1Entity*>* entity_iterator = entities.start; entity_iterator != NULL; entity_iterator = entity_iterator->next)
//	{
//		if (entity_iterator->data->type == ENTITY_TYPE::COIN)
//		{
//			LOG("Coins in the entities list at coin Addition n %d", i);
//			i++;
//		}
//	}*/
//}

void j1EntityManager::DestroyEntities()
{
	BROFILER_CATEGORY("EntityManager PostUpdate", Profiler::Color::FireBrick);
	//Iterates all entities in the entities list and searches for the entity passed as argument, if it is inside the list and is found, it is then destroyed.
	LOG("There are %d entities in the entities list.", entities.count());
	
	for (p2List_item<j1Entity*>* entity_iterator = entities.start; entity_iterator != NULL; entity_iterator = entity_iterator->next)
	{ 	
		if (entity_iterator->data->type != ENTITY_TYPE::PLAYER && entity_iterator->data->type != ENTITY_TYPE::PLAYER2)
		{
			entity_iterator->data->CleanUp();			//Calls the CleanUp() method of the iterated entity (an enemy entity).
			RELEASE(entity_iterator->data);				//Deletes the data buffer
			entities.del(entity_iterator);				//Deletes the entity being iterated from the list.
			
			//break;
		}
	}
}

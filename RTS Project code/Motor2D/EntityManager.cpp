#include "Brofiler\Brofiler.h"

#include "p2Log.h"
#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Collisions.h"
#include "Map.h"

#include "EntityManager.h"
#include "Entity.h"
#include "Dynamic_Object.h"
#include "Static_Object.h"
#include "Gatherer.h"
#include "Infantry.h"
#include "Enemy.h"
#include "TownHall.h"
#include "EnemyTownHall.h"
#include "Barracks.h"
#include "EnemyBarracks.h"
#include "Rock.h"

//#include "mmgr/mmgr.h"


EntityManager::EntityManager()	//Sets the j1Player1* pointers declared in the header to nullptr
{
	name = ("entities");
}

EntityManager::~EntityManager()
{
	RELEASE_ARRAY(entity_map);
}

bool EntityManager::Awake(pugi::xml_node& config)
{
	this->config = config;

	cycle_length = config.child("enemies").child("update_cycle_length").attribute("length").as_float(); //Fix pathfinding so it works with doLogic


	for (std::vector<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++) //Iterates all entities and calls their Awake() methods.
	{
		//entity_iterator->data->Awake(config.child(entity_iterator->data->name.GetString()));
	}

	return true;
}

bool EntityManager::Start()
{
	LoadEntityTextures();

	//Iterates all entities in the entities list and calls their Start() method.
	for (std::vector<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		(*entity_iterator)->Start();
	}

	return true;
}

bool EntityManager::PreUpdate()
{
	for (std::vector<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
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
	for (std::vector<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
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
	for (std::vector<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		(*entity_iterator)->PostUpdate();
	}

	return true;
}

bool EntityManager::CleanUp()
{
	//Iterates all entities in the entities list and calls their CleanUp() method.
	for (std::vector<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
	{
		(*entity_iterator)->CleanUp();
		RELEASE((*entity_iterator));
	}

	entities.clear();

	UnLoadEntityTextures();

	return true;
}

void EntityManager::OnCollision(Collider* C1, Collider* C2)		//This OnCollision will manage the collisions of all entities and derive them to their respective OnCollision methods()
{
	for (std::vector<Entity*>::iterator entity_iterator = entities.begin(); entity_iterator != entities.end(); entity_iterator++)
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

Entity* EntityManager::CreateEntity(ENTITY_TYPE type, int x, int y, int level)
{
	//static_assert?

	Entity* entity = nullptr;

	switch (type)
	{
	case ENTITY_TYPE::GATHERER:
		entity = new Gatherer(x, y, type, level);
		break;

	case ENTITY_TYPE::INFANTRY:
		entity = new Infantry(x, y, type, level);
		break;

	case ENTITY_TYPE::ENEMY:
		entity = new Enemy(x, y, type, level);
		break;

	case ENTITY_TYPE::TOWNHALL:
		entity = new TownHall(x, y, type, level);
		break;

	case ENTITY_TYPE::BARRACKS:
		entity = new Barracks(x, y, type, level);

	case ENTITY_TYPE::ENEMY_TOWNHALL:
		entity = new EnemyTownHall(x, y, type);
		break;

	case ENTITY_TYPE::ENEMY_BARRACKS:
		entity = new EnemyBarracks(x, y, type);
		break;

	case ENTITY_TYPE::ROCK:
		entity = new Rock(x, y, type, level);
		break;
	}

	if (entity != nullptr)
	{
		if (CheckTileAvailability(iPoint(x, y), entity))
		{
			entities.push_back(entity);								//Adds the generated entity to the entities list.
			ChangeEntityMap(iPoint(x, y), entity);					//Adds the generated entity to entity_map.

			entity->Start();
		}
		else
		{
			delete entity;
			entity = nullptr;
		}
	}

	return entity;
}

void EntityManager::DestroyEntities()
{
	BROFILER_CATEGORY("EntityManager PostUpdate", Profiler::Color::FireBrick);

	std::vector<Entity*> tmp;

	std::vector<Entity*>::iterator entity_iterator = entities.begin();

	for (; entity_iterator != entities.end(); ++entity_iterator)
	{
		tmp.push_back((*entity_iterator));
	}

	std::vector<Entity*>::iterator item = tmp.begin();

	for (; item != tmp.end(); ++item)
	{
		(*item)->CleanUp();
		RELEASE((*item));
	}

	//std::vector<Entity*>::iterator entity_iterator = entities.begin();

	//for (; entity_iterator != entities.end(); ++entity_iterator)
	//{
	//	(*entity_iterator)->CleanUp();			//Calls the CleanUp() method of the iterated entity (an enemy entity).
	//	RELEASE((*entity_iterator));			//Deletes the data buffer

	//	//break;
	//}

	tmp.clear();

	entities.clear();
}

void EntityManager::DeleteEntity(Entity* entity)
{
	std::vector<Entity*>::iterator item = entities.begin();

	for (; item != entities.end(); ++item)
	{
		if ((*item) == entity)
		{
			(*item)->CleanUp();
			RELEASE((*item));

			entities.erase(item);

			break;
		}
	}
}

void EntityManager::LoadEntityTextures()
{
	config_file.load_file("config.xml");

	pugi::xml_node entity_textures = config_file.child("config").child("entities").child("textures");

	gatherer_tex = App->tex->Load(entity_textures.child("gatherer_texture").attribute("path").as_string());
	infantry_tex = App->tex->Load(entity_textures.child("infantry_texture").attribute("path").as_string());
	enemy_tex = App->tex->Load(entity_textures.child("enemy_texture").attribute("path").as_string());
	townhall_tex = App->tex->Load(entity_textures.child("townhall_texture").attribute("path").as_string());
	enemy_townhall_tex = App->tex->Load(entity_textures.child("enemy_townhall_texture").attribute("path").as_string());
	barracks_tex = App->tex->Load(entity_textures.child("barracks_texture").attribute("path").as_string());
	enemy_barracks_tex = App->tex->Load(entity_textures.child("enemy_barracks_texture").attribute("path").as_string());
	rock_tex = App->tex->Load(entity_textures.child("rock_texture").attribute("path").as_string());
}

void EntityManager::UnLoadEntityTextures()
{
	App->tex->UnLoad(gatherer_tex);
	App->tex->UnLoad(infantry_tex);
	App->tex->UnLoad(enemy_tex);
	App->tex->UnLoad(townhall_tex);
	App->tex->UnLoad(enemy_townhall_tex);
	App->tex->UnLoad(barracks_tex);
	App->tex->UnLoad(enemy_barracks_tex);
	App->tex->UnLoad(rock_tex);

	gatherer_tex = nullptr;
	infantry_tex = nullptr;
	enemy_tex = nullptr;
	townhall_tex = nullptr;
	enemy_townhall_tex = nullptr;
	barracks_tex = nullptr;
	enemy_barracks_tex = nullptr;
	rock_tex = nullptr;
}

SDL_Texture* EntityManager::GetGathererTexture() const
{
	return gatherer_tex;
}

SDL_Texture* EntityManager::GetInfantryTexture() const
{
	return infantry_tex;
}

SDL_Texture* EntityManager::GetEnemyTexture() const
{
	return enemy_tex;
}

SDL_Texture* EntityManager::GetTownHallTexture() const
{
	return townhall_tex;
}

SDL_Texture* EntityManager::GetEnemyTownHallTexture() const
{
	return enemy_townhall_tex;
}

SDL_Texture* EntityManager::GetBarracksTexture() const
{
	return barracks_tex;
}

SDL_Texture* EntityManager::GetEnemyBarracksTexture() const
{
	return enemy_barracks_tex;
}

SDL_Texture* EntityManager::GetRockTexture() const
{
	return rock_tex;
}

bool EntityManager::IsUnit(Entity* entity)
{
	if (entity->type == ENTITY_TYPE::GATHERER || entity->type == ENTITY_TYPE::INFANTRY || entity->type == ENTITY_TYPE::ENEMY)
	{
		return true;
	}

	return false;
}

bool EntityManager::IsBuilding(Entity* entity)
{
	if (entity->type == ENTITY_TYPE::TOWNHALL || entity->type == ENTITY_TYPE::ENEMY_TOWNHALL
		|| entity->type == ENTITY_TYPE::BARRACKS || entity->type == ENTITY_TYPE::ENEMY_BARRACKS)
	{
		return true;
	}

	return false;
}

bool EntityManager::IsResource(Entity* entity)
{
	if (entity->type == ENTITY_TYPE::ROCK)
	{
		return true;
	}

	return false;
}

void EntityManager::SetEntityMap(int width, int height)
{
	entity_map_width = width;
	entity_map_height = height;

	RELEASE_ARRAY(entity_map);

	entity_map = new Entity * [width * height];

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int index = (y * width) + x;

			entity_map[index] = nullptr;
		}
	}

	//memcpy(entity_map, data, width * height);							// THIS HERE
}

void EntityManager::ChangeEntityMap(const iPoint& pos, Entity* entity, bool set_to_null)
{

	if (entity_map != nullptr)
	{
		if (!set_to_null)
		{
			if (IsUnit(entity))
			{
				entity_map[(pos.y * entity_map_width) + pos.x] = entity;
				return;
			}

			if (IsBuilding(entity) || IsResource(entity))
			{
				Static_Object* item = (Static_Object*)entity;

				for (int y = 0; y != item->tiles_occupied_y; ++y)
				{
					for (int x = 0; x != item->tiles_occupied_x; ++x)
					{
						int pos_y = pos.y + y;
						int pos_x = pos.x + x;

						entity_map[(pos_y * entity_map_width) + pos_x] = entity;
					}
				}
			}
		}
		else
		{
			if (IsUnit(entity))
			{
				entity_map[(pos.y * entity_map_width) + pos.x] = nullptr;
				return;
			}

			if (IsBuilding(entity) || IsResource(entity))
			{
				Static_Object* item = (Static_Object*)entity;

				for (int y = 0; y != item->tiles_occupied_y; ++y)
				{
					for (int x = 0; x != item->tiles_occupied_x; ++x)
					{
						int pos_y = pos.y + y;
						int pos_x = pos.x + x;

						entity_map[(pos_y * entity_map_width) + pos_x] = nullptr;
					}
				}
			}
		}
	}
}

bool EntityManager::CheckEntityMapBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= entity_map_width &&
		pos.y >= 0 && pos.y <= entity_map_height);
}

Entity* EntityManager::GetEntityAt(const iPoint& pos) const
{
	if (CheckEntityMapBoundaries(pos))
	{
		return entity_map[(pos.y * entity_map_width) + pos.x];
	}
}

bool EntityManager::CheckTileAvailability(const iPoint& pos, Entity* entity)
{
	if (entity_map != nullptr)
	{
		if (IsUnit(entity))
		{
			if (entity_map[(pos.y * entity_map_width) + pos.x] != nullptr)
			{
				return false;
			}
		}

		if (IsBuilding(entity) || IsResource(entity))
		{
			Static_Object* building = (Static_Object*)entity;

			for (int y = 0; y != building->tiles_occupied_y; ++y)
			{
				for (int x = 0; x != building->tiles_occupied_x; ++x)
				{
					int pos_y = pos.y + y;
					int pos_x = pos.x + x;

					if (entity_map[(pos_y * entity_map_width) + pos_x] != nullptr)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}
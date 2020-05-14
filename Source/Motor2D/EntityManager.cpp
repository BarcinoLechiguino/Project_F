#include "Brofiler\Brofiler.h"
//#include "mmgr/mmgr.h"

#include <algorithm>

#include "EntityQuadTree.h"

#include "Log.h"
#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Collisions.h"
#include "Map.h"
#include "Audio.h"

#include "EntityManager.h"
#include "Entity.h"

#include "DynamicObject.h"
#include "Gatherer.h"
#include "Scout.h"
#include "Infantry.h"
#include "Heavy.h"
#include "EnemyGatherer.h"
#include "EnemyScout.h"
#include "EnemyInfantry.h"
#include "EnemyHeavy.h"

#include "StaticObject.h"
#include "TownHall.h"
#include "Barracks.h"
#include "Wall.h"
#include "EnemyTownHall.h"
#include "EnemyBarracks.h"
#include "EnemyWall.h"

#include "Rock.h"
#include "Tree.h"
#include "Bits.h"

struct {
	bool operator()(Entity* a, Entity* b) const
	{
		return a->center_point.y < b->center_point.y;
	}
} customLess;

EntityManager::EntityManager() : entity_map(nullptr)
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

	cycle_length = config.child("units").child("update_cycle_length").attribute("length").as_float();		//Fix pathfinding so it works with do_logic

	for (int i = 0; i < (int)entities.size(); ++i)
	{
		//entities[i]->Awake(config.child(entities[i]->name_tag.c_str()));									//name_tag is currently not being used.
	}

	return true;
}

bool EntityManager::Start()
{
	debug_center_point = false;

	LoadEntityTextures();
	LoadEntityAudios();

	//Iterates all entities in the entities list and calls their Start() method.
	for (int i = 0; i < (int)entities.size(); ++i)
	{
		entities[i]->Start();
	}
	return true;
}

bool EntityManager::PreUpdate()
{

	for (int i = 0; i < (int)entities.size(); ++i)
	{
		entities[i]->PreUpdate();
	}
	return true;
}

bool EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManager Update", Profiler::Color::FireBrick);
	accumulated_time += dt;

	entities_in_screen.clear();

	if (accumulated_time >= cycle_length)											//Timer that will set do_logic to true 10 times per second (cycle_length == 0.1 sec).
	{
		do_logic = true;
	}

	int j = 0;

	for (int i = 0; i < (int)entities.size(); ++i)
	{
		entities[i]->Update(dt, do_logic);

		if(InViewport(entities[i]) && entities[i]->is_visible) //Entities to be drawn
		{
			entities_in_screen.push_back(entities[i]);
			j++;
			//LOG("trues %d", j);
		}
	}

	if (do_logic == true)				//Resets the do_logic timer.
	{
		do_logic = false;
		accumulated_time = 0.0f;
	}

	OrderEntities();

	return true;
}

bool EntityManager::PostUpdate()
{
	//Iterates all entities and calls their PostUpdate() methods.
	for (int i = 0; i < (int)entities.size(); ++i)
	{
		entities[i]->PostUpdate();
	}

	return true;
}

bool EntityManager::CleanUp()
{
	//Iterates all entities in the entities list and calls their CleanUp() method.
	for (int i = 0; i < (int)entities.size(); ++i)
	{
		entities[i]->CleanUp();
		RELEASE(entities[i]);
	}

	LOG("RELEASING ENTITY MAP");
	RELEASE_ARRAY(entity_map);
	LOG("ENTITY MAP RELEASED");

	entities.clear();

	UnLoadEntityTextures();
	UnLoadEntityAudios();

	return true;
}

void EntityManager::OrderEntities()
{
	BROFILER_CATEGORY("OrderEntities", Profiler::Color::FireBrick);

	std::sort(entities_in_screen.begin(), entities_in_screen.end(), customLess);

	DrawEntities();
}

void EntityManager::DrawEntities()
{
	//LOG("Entities in screen %d", entities_in_screen.size());

	for (int i = 0; i < (int)entities_in_screen.size(); ++i)
	{
		entities_in_screen[i]->Draw();

		if (debug_center_point)
		{
			App->render->Blit(center_point_debug, (int)entities_in_screen[i]->center_point.x + App->map->data.tile_width/2 - 6, (int)entities_in_screen[i]->center_point.y - 5, nullptr);
		}
	}
}

void EntityManager::OnCollision(Collider* C1, Collider* C2)		//This OnCollision will manage the collisions of all entities and derive them to their respective OnCollision methods()
{
	std::vector<Entity*>::iterator item = entities.begin();

	for (; item != entities.end(); ++item)
	{
		if (C1 == (*item)->collider)							//Will be run if there is a collision and any of the colliders are of the type PLAYER.
		{
			(*item)->OnCollision(C1, C2);
			break;
		}
		else if (C2 == (*item)->collider)
		{
			(*item)->OnCollision(C2, C1);
			break;
		}
	}
}

// -------------------------------------- ENTITY MANAGING METHODS --------------------------------------

Entity* EntityManager::CreateEntity(ENTITY_TYPE type, int x, int y, int level)
{
	//static_assert?
	if (abs(x) > App->map->data.width || abs(y) > App->map->data.height)
	{
		LOG("but why doe :(");
	}

	Entity* entity = nullptr;

	switch (type)
	{
	case ENTITY_TYPE::GATHERER:
		entity = new Gatherer(x, y, type, level);
		break;

	case ENTITY_TYPE::SCOUT:
		entity = new Scout(x, y, type, level);
		break;

	case ENTITY_TYPE::INFANTRY:
		entity = new Infantry(x, y, type, level);
		break;

	case ENTITY_TYPE::HEAVY:
		entity = new Heavy(x, y, type, level);
		break;

	case ENTITY_TYPE::ENEMY_GATHERER:
		entity = new EnemyGatherer(x, y, type, level);
		break;

	case ENTITY_TYPE::ENEMY_SCOUT:
		entity = new EnemyScout(x, y, type, level);
		break;

	case ENTITY_TYPE::ENEMY_INFANTRY:
		entity = new EnemyInfantry(x, y, type, level);
		break;

	case ENTITY_TYPE::ENEMY_HEAVY:
		entity = new EnemyHeavy(x, y, type, level);
		break;

	case ENTITY_TYPE::TOWNHALL:
		entity = new TownHall(x, y, type, level);
		break;

	case ENTITY_TYPE::BARRACKS:
		entity = new Barracks(x, y, type, level);
		break;

	case ENTITY_TYPE::WALL:
		entity = new Wall(x, y, type, level);
		break;

	case ENTITY_TYPE::ENEMY_TOWNHALL:
		entity = new EnemyTownHall(x, y, type, level);
		break;

	case ENTITY_TYPE::ENEMY_BARRACKS:
		entity = new EnemyBarracks(x, y, type, level);
		break;

	case ENTITY_TYPE::ENEMY_WALL:
		entity = new EnemyWall(x, y, type, level);
		break;

	case ENTITY_TYPE::ROCK:
		entity = new Rock(x, y, type, level);
		break;

	case ENTITY_TYPE::TREE:
		entity = new Tree(x, y, type, level);
		break;

	case ENTITY_TYPE::BITS:
		entity = new Bits(x, y, type, level);
		break;
	}

	if (entity != nullptr)
	{
		if (CheckTileAvailability(iPoint(x, y), entity))
		{
			entities.push_back(entity);								// Adds the generated entity to the entities list.
			ChangeEntityMap(iPoint(x, y), entity);					// Adds the generated entity to entity_map.

			entity->Start();										// Required. entity_manager only iterates the entities' Start() at EntityManager::Start() (Once before first app frame).
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

	std::vector<Entity*>::iterator item = entities.begin();

	for (; item != entities.end(); ++item)
	{
		(*item)->CleanUp();						//Calls the CleanUp() method of the iterated entity (an enemy entity).
		RELEASE((*item));						//Deletes the data buffer
	}

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

			entities.resize(entities.size());		// THIS?

			break;
		}
	}
}

// --- ENTITY TEXTURE LOAD & UNLOAD ---
void EntityManager::LoadEntityTextures()
{
	config_file.load_file("config.xml");

	pugi::xml_node entity_textures = config_file.child("config").child("entities").child("textures");

	// UNITS
	gatherer_tex		= App->tex->Load(entity_textures.child("gatherer_texture").attribute("path").as_string());
	scout_tex			= App->tex->Load(entity_textures.child("scout_texture").attribute("path").as_string());
	infantry_tex		= App->tex->Load(entity_textures.child("infantry_texture").attribute("path").as_string());
	heavy_tex			= App->tex->Load(entity_textures.child("heavy_texture").attribute("path").as_string());
	enemy_gatherer_tex	= App->tex->Load(entity_textures.child("enemy_gatherer_texture").attribute("path").as_string());
	enemy_scout_tex		= App->tex->Load(entity_textures.child("enemy_scout_texture").attribute("path").as_string());
	enemy_infantry_tex	= App->tex->Load(entity_textures.child("enemy_infantry_texture").attribute("path").as_string());
	enemy_heavy_tex		= App->tex->Load(entity_textures.child("enemy_heavy_texture").attribute("path").as_string());

	// BUILDINGS
	townhall_tex		= App->tex->Load(entity_textures.child("townhall_texture").attribute("path").as_string());
	barracks_tex		= App->tex->Load(entity_textures.child("barracks_texture").attribute("path").as_string());
	wall_tex			= App->tex->Load(entity_textures.child("wall_texture").attribute("path").as_string());
	enemy_townhall_tex	= App->tex->Load(entity_textures.child("enemy_townhall_texture").attribute("path").as_string());
	enemy_barracks_tex	= App->tex->Load(entity_textures.child("enemy_barracks_texture").attribute("path").as_string());
	enemy_wall_tex		= App->tex->Load(entity_textures.child("enemy_wall_texture").attribute("path").as_string());

	// RESOURCES
	rock_tex			= App->tex->Load(entity_textures.child("rock_texture").attribute("path").as_string());
	tree_tex			= App->tex->Load(entity_textures.child("tree_texture").attribute("path").as_string());
	bits_tex			= App->tex->Load(entity_textures.child("bits_texture").attribute("path").as_string());

	// SPRITE RENDERING ORDER DEBUG TEXTURE
	center_point_debug	= App->tex->Load("maps/center_position_debug.png");
}

void EntityManager::UnLoadEntityTextures()
{
	App->tex->UnLoad(gatherer_tex);
	App->tex->UnLoad(scout_tex);
	App->tex->UnLoad(infantry_tex);
	App->tex->UnLoad(heavy_tex);
	App->tex->UnLoad(enemy_gatherer_tex);
	App->tex->UnLoad(enemy_scout_tex);
	App->tex->UnLoad(enemy_infantry_tex);
	App->tex->UnLoad(enemy_heavy_tex);

	App->tex->UnLoad(townhall_tex);
	App->tex->UnLoad(barracks_tex);
	App->tex->UnLoad(wall_tex);
	App->tex->UnLoad(enemy_townhall_tex);
	App->tex->UnLoad(enemy_barracks_tex);
	App->tex->UnLoad(enemy_wall_tex);

	App->tex->UnLoad(rock_tex);
	App->tex->UnLoad(tree_tex);
	App->tex->UnLoad(bits_tex);

	gatherer_tex		= nullptr;
	scout_tex			= nullptr;
	infantry_tex		= nullptr;
	heavy_tex			= nullptr;
	enemy_gatherer_tex	= nullptr;
	enemy_scout_tex		= nullptr;
	enemy_infantry_tex	= nullptr;
	enemy_heavy_tex		= nullptr;

	townhall_tex		= nullptr;
	barracks_tex		= nullptr;
	wall_tex			= nullptr;
	enemy_townhall_tex	= nullptr;
	enemy_barracks_tex	= nullptr;
	enemy_wall_tex		= nullptr;

	rock_tex			= nullptr;
	tree_tex			= nullptr;
	bits_tex			= nullptr;
}

// --- ENTITY AUDIO LOAD & UNLOAD ---
void EntityManager::LoadEntityAudios()
{
	//Pass to xml
	gatherer_movement_fx	= App->audio->LoadFx("audio/fx/Gatherer_movement.wav");
	gather_fx				= App->audio->LoadFx("audio/fx/Gathering.wav");
	finished_gather_fx		= App->audio->LoadFx("audio/fx/Finnished_gathering.wav");
	infantry_movement_fx	= App->audio->LoadFx("audio/fx/Infantry_movement.wav");
	infantry_shot_fx		= App->audio->LoadFx("audio/fx/Infantry_shot.wav");
	click_barracks_fx		= App->audio->LoadFx("audio/fx/Click Barracks.wav");
	building_fx				= App->audio->LoadFx("audio/fx/Building.wav");
	finished_building_fx	= App->audio->LoadFx("audio/fx/Finished_building.wav");
	finished_recruiting_fx	= App->audio->LoadFx("audio/fx/Finished_recruiting.wav");
	finished_upgrading_fx	= App->audio->LoadFx("audio/fx/Finished_Upgrading.wav");
	click_townhall_fx		= App->audio->LoadFx("audio/fx/Click_Townhall.wav");
}

void EntityManager::UnLoadEntityAudios()
{
	
}

// --- ENTITY TEXTURE GETTERS ---
SDL_Texture* EntityManager::GetGathererTexture() const
{
	return gatherer_tex;
}

SDL_Texture* EntityManager::GetScoutTexture() const
{
	return scout_tex;
}

SDL_Texture* EntityManager::GetInfantryTexture() const
{
	return infantry_tex;
}

SDL_Texture* EntityManager::GetHeavyTexture() const
{
	return heavy_tex;
}

SDL_Texture* EntityManager::GetEnemyGathererTexture() const
{
	return enemy_gatherer_tex;
}

SDL_Texture* EntityManager::GetEnemyScoutTexture() const
{
	return enemy_scout_tex;
}

SDL_Texture* EntityManager::GetEnemyInfantryTexture() const
{
	return enemy_infantry_tex;
}

SDL_Texture* EntityManager::GetEnemyHeavyTexture() const
{
	return enemy_heavy_tex;
}

SDL_Texture* EntityManager::GetTownHallTexture() const
{
	return townhall_tex;
}

SDL_Texture* EntityManager::GetBarracksTexture() const
{
	return barracks_tex;
}

SDL_Texture* EntityManager::GetWallTexture() const
{
	return wall_tex;
}

SDL_Texture* EntityManager::GetEnemyTownHallTexture() const
{
	return enemy_townhall_tex;
}

SDL_Texture* EntityManager::GetEnemyBarracksTexture() const
{
	return enemy_barracks_tex;
}

SDL_Texture* EntityManager::GetEnemyWallTexture() const
{
	return enemy_wall_tex;
}

SDL_Texture* EntityManager::GetRockTexture() const
{
	return rock_tex;
}

SDL_Texture* EntityManager::GetTreeTexture() const
{
	return tree_tex;
}

SDL_Texture* EntityManager::GetBitsTexture() const
{
	return bits_tex;
}

SDL_Texture* EntityManager::GetCenterPointTexture() const
{
	return center_point_debug;
}

// --- ENTITY CONDITIONAL METHODS
bool EntityManager::IsAllyEntity(Entity* entity)
{
	if (entity->type == ENTITY_TYPE::GATHERER || entity->type == ENTITY_TYPE::SCOUT || entity->type == ENTITY_TYPE::INFANTRY || entity->type == ENTITY_TYPE::HEAVY
		|| entity->type == ENTITY_TYPE::TOWNHALL || entity->type == ENTITY_TYPE::BARRACKS || entity->type == ENTITY_TYPE::WALL)
	{
		return true;
	}

	return false;
}

bool EntityManager::IsEnemyEntity(Entity* entity)
{
	if (entity->type == ENTITY_TYPE::ENEMY_GATHERER || entity->type == ENTITY_TYPE::ENEMY_SCOUT
		|| entity->type == ENTITY_TYPE::ENEMY_INFANTRY || entity->type == ENTITY_TYPE::ENEMY_HEAVY
		|| entity->type == ENTITY_TYPE::ENEMY_TOWNHALL || entity->type == ENTITY_TYPE::ENEMY_BARRACKS || entity->type == ENTITY_TYPE::ENEMY_WALL)
	{
		return true;
	}

	return false;
}

bool EntityManager::IsUnit(Entity* entity)	// Maybe change condition to !IsBuilding() or something.
{
	if (entity->type == ENTITY_TYPE::GATHERER || entity->type == ENTITY_TYPE::SCOUT || entity->type == ENTITY_TYPE::INFANTRY || entity->type == ENTITY_TYPE::HEAVY 
		|| entity->type == ENTITY_TYPE::ENEMY_GATHERER || entity->type == ENTITY_TYPE::ENEMY_SCOUT || entity->type == ENTITY_TYPE::ENEMY_INFANTRY || entity->type == ENTITY_TYPE::ENEMY_HEAVY)
	{
		return true;
	}

	return false;
}

bool EntityManager::IsAllyUnit(Entity* entity)	// Maybe change condition to !IsBuilding() or something.
{
	if (entity->type == ENTITY_TYPE::GATHERER || entity->type == ENTITY_TYPE::SCOUT || entity->type == ENTITY_TYPE::INFANTRY || entity->type == ENTITY_TYPE::HEAVY)
	{
		return true;
	}

	return false;
}

bool EntityManager::IsEnemyUnit(Entity* entity)	// Maybe change condition to !IsBuilding() or something.
{
	if (entity->type == ENTITY_TYPE::ENEMY_GATHERER || entity->type == ENTITY_TYPE::ENEMY_SCOUT || entity->type == ENTITY_TYPE::ENEMY_INFANTRY || entity->type == ENTITY_TYPE::ENEMY_HEAVY)
	{
		return true;
	}

	return false;
}

bool EntityManager::IsGatherer(Entity* entity)
{
	if (entity->type == ENTITY_TYPE::GATHERER)
	{
		return true;
	}

	return false;
}

bool EntityManager::IsScout(Entity* entity)
{
	if (entity->type == ENTITY_TYPE::SCOUT)
	{
		return true;
	}

	return false;
}

bool EntityManager::IsInfantry(Entity* entity)
{
	if (entity->type == ENTITY_TYPE::INFANTRY)
	{
		return true;
	}

	return false;
}

bool EntityManager::IsHeavy(Entity* entity)
{
	if (entity->type == ENTITY_TYPE::HEAVY)
	{
		return true;
	}

	return false;
}

bool EntityManager::IsBuilding(Entity* entity)
{
	if (entity->type == ENTITY_TYPE::TOWNHALL || entity->type == ENTITY_TYPE::BARRACKS || entity->type == ENTITY_TYPE::WALL
		|| entity->type == ENTITY_TYPE::ENEMY_TOWNHALL || entity->type == ENTITY_TYPE::ENEMY_BARRACKS || entity->type == ENTITY_TYPE::ENEMY_WALL)
	{
		return true;
	}

	return false;
}

bool EntityManager::IsResource(Entity* entity)
{
	if (entity->type == ENTITY_TYPE::ROCK || entity->type == ENTITY_TYPE::TREE || entity->type == ENTITY_TYPE::BITS)
	{
		return true;
	}

	return false;
}

bool EntityManager::InViewport(Entity* entity)
{
	SDL_Rect window;

	App->win->GetWindowRect(window);

	window.x = -App->render->camera.x - 100;
	window.y = -App->render->camera.y -100;
	window.w += 100;
	window.h += 100;

	//LOG("WINDOW RECT x %d y %d w %d h %d", window.x, window.y, window.w, window.h);
	//LOG("ENTITY POS x %d y %d", (int)entity->pixel_position.x, (int)entity->pixel_position.y);

	if (window.x < (int)entity->pixel_position.x
		&& window.x + window.w > (int)entity->pixel_position.x
		&& window.y < (int)entity->pixel_position.y
		&& window.y + window.h > (int)entity->pixel_position.y)
	{
		return true;
	}
	
	return false;
}

void EntityManager::SetEntityMap(int width, int height)
{
	if (entity_map != nullptr)
	{
		RELEASE_ARRAY(entity_map);
	}

	entity_map_width = width;
	entity_map_height = height;
	//RELEASE_ARRAY(entity_map); //Used to crash

	entity_map = new Entity* [width * height];

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

iPoint EntityManager::GetEntityPos(Entity* entity)
{
	return entity->tile_position;
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
				for (int y = 0; y != entity->tiles_occupied.y; ++y)
				{
					for (int x = 0; x != entity->tiles_occupied.x; ++x)
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
				StaticObject* item = (StaticObject*)entity;

				for (int y = 0; y != item->tiles_occupied.y; ++y)
				{
					for (int x = 0; x != item->tiles_occupied.x; ++x)
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
	return (pos.x >= 0 && pos.x < entity_map_width &&
		pos.y >= 0 && pos.y < entity_map_height);
}

Entity* EntityManager::GetEntityAt(const iPoint& pos) const
{
	if (entity_map != nullptr)
	{
		if (CheckEntityMapBoundaries(pos))
		{
			return entity_map[(pos.y * entity_map_width) + pos.x];
		}
	}

	return nullptr;
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
			for (int y = 0; y != entity->tiles_occupied.y; ++y)
			{
				for (int x = 0; x != entity->tiles_occupied.x; ++x)
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
#include "Application.h"
#include "Player.h"
#include "Pathfinding.h"
#include "DynamicObject.h"
#include "EntityManager.h"
#include "EnemyUnit.h"
#include "TownHall.h"
#include "Barracks.h"
#include "EnemyTownHall.h"
#include "EnemyBarracks.h"

#include "EnemyAIManager.h"

EnemyAIManager::EnemyAIManager() : Module()
, wave_timer(0.0f)
, ally_townhall(nullptr)
, ally_barracks(nullptr)
, enemy_townhall(nullptr)
, enemy_barracks(nullptr)
, ally_townhall_string("ally_townhall")
, ally_barracks_string("ally_barracks")
{
	name = ("enemy_AI_manager");
}

EnemyAIManager::~EnemyAIManager()
{

}

bool EnemyAIManager::Awake(pugi::xml_node& config)
{	
	return true;
}

bool EnemyAIManager::Start()
{
	InitEnemyAI();
	

	std::vector<EnemyAIEntity*>::iterator item = enemy_AI_entities.begin();

	for (; item != enemy_AI_entities.end(); ++item)
	{
		if (App->entity_manager->IsEnemyUnit((*item)->enemy_entity))
		{
			(*item)->AI_state == ENEMY_AI_STATE::PATROLING;
			patroling_entities.push_back((EnemyUnit*)(*item)->enemy_entity);
		}
	}

	return true;
}

bool EnemyAIManager::PreUpdate()
{
	return true;
}

bool EnemyAIManager::Update(float dt)
{
	if (App->player->CurrentlyInGameplayScene())
	{
		//wave_timer += dt;

		GivePatrolPath();
		
		//CheckWaveTimers();

		//GiveTargetOnWaveSpawnCompletion();
	}

	return true;
}

bool EnemyAIManager::PostUpdate()
{
	return true;
}

bool EnemyAIManager::CleanUp()
{
	ally_townhall = nullptr;
	ally_barracks = nullptr;

	enemy_townhall = nullptr;
	enemy_barracks = nullptr;
	
	DestroyEnemyAIEntities();

	enemy_waves.clear();
	enemy_wave_buffer.clear();
	patroling_entities.clear();
	
	return true;
}

void EnemyAIManager::SpawnEnemyWave(const uchar& gatherers_to_spawn, const uchar& scouts_to_spawn, const uchar& infantries_to_spawn, const uchar& heavys_to_spawn)
{
	if (enemy_townhall != nullptr)
	{
		for (int i = 0; i < gatherers_to_spawn; ++i)
		{
			enemy_townhall->creation_queue.push_back(ENTITY_TYPE::ENEMY_GATHERER);
		}

		for (int i = 0; i < scouts_to_spawn; ++i)
		{
			enemy_townhall->creation_queue.push_back(ENTITY_TYPE::ENEMY_SCOUT);
		}
	}

	if (enemy_barracks != nullptr)
	{
		for (int i = 0; i < infantries_to_spawn; ++i)
		{
			enemy_barracks->creation_queue.push_back(ENTITY_TYPE::ENEMY_INFANTRY);
		}

		for (int i = 0; i < heavys_to_spawn; ++i)
		{
			enemy_barracks->creation_queue.push_back(ENTITY_TYPE::ENEMY_HEAVY);
		}
	}
}

void EnemyAIManager::SpawnEnemyWave(const EnemyWave& enemy_wave)
{
	if (enemy_townhall != nullptr)
	{
		for (int i = 0; i < enemy_wave.gatherers_to_spawn; ++i)
		{
			enemy_townhall->creation_queue.push_back(ENTITY_TYPE::ENEMY_GATHERER);
		}

		for (int i = 0; i < enemy_wave.scouts_to_spawn; ++i)
		{
			enemy_townhall->creation_queue.push_back(ENTITY_TYPE::ENEMY_SCOUT);
		}
	}

	if (enemy_barracks != nullptr)
	{
		for (int i = 0; i < enemy_wave.infantries_to_spawn; ++i)
		{
			enemy_barracks->creation_queue.push_back(ENTITY_TYPE::ENEMY_INFANTRY);
		}

		for (int i = 0; i < enemy_wave.heavys_to_spawn; ++i)
		{
			enemy_barracks->creation_queue.push_back(ENTITY_TYPE::ENEMY_HEAVY);
		}
	}
}

void EnemyAIManager::GiveTargetOnWaveSpawnCompletion()
{
	if (enemy_wave_buffer.size() != 0)
	{
		if (enemy_townhall->creation_queue.size() == 0 && enemy_barracks->creation_queue.size() == 0)
		{
			GiveTargetToEnemyWave((*enemy_wave_buffer.begin()));

			enemy_wave_buffer.erase(enemy_wave_buffer.begin());
		}
	}
}

void EnemyAIManager::GiveTargetToEnemyWave(const EnemyWave& enemy_wave)
{
	std::vector<DynamicObject*> wave_units;

	std::vector<EnemyAIEntity*>::iterator item = enemy_AI_entities.begin();

	for (; item != enemy_AI_entities.end(); ++item)
	{
		if (App->entity_manager->IsEnemyUnit((*item)->enemy_entity))
		{
			if ((*item)->AI_state == ENEMY_AI_STATE::IDLE)
			{
				(*item)->AI_state == ENEMY_AI_STATE::ATTACKING;
				wave_units.push_back((DynamicObject*)(*item)->enemy_entity);
			}
		}
	}

	if (enemy_wave.target == ally_townhall_string)
	{
		if (ally_townhall != nullptr)
		{
			App->pathfinding->AttackOrder(ally_townhall->tile_position, wave_units);
		}
		else
		{
			if (ally_barracks != nullptr)
			{
				App->pathfinding->AttackOrder(ally_barracks->tile_position, wave_units);
			}
		}
	}

	if (enemy_wave.target == ally_barracks_string)
	{
		if (ally_barracks != nullptr)
		{
			App->pathfinding->AttackOrder(ally_barracks->tile_position, wave_units);
		}
		else
		{
			if (ally_townhall != nullptr)
			{
				App->pathfinding->AttackOrder(ally_townhall->tile_position, wave_units);
			}
		}
	}

	wave_units.clear();
}

void EnemyAIManager::GivePatrolPath()
{
	/*std::vector<DynamicObject*> patrol;
	
	for (int i = 0; i < patroling_entities.size(); ++i)
	{
		if (!patroling_entities[i]->has_proximity_target && patroling_entities[i]->entity_path.empty())
		{
			//Give Random Path
			patrol.push_back(patroling_entities[i]);

			App->pathfinding->MoveOrder(patroling_entities[i]->tile_position + iPoint(rand() % 5, rand() % 5), patrol);

			patrol.pop_back();
		}
	}*/
}

EnemyAIEntity* EnemyAIManager::CreateEnemyAIEntity(Entity* enemy_entity)
{
	EnemyAIEntity* enemy_AI = nullptr;

	enemy_AI = new EnemyAIEntity(enemy_entity);

	if (enemy_AI != nullptr)
	{
		enemy_AI_entities.push_back(enemy_AI);
	}

	return enemy_AI;
}

void EnemyAIManager::DeleteEnemyAIEntity(EnemyAIEntity* AI_entity_to_delete)
{
	if (enemy_townhall != nullptr)
	{
		if (enemy_townhall->enemy_AI_entity == AI_entity_to_delete)
		{
			EnemyTownhallReassignment();
		}
	}

	if (enemy_barracks != nullptr)
	{
		if (enemy_barracks->enemy_AI_entity == AI_entity_to_delete)
		{
			EnemyBarracksReassignment();
		}
	}
	
	std::vector<EnemyUnit*>::iterator unit = patroling_entities.begin();

	for (; unit != patroling_entities.end(); ++unit)
	{
		if ((*unit) == AI_entity_to_delete->enemy_entity)
		{
			patroling_entities.erase(unit);
			break;
		}
	}

	std::vector<EnemyAIEntity*>::iterator item = enemy_AI_entities.begin();

	for (; item != enemy_AI_entities.end(); ++item)
	{
		if ((*item) == AI_entity_to_delete)
		{
			(*item)->CleanUp();
			
			RELEASE((*item));
			enemy_AI_entities.erase(item);
			
			break;
		}
	}
}

void EnemyAIManager::DestroyEnemyAIEntities()
{
	std::vector<EnemyAIEntity*>::iterator item = enemy_AI_entities.begin();

	for (; item != enemy_AI_entities.end(); ++item)
	{
		if ((*item) != nullptr)
		{
			(*item)->CleanUp();
			RELEASE((*item));
		}
	}

	enemy_AI_entities.clear();
}

void EnemyAIManager::TownhallReassignment()
{
	ally_townhall = nullptr;
	
	/*std::vector<Entity*>::iterator entity = App->entity_manager->entities.begin();

	for (; entity != App->entity_manager->entities.end(); ++entity)
	{
		if ((*entity) != nullptr)
		{
			if ((*entity)->type == ENTITY_TYPE::TOWNHALL)
			{
				ally_townhall = (TownHall*)(*entity);
				break;
			}
		}
	}*/
}

void EnemyAIManager::BarracksReassignment()
{
	ally_barracks = nullptr;

	/*std::vector<Entity*>::iterator entity = App->entity_manager->entities.begin();

	for (; entity != App->entity_manager->entities.end(); ++entity)
	{
		if ((*entity) != nullptr)
		{
			if ((*entity)->type == ENTITY_TYPE::BARRACKS)
			{
				ally_barracks = (Barracks*)(*entity);
				break;
			}
		}
	}*/
}

void EnemyAIManager::EnemyTownhallReassignment()
{
	enemy_townhall = nullptr;
	
	/*for (int i = 0; i < enemy_AI_entities.size(); ++i)
	{
		if (enemy_AI_entities[i]->enemy_entity->type == ENTITY_TYPE::ENEMY_TOWNHALL)
		{
			enemy_townhall = (EnemyTownHall*)enemy_AI_entities[i]->enemy_entity;
			break;
		}
	}*/
}

void EnemyAIManager::EnemyBarracksReassignment()
{
	enemy_barracks = nullptr;
	
	/*for (int i = 0; i < enemy_AI_entities.size(); ++i)
	{
		if (enemy_AI_entities[i]->enemy_entity->type == ENTITY_TYPE::ENEMY_BARRACKS)
		{
			enemy_barracks = (EnemyBarracks*)enemy_AI_entities[i]->enemy_entity;
			break;
		}
	}*/
} 

void EnemyAIManager::CheckWaveTimers()
{
	std::vector<EnemyWave>::iterator wave = enemy_waves.begin();

	for (; wave != enemy_waves.end(); ++wave)
	{
		if (wave_timer > (*wave).spawn_timer)
		{
			SpawnEnemyWave((*wave));
			
			enemy_wave_buffer.push_back((*wave));
			enemy_waves.erase(wave);
		}
	}
}

void EnemyAIManager::InitEnemyAI()
{
	pugi::xml_node wave_node = App->entities_file.child("entities").child("enemy_AI").child("enemy_waves").child("wave");

	for (; wave_node != nullptr; wave_node = wave_node.next_sibling("wave"))
	{
		EnemyWave enemy_wave;

		enemy_wave.spawn_timer			= wave_node.attribute("spawn_timer").as_float();
		enemy_wave.target				= wave_node.attribute("target").as_string();
		enemy_wave.gatherers_to_spawn	= (uchar)wave_node.attribute("gatherers_to_spawn").as_uint();
		enemy_wave.scouts_to_spawn		= (uchar)wave_node.attribute("scouts_to_spawn").as_uint();
		enemy_wave.infantries_to_spawn	= (uchar)wave_node.attribute("infantries_to_spawn").as_uint();
		enemy_wave.heavys_to_spawn		= (uchar)wave_node.attribute("heavys_to_spawn").as_uint();

		enemy_waves.push_back(enemy_wave);
	}
}

// --------- ENEMY AI ENTITY METHODS ---------
EnemyAIEntity::EnemyAIEntity(Entity* enemy_entity) : enemy_entity(enemy_entity), AI_state(ENEMY_AI_STATE::IDLE), reached_target(false)
{

}

EnemyAIEntity::EnemyAIEntity() : enemy_entity(nullptr), AI_state(ENEMY_AI_STATE::NONE), reached_target(false)
{

}

void EnemyAIEntity::CleanUp()
{

}
#include "Application.h"
#include "EntityManager.h"
#include "EnemyTownHall.h"
#include "EnemyBarracks.h"

#include "EnemyAIManager.h"

EnemyAIManager::EnemyAIManager() : Module(), enemy_townhall(nullptr), enemy_barracks(nullptr)
{

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
	return true;
}

bool EnemyAIManager::PreUpdate()
{
	return true;
}

bool EnemyAIManager::Update(float dt)
{
	enemy_barracks;
	enemy_townhall;
	return true;
}

bool EnemyAIManager::PostUpdate()
{
	return true;
}

bool EnemyAIManager::CleanUp()
{
	return true;
}

void EnemyAIManager::SpawnEnemyWave(int gatherer_amount, int scout_amount, int infantry_amount, int heavy_amount)
{
	EnemyTownHall* enemy_townhall = nullptr;
	EnemyBarracks* enemy_barracks = nullptr;

	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item < App->entity_manager->entities.end(); ++item)									// Finding Both the enemy townhall and the enemy barracks.
	{
		if (enemy_townhall == nullptr || enemy_barracks == nullptr)
		{
			if ((*item)->type == ENTITY_TYPE::ENEMY_TOWNHALL)
			{
				enemy_townhall = (EnemyTownHall*)(*item);
			}

			if ((*item)->type == ENTITY_TYPE::ENEMY_BARRACKS && enemy_barracks == nullptr)			// COULD THERE BE MULTIPLE ENEMY BARRACKS?
			{
				enemy_barracks = (EnemyBarracks*)(*item);
			}
		}
		else
		{
			break;
		}
	}

	if (enemy_townhall != nullptr)
	{
		for (int i = 0; i < gatherer_amount; ++i)
		{
			enemy_townhall->creation_queue.push_back(ENTITY_TYPE::ENEMY_GATHERER);
		}

		for (int i = 0; i < scout_amount; ++i)
		{
			enemy_townhall->creation_queue.push_back(ENTITY_TYPE::ENEMY_SCOUT);
		}
	}

	if (enemy_barracks != nullptr)
	{
		for (int i = 0; i < infantry_amount; ++i)
		{
			enemy_barracks->creation_queue.push_back(ENTITY_TYPE::ENEMY_INFANTRY);
		}

		for (int i = 0; i < heavy_amount; ++i)
		{
			enemy_barracks->creation_queue.push_back(ENTITY_TYPE::ENEMY_HEAVY);
		}
	}
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
		(*item)->CleanUp();
		RELEASE((*item));
	}

	enemy_AI_entities.clear();
}

// --------- ENEMY AI ENTITY METHODS ---------
EnemyAIEntity::EnemyAIEntity(Entity* enemy_entity) : enemy_entity(enemy_entity), AI_state(ENEMY_AI_STATE::IDLE)
{

}

EnemyAIEntity::EnemyAIEntity() : enemy_entity(nullptr), AI_state(ENEMY_AI_STATE::NONE)
{

}

void EnemyAIEntity::CleanUp()
{

}
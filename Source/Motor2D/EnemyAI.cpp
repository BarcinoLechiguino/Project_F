#include "Application.h"
#include "EntityManager.h"
#include "EnemyTownHall.h"
#include "EnemyBarracks.h"

#include "EnemyAI.h"

EnemyAI::EnemyAI() : Module()
{

}

EnemyAI::~EnemyAI()
{

}

bool EnemyAI::Awake(pugi::xml_node& config)
{
	return true;
}

bool EnemyAI::Start()
{
	return true;
}

bool EnemyAI::PreUpdate()
{
	return true;
}

bool EnemyAI::Update(float dt)
{
	return true;
}

bool EnemyAI::PostUpdate()
{
	return true;
}

bool EnemyAI::CleanUp()
{
	return true;
}

void EnemyAI::SpawnEnemyWave(int gatherer_amount, int scout_amount, int infantry_amount, int heavy_amount)
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
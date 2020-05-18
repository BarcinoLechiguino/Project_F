#ifndef __ENEMY_AI_H__
#define __ENEMY_AI_H__

#include "Module.h"

class Entity;
class EnemyTownHall;
class EnemyBarracks;

enum class ENEMY_AI_STATE
{
	NONE,
	IDLE,
	PATROLING,
	ATTACKING
};

struct EnemyAIEntity
{
	EnemyAIEntity(Entity* enemy_entity);
	EnemyAIEntity();

	void CleanUp();
	
	Entity* enemy_entity;
	ENEMY_AI_STATE AI_state;
};

class EnemyAIManager : public Module
{
public:
	EnemyAIManager();
	~EnemyAIManager();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

public:
	void SpawnEnemyWave(int gatherer_amount, int scout_amount, int infantry_amount, int heavy_amount);

	EnemyAIEntity* CreateEnemyAIEntity(Entity* enemy_entity);
	void DeleteEnemyAIEntity(EnemyAIEntity* AI_entity_to_delete);											// Will delete the given FowEntity from the fow_entities vector.
	void DestroyEnemyAIEntities();

public:
	EnemyTownHall* enemy_townhall;
	EnemyBarracks* enemy_barracks;

	std::vector<EnemyAIEntity*> enemy_AI_entities;

private:

};

#endif // !__ENEMY_AI_H__
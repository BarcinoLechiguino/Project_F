#ifndef __ENEMY_AI_H__
#define __ENEMY_AI_H__

#include "Module.h"

class Entity;
class EnemyUnit;
class TownHall;
class Barracks;
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
	
	Entity*			enemy_entity;
	ENEMY_AI_STATE	AI_state;
	bool			reached_target;
};

struct EnemyWave
{
	float			spawn_timer;
	std::string		target;

	uchar			gatherers_to_spawn;
	uchar			scouts_to_spawn;
	uchar			infantries_to_spawn;
	uchar			heavys_to_spawn;
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
	void InitEnemyAI();
	
	void SpawnEnemyWave(const uchar& gatherers_to_spawn, const uchar& scouts_to_spawn, const uchar& infantries_to_spawn, const uchar& heavys_to_spawn);
	void SpawnEnemyWave(const EnemyWave& enemy_wave);
	void GiveTargetOnWaveSpawnCompletion();
	void GiveTargetToEnemyWave(const EnemyWave& enemy_wave);

	void GivePatrolPath();

	EnemyAIEntity* CreateEnemyAIEntity(Entity* enemy_entity);
	void DeleteEnemyAIEntity(EnemyAIEntity* AI_entity_to_delete);														// Will delete the given EnemyAIEntity from the fow_entities vector.
	void DestroyEnemyAIEntities();

	void TownhallReassignment();
	void BarracksReassignment();
	void EnemyTownhallReassignment();
	void EnemyBarracksReassignment();

	void CheckWaveTimers();

public:
	TownHall*					ally_townhall;
	Barracks*					ally_barracks;
	
	EnemyTownHall*				enemy_townhall;
	EnemyBarracks*				enemy_barracks;

private:
	float						wave_timer;
	
	std::string					ally_townhall_string;
	std::string					ally_barracks_string;

	std::vector<EnemyAIEntity*> enemy_AI_entities;
	std::vector<EnemyUnit*>		patroling_entities;
	std::vector<EnemyWave>		enemy_waves;
	std::vector<EnemyWave>		enemy_wave_buffer;

};

#endif // !__ENEMY_AI_H__
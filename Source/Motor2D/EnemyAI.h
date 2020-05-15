#ifndef __ENEMY_AI_H__
#define __ENEMY_AI_H__

#include "Module.h"

class EnemyAI : public Module
{
public:
	EnemyAI();
	~EnemyAI();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

public:
	void SpawnEnemyWave(int gatherer_amount, int scout_amount, int infantry_amount, int heavy_amount);

private:

};

#endif // !__ENEMY_AI_H__
#ifndef __ENEMY_UNIT_H__
#define __ENEMY_UNIT_H__

#include "DynamicObject.h"

struct EnemyAIEntity;

class EnemyUnit : public DynamicObject
{
public:
	EnemyUnit(int x, int y, ENTITY_TYPE type, int level);
	virtual ~EnemyUnit();

	virtual bool Awake(pugi::xml_node&);
	virtual bool Start();
	virtual bool PreUpdate();
	virtual bool Update(float dt, bool do_logic);
	virtual bool PostUpdate();
	virtual bool CleanUp();

	virtual void Draw();

public:
	EnemyAIEntity* enemy_AI_entity;

private:

};

#endif // !__ENEMY_UNIT_H__
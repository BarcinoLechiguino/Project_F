#ifndef __ENEMY_TOWNHALL_H__
#define __ENEMY_TOWNHALL_H__

#include "Static_Object.h"

class EnemyTownHall : public Static_Object
{
public:

	EnemyTownHall(int x, int y, ENTITY_TYPE type);

	virtual bool Awake(pugi::xml_node&);

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

public:

	void GenerateUnit(ENTITY_TYPE type);

public:
	SDL_Rect	hall_rect;
};

#endif // !__ENEMY_TOWNHALL_H__

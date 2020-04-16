#ifndef __ENEMY_BARRACKS_H__
#define __ENEMY_BARRACKS_H__

#include "Static_Object.h"

class EnemyBarracks : public Static_Object
{
public:

	EnemyBarracks(int x, int y, ENTITY_TYPE type);

	virtual bool Awake(pugi::xml_node&);

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

public:

	void GenerateUnit(ENTITY_TYPE type);

public:
	SDL_Rect	barracks_rect_1;
	SDL_Rect	barracks_rect_2;
};

#endif // !__ENEMY_BARRACKS_H__
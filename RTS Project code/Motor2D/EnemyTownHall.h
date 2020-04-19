#ifndef __ENEMY_TOWNHALL_H__
#define __ENEMY_TOWNHALL_H__

#include "Static_Object.h"

class EnemyTownHall : public Static_Object
{
public:

	EnemyTownHall(int x, int y, ENTITY_TYPE type, int level);

	virtual bool Awake(pugi::xml_node&);

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

	virtual void Draw();

public:
	void InitEntity();
	void AttachHealthbarToEntity();

	void GenerateUnit(ENTITY_TYPE type, int level);

	void LevelChanges();

public:
	SDL_Rect	hall_rect;
	SDL_Rect	hall_rect_1;
	SDL_Rect	hall_rect_2;
};

#endif // !__ENEMY_TOWNHALL_H__

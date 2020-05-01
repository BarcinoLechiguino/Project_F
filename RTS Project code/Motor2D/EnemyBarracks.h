#ifndef __ENEMY_BARRACKS_H__
#define __ENEMY_BARRACKS_H__

#include "Static_Object.h"

class EnemyBarracks : public Static_Object
{
public:

	EnemyBarracks(int x, int y, ENTITY_TYPE type, int level);

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
	SDL_Rect	barracks_rect_1;
	SDL_Rect	barracks_rect_2;
	SDL_Rect	barracks_rect;

	ENTITY_TYPE created_unit_type;
	
	float		enemy_scout_creation_time;
	float		enemy_infantry_creation_time;
	float		enemy_heavy_creation_time;
};

#endif // !__ENEMY_BARRACKS_H__
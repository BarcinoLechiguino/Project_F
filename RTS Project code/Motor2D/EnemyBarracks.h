#ifndef __ENEMY_BARRACKS_H__
#define __ENEMY_BARRACKS_H__

#include "Building.h"

class EnemyBarracks : public Building
{
public:
	EnemyBarracks(int x, int y, ENTITY_TYPE type, int level);
	~EnemyBarracks();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt, bool do_logic);
	bool PostUpdate();
	bool CleanUp();

	void Draw();

public:
	void InitEntity();
	void AttachHealthbarToEntity();
	void AttachCreationBarToEntity();

	void StartUnitCreation();

	void GenerateUnit(ENTITY_TYPE type, int level);

	void LevelChanges();

public:
	SDL_Rect	barracks_rect_1;
	SDL_Rect	barracks_rect_2;
	SDL_Rect	barracks_rect;

	int			unit_level;
	
	float		enemy_scout_creation_time;
	float		enemy_infantry_creation_time;
	float		enemy_heavy_creation_time;

	ENTITY_TYPE created_unit_type;
	bool		creating_unit;
};

#endif // !__ENEMY_BARRACKS_H__
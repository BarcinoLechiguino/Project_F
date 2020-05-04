#ifndef __TOWNHALL_H__
#define __TOWNHALL_H__

#include "Building.h"

class TownHall : public Building
{
public:
	TownHall(int x, int y, ENTITY_TYPE type, int level);
	~TownHall();

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
	SDL_Rect	hall_rect;
	SDL_Rect	hall_rect_1;
	SDL_Rect	hall_rect_2;

	int			unit_level;

	float		gatherer_creation_time;

	ENTITY_TYPE created_unit_type;
	bool		creating_unit;
};

#endif // !__TOWNHALL_H__
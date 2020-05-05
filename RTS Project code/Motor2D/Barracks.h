#ifndef __BARRACKS_H__
#define __BARRACKS_H__

#include "Building.h"

class Barracks : public Building
{
public:
	Barracks(int x, int y, ENTITY_TYPE type, int level);
	~Barracks();

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

	void GenerateUnit(ENTITY_TYPE type, int level);				// Will spawn a unit given a type and a level.
	void GenerateUnitByType(ENTITY_TYPE type);					// Will spawn a unit given a type. The level is deduced from the type (See scout_level, infantry_level and heavy_level).

	void LevelChanges();

public:
	SDL_Rect	barracks_rect;
	SDL_Rect	barracks_rect_1;
	SDL_Rect	barracks_rect_2;

	int			scout_level;
	int			infantry_level;
	int			heavy_level;

	float		scout_creation_time;
	float		infantry_creation_time;
	float		heavy_creation_time;

	ENTITY_TYPE created_unit_type;
	bool		creating_unit;
};

#endif // !__BARRACKS_H__
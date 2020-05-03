#ifndef __BARRACKS_H__
#define __BARRACKS_H__

#include "Static_Object.h"

class Barracks : public Static_Object
{
public:

	Barracks(int x, int y, ENTITY_TYPE type, int level);

	virtual bool Awake(pugi::xml_node&);

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

	virtual void Draw();

public:
	void InitEntity();
	void AttachHealthbarToEntity();
	void AttachCreationBarToEntity();

	void StartUnitCreation();

	void GenerateUnit(ENTITY_TYPE type, int level);

	void LevelChanges();

public:
	SDL_Rect	barracks_rect;
	SDL_Rect	barracks_rect_1;
	SDL_Rect	barracks_rect_2;

	int			unit_level;

	float		scout_creation_time;
	float		infantry_creation_time;
	float		heavy_creation_time;

	ENTITY_TYPE created_unit_type;
	bool		creating_unit;
};

#endif // !__BARRACKS_H__
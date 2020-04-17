#ifndef __TOWNHALL_H__
#define __TOWNHALL_H__

#include "Static_Object.h"

class TownHall : public Static_Object
{
public:

	TownHall(int x, int y, ENTITY_TYPE type, int level);

	virtual bool Awake(pugi::xml_node&);

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

public:

	void GenerateUnit(ENTITY_TYPE type, int level);

public:
	SDL_Rect	hall_rect;
};

#endif // !__TOWNHALL_H__
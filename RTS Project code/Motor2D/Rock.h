#ifndef __ROCK_H__
#define __ROCK_H__

#include "Static_Object.h"

class Rock : public Static_Object
{
public:

	Rock(int x, int y, ENTITY_TYPE type);

	virtual bool Awake(pugi::xml_node&);

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

public:

	int ore;

	int gather_time;
};

#endif // !__ROCK_H__
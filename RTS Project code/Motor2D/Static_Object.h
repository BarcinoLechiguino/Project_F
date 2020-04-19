#ifndef __STATIC_OBJECT_H__
#define __STATIC_OBJECT_H__

#include "Entity.h"

class Static_Object : public Entity
{
public:

	Static_Object(int x, int y, ENTITY_TYPE type, int level);

	virtual bool Awake(pugi::xml_node&);

	virtual bool Start();

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

	virtual void Draw();

	//virtual void LevelChanges();

public:
	int tile_width;
	int tile_height;

	int tiles_occupied_x;
	int tiles_occupied_y;
};

#endif // !__STATIC_OBJECT_H__
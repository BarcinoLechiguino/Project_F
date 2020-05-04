#ifndef __STATIC_OBJECT_H__
#define __STATIC_OBJECT_H__

#include "Entity.h"

enum class BUILDING_STATE
{
	UNKNOWN,
	IDLE,
	CONSTRUCTING,
	GENERATING_UNIT,
	DAMAGED,
	DESTROYED
};

class StaticObject : public Entity
{
public:
	StaticObject(int x, int y, ENTITY_TYPE type, int level);
	virtual ~StaticObject();

	virtual bool Awake(pugi::xml_node&);
	virtual bool Start();
	virtual bool PreUpdate();
	virtual bool Update(float dt, bool do_logic);
	virtual bool PostUpdate();
	virtual bool CleanUp();

	virtual void Draw();

	//virtual void LevelChanges();

public:
	BUILDING_STATE			state;

	int						tile_width;
	int						tile_height;
};

#endif // !__STATIC_OBJECT_H__
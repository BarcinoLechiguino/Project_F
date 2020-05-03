#ifndef __STATIC_OBJECT_H__
#define __STATIC_OBJECT_H__

#include "Entity.h"

class UI_CreationBar;

enum class BUILDING_STATE
{
	UNKNOWN,
	IDLE,
	CONSTRUCTING,
	GENERATING_UNIT,
	DAMAGED,
	DESTROYED
};

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
	BUILDING_STATE			state;

	int						tile_width;
	int						tile_height;

	std::vector<ENTITY_TYPE>	creation_queue;

	UI_CreationBar*			creation_bar;

	SDL_Rect				creation_bar_background_rect;						// The background sprite of an entity's healthbar.
	SDL_Rect				creation_bar_rect;									// The foremost sprite of an entity's healthbar.
	iPoint					creation_bar_position_offset;						// Value that will affect where around the entity the healthbar will appear.
};

#endif // !__STATIC_OBJECT_H__
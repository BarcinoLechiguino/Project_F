#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "StaticObject.h"

#define MAX_BUILDING_LEVEL 2									//TMP. Right now all buildings have the same level cap.

class GuiCreationBar;

enum class BUILDING_STATE
{
	UNKNOWN,
	IDLE,
	CONSTRUCTING,
	GENERATING_UNIT,
	DAMAGED,
	DESTROYED
};

class Building : public StaticObject
{
public:
	Building(int x, int y, ENTITY_TYPE type, int level);
	virtual ~Building();

	virtual bool Awake(pugi::xml_node&);
	virtual bool Start();
	virtual bool PreUpdate();
	virtual bool Update(float dt, bool do_logic);
	virtual bool PostUpdate();
	virtual bool CleanUp();

	virtual void Draw();

public:
	std::vector<ENTITY_TYPE>	creation_queue;

	BUILDING_STATE				state;
	
	GuiCreationBar*				creation_bar;

	SDL_Rect					creation_bar_background_rect;						// The background sprite of an entity's healthbar.
	SDL_Rect					creation_bar_rect;									// The foremost sprite of an entity's healthbar.
	iPoint						creation_bar_position_offset;						// Value that will affect where around the entity the healthbar will appear.
};

#endif // !__BUILDING_H__
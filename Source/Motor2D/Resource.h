#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "StaticObject.h"

class Resource : public StaticObject
{
public:
	Resource(int x, int y, ENTITY_TYPE type, int level);
	virtual ~Resource();

	virtual bool Awake(pugi::xml_node&);
	virtual bool Start();
	virtual bool PreUpdate();
	virtual bool Update(float dt, bool do_logic);
	virtual bool PostUpdate();
	virtual bool CleanUp();

	virtual void Draw();

public:
	SDL_Rect* blit_section;
};

#endif // !__RESOURCE_H__
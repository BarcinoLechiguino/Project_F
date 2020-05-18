#ifndef __ALLY_UNIT_H__
#define __ALLY_UNIT_H__

#include "DynamicObject.h"

class AllyUnit : public DynamicObject
{
public:
	AllyUnit(int x, int y, ENTITY_TYPE type, int level);
	virtual ~AllyUnit();

	virtual bool Awake(pugi::xml_node&);
	virtual bool Start();
	virtual bool PreUpdate();
	virtual bool Update(float dt, bool do_logic);
	virtual bool PostUpdate();
	virtual bool CleanUp();

	virtual void Draw();

private:


};

#endif // !__ALLY_UNIT_H__
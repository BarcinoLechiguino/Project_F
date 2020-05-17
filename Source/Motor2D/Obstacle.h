#ifndef __OBSTACLE_H__
#define __OBSTACLE_H__

#include "StaticObject.h"

class Obstacle : public StaticObject
{
public:
	Obstacle(int x, int y, ENTITY_TYPE type, int level);
	virtual ~Obstacle();

	virtual bool Awake(pugi::xml_node&);
	virtual bool Start();
	virtual bool PreUpdate();
	virtual bool Update(float dt, bool do_logic);
	virtual bool PostUpdate();
	virtual bool CleanUp();

	virtual void Draw();
};

#endif // !__OBSTACLE_H__
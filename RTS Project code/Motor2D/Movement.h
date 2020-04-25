#ifndef __MOVEMENT_H__
#define __MOVEMENT_H__

#include "Module.h"

class Movement : public Module
{
public:
	Movement();
	~Movement();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	bool CleanUp();

public:
	//void GiveNewTile();

private:

};

#endif // !__MOVEMENT_H__

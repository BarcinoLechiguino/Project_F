#pragma once
#ifndef __TREE_H__
#define __TREE_H__

#include "Static_Object.h"

class Tree : public Static_Object
{
public:

	Tree(int x, int y, ENTITY_TYPE type, int level);

	virtual bool Awake(pugi::xml_node&);

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

	virtual void Draw();

public:
	void InitEntity();

public:
	int wood;

	int gather_time;

private:
	SDL_Rect* blit_section;

};

#endif // !__TREE_H__
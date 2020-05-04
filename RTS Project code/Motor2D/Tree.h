#pragma once
#ifndef __TREE_H__
#define __TREE_H__

#include "Resource.h"

class Tree : public Resource
{
public:
	Tree(int x, int y, ENTITY_TYPE type, int level);
	~Tree();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt, bool do_logic);
	bool PostUpdate();
	bool CleanUp();

	void Draw();

public:
	void InitEntity();

	void AttachHealthbarToEntity();

private:

};

#endif // !__TREE_H__
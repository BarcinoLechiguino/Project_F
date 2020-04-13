#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Dynamic_Object.h"

class Infantry;

class Enemy : public Dynamic_Object
{
public:

	Enemy(int x, int y, ENTITY_TYPE type);

	~Enemy();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt,  bool doLogic);

	bool PostUpdate();

	bool CleanUp();

	void OnCollision(Collider* C1, Collider* C2);

public:
	void SetTarget();		//TMP

public:

	const std::vector<iPoint>* entity_path;
	Entity* target;

};

#endif // __ENEMY_H__
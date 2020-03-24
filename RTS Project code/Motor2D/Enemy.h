#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Module.h"
#include "Animation.h"
#include "Point.h"
#include <vector>

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
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	
public:
	Entity_State	state;

	const std::vector<iPoint>* entity_path;

};

#endif // __ENEMY_H__
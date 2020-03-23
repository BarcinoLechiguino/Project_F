#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Module.h"
#include "Entity.h"
#include "Dynamic_Object.h"
#include "Animation.h"
#include "Point.h"
#include <vector>

enum class Entity_State
{
	IDLE = 0,
	PATHING_UP,
	PATHING_DOWN,
	PATHING_RIGHT,
	PATHING_LEFT,
	PATHING_UP_RIGHT,
	PATHING_UP_LEFT,
	PATHING_DOWN_RIGHT,
	PATHING_DOWN_LEFT,
	JUMPING,
	FALLING,
	DEAD,
	HURT
};

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
#ifndef __j1Enemy_H__
#define __j1Enemy_H__

#include "j1Module.h"
#include "j1Entity.h"
#include "j1Dynamic_Object.h"
#include "Animation.h"
#include "p2Point.h"

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

class j1Enemy : public j1Dynamic_Object
{
public:
	j1Enemy(int x, int y, ENTITY_TYPE type);

	~j1Enemy();

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
	Entity_State	state;									//State in which the entity is in any given moment.

	const p2DynArray<iPoint>* entity_path;					//Will store the path created between the enemy entity and a player.

	
	
};

#endif // __j1ENEMY_H__
#ifndef __GATHERER_H__
#define __GATHERER_H__

#include "Dynamic_Object.h"

class Gatherer : public Dynamic_Object
{
public:

	Gatherer(int x, int y, ENTITY_TYPE type);

	~Gatherer();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt, bool doLogic);

	bool PostUpdate();

	bool CleanUp();

public:
	void AssignEntityIndex();

	void InitUnitSpriteSections();
	void UpdateUnitSpriteSection();

	void OnCollision(Collider* C1, Collider* C2);

public:
	int		gatherer_index;
	
	float	gathering_speed;
};

#endif // __ENEMY_H__
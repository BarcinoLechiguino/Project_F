#ifndef __INFANTRY_H__
#define __INFANTRY_H__

#include "Dynamic_Object.h"

class Infantry : public Dynamic_Object
{
public:

	Infantry(int x, int y, ENTITY_TYPE type);

	~Infantry();

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
	
	void ApplyDamage(Entity* objective);

	void OnCollision(Collider* C1, Collider* C2);

public:
	int							infantry_index;

	int							attack_range;							// A unit's attack range in tiles.
	float						attack_speed;							// A unit's attack speed in attacks/second.

	const std::vector<iPoint>* entity_path;
};

#endif // __ENEMY_H__
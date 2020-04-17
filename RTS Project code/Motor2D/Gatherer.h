#ifndef __GATHERER_H__
#define __GATHERER_H__

#include "Dynamic_Object.h"

class Gatherer : public Dynamic_Object
{
public:

	Gatherer(int x, int y, ENTITY_TYPE type, int level);

	~Gatherer();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt, bool doLogic);

	bool PostUpdate();

	bool CleanUp();

public:
	void InitEntity();
	
	void AttachHealthbarToEntity();

	void InitUnitSpriteSections();
	void UpdateUnitSpriteSection();

	void SetGatheringTarget(const iPoint& tile_position);
	void PathToGatheringTarget();

	bool TargetIsInRange();

	void OnCollision(Collider* C1, Collider* C2);

public:
	float	gathering_speed;
	int		gathering_amount;
};

#endif // __ENEMY_H__
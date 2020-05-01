#ifndef __HEAVY_H__
#define __HEAVY_H__

#include "Dynamic_Object.h"

class Heavy : public Dynamic_Object
{
public:

	Heavy(int x, int y, ENTITY_TYPE type, int level);

	~Heavy();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt, bool doLogic);

	bool PostUpdate();

	bool CleanUp();

	void Draw();

public:
	void InitEntity();

	void AttachHealthbarToEntity();

	void InitUnitSpriteSections();
	void UpdateUnitSpriteSection();

	void SetEntityTargetByProximity();
	void GetShortestPathWithinAttackRange();
	void UpdateUnitOrientation();

	bool TargetIsInRange();
	void ChaseTarget();
	void DealDamage();

	void OnCollision(Collider* C1, Collider* C2);

public:
	int							attack_range;							// A unit's attack range in tiles.
	int							attack_radius;							// The unit's effective attack area. The heavy has an AoE attack. Radius in tiles.
	float						attack_speed;							// A unit's attack speed in attacks/second.

	//const std::vector<iPoint>*	entity_path;
};

#endif // !__HEAVY_H__
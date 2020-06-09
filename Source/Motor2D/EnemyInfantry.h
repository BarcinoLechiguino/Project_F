#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "EnemyUnit.h"

class Infantry;

class EnemyInfantry : public EnemyUnit
{
public:
	EnemyInfantry(int x, int y, ENTITY_TYPE type, int level);
	~EnemyInfantry();
	
	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt,  bool do_logic);
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

	Entity* GetTarget();
	int GetAttackRange();

public:
	/*int							attack_range;							// A unit's attack range in tiles.
	float						attack_speed;							// A unit's attack speed in attacks/second.*/

	//const std::vector<iPoint>* entity_path;
};

#endif // __ENEMY_H__
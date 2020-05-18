#ifndef __ENEMY_GATHERER_H__
#define __ENEMY_GATHERER_H__

#include "DynamicObject.h"

class EnemyGatherer : public DynamicObject
{
public:
	EnemyGatherer(int x, int y, ENTITY_TYPE type, int level);
	~EnemyGatherer();

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

	void InitUnitSpriteSections();
	void UpdateUnitSpriteSection();

	void SetGatheringTarget(const iPoint& tile_position);
	void PathToGatheringTarget();

	bool TargetIsInRange();
	void GatherResource();

	Entity* GetTarget();
	int GetAttackRange();

	//void HandleFx();
	//void SubstractResource();

	void OnCollision(Collider* C1, Collider* C2);

public:
	float	gathering_speed;
	int		gathering_amount_data;
	int		gathering_amount_electricity;
	int		gathering_amount_bits;
	bool	gather_in_cooldown;

};

#endif // !__ENEMY_GATHERER_H__
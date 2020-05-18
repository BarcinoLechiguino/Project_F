#ifndef __GATHERER_H__
#define __GATHERER_H__

#include "DynamicObject.h"

class Emitter;

class Gatherer : public DynamicObject
{
public:
	Gatherer(int x, int y, ENTITY_TYPE type, int level);
	~Gatherer();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt, bool do_logic);
	bool PostUpdate();
	bool CleanUp();

	void Draw();

public:
	void InitEntity();
	void InitUnitSpriteSections();
	void AttachHealthbarToEntity();

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
	bool	gather_in_cooldown;
	float	gathering_speed;
	int		gathering_amount_data;
	int		gathering_amount_electricity;
	int		gathering_amount_bits;

private:
	Emitter* SmokeEmitter;
};

#endif // !__GATHERER_H__
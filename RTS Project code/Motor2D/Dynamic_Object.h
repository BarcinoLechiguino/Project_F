#ifndef __DYNAMIC_OBJECT_H__
#define __DYNAMIC_OBJECT_H__

#include "Entity.h"

enum class PATHFINDING_STATE
{
	IDLE,
	WALKING,
	WAITING_NEXT_TILE
};

enum class ENTITY_STATE //Maybe WALKING instead?
{
	IDLE,
	PATHING_UP,
	PATHING_DOWN,
	PATHING_RIGHT,
	PATHING_LEFT,
	PATHING_UP_RIGHT,
	PATHING_UP_LEFT,
	PATHING_DOWN_RIGHT,
	PATHING_DOWN_LEFT,
	DAMAGED,
	DEAD,
};

class Dynamic_Object : public Entity
{
public:

	Dynamic_Object(int x, int y, ENTITY_TYPE type, int level);

	virtual bool Awake(pugi::xml_node&);

	virtual bool Start();

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

public:
	virtual void InitUnitSpriteSections();										// Initializes all sprite sections of a given unit.
	virtual void UpdateUnitSpriteSection();										// Updates all sprite sections of a given unit.
	virtual void UpdateUnitOrientation();										// Method that wil orient the entity towards the target it's attacking. unit_state should be IDLE.
	
	virtual void SetEntityTargetByProximity(const iPoint& target_position);		// Method that will give an entity a combat target.
	virtual void GetShortestPathWithinAttackRange();							// 
	virtual bool TargetIsInRange();												// Method that will check whether or not a unit's target is within it's attack_range.
	virtual void PathToTarget();
	virtual void DealDamage();													// Method that will deal damage to the target entity. Damage ticks are limited by attack_speed.

	virtual void SetGatheringTarget(const iPoint& tile_position);				// Method that will set as a target the selected Resource
	virtual void PathToGatheringTarget();										// Method that will define the path to the target

	bool GiveNewTargetTile(const iPoint& new_target_tile);
	void ChangeOccupiedTile(const iPoint& new_occupied_tile);
	
	void HandleMovement(float dt);
	void SetEntityState();
	void Move(float dt);

	void HandleFx();

	void DataMapSafetyCheck();													// Method that makes sure that when a unit is idle the walkability and entity maps are correctly updated. (Dirty Fix)

public:
	bool							path_full;
	bool							is_selectable;

	float							speed;
	float							speed_x_factor;								// Factor applied to the speed so the speed is constant regardless of horizontal-vertical-diagonal movement.
	float							speed_y_factor;								// Factor applied to the speed so the speed is constant regardless of horizontal-vertical-diagonal movement.

	int								attack_range;								// Attack range of a given unit. In Tiles.
	float							attack_speed;								// Attacks per second that a given unit will perform.

	float							accumulated_cooldown;						// Total amount of time that has been accumulated since the unit's attack entered cooldown.
	bool							attack_in_cooldown;							// Will keep track whether or not the attack of a unit is in cooldown or not.


	Entity*							target;

	SDL_Rect						entity_sprite_section;
	SDL_Rect						pathing_up_section;
	SDL_Rect						pathing_down_section;
	SDL_Rect						pathing_rigth_section;
	SDL_Rect						pathing_left_section;
	SDL_Rect						pathing_up_right_section;
	SDL_Rect						pathing_up_left_section;
	SDL_Rect						pathing_down_right_section;
	SDL_Rect						pathing_down_left_section;

	iPoint							next_tile;
	iPoint							next_tile_position;
	iPoint							target_tile;
	iPoint							occupied_tile;

	ENTITY_STATE					unit_state;
	PATHFINDING_STATE				path_state;

	std::vector<iPoint>				entity_path;
	std::vector<iPoint>::iterator	current_path_tile;

private:

	uint channel;

	bool fx_playing;
};

#endif // !__DYNAMIC_OBJECT_H__
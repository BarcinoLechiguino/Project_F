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

class Dynamic_Object : public Entity
{
public:

	Dynamic_Object(int x, int y, ENTITY_TYPE type);

	virtual bool Awake(pugi::xml_node&);

	virtual bool Start();

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

	void GiveNewTarget(iPoint new_target);
	void ChangeOccupiedTile(iPoint new_occupied_tile);
	void HandleMovement(float dt);
	void Move(float dt);
	
public:

	float							speed;

	bool							path_full;
	bool							is_selectable;

	ENTITY_STATE					unit_state;
	PATHFINDING_STATE				path_state;

	std::vector<iPoint>				entity_path;
	std::vector<iPoint>::iterator	current_path_tile;

	SDL_Rect						entity_sprite_section;

	iPoint							next_tile;
	iPoint							next_tile_position;
	iPoint							target_tile;
	iPoint							occupied_tile;
};

#endif // !__DYNAMIC_OBJECT_H__
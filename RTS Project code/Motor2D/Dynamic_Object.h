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

public:
	virtual void InitUnitSpriteSections();
	virtual void UpdateUnitSpriteSection();

	void GiveNewTarget(iPoint new_target);
	void ChangeOccupiedTile(iPoint new_occupied_tile);
	void HandleMovement(float dt);
	void Move(float dt);
	
public:
	int								dynamic_object_index;
	
	float							speed;

	bool							path_full;
	bool							is_selectable;

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
};

#endif // !__DYNAMIC_OBJECT_H__
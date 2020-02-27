#ifndef __j1ENTITY_H__
#define __j1ENTITY_H__

#include "j1Module.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "SDL/include/SDL_rect.h"
#include "j1Timer.h"


class Animation;

struct Collider;
struct SDL_Texture;	
struct SDL_Rect;

enum class ENTITY_TYPE
{
	UNKNOWN_TYPE,
	PLAYER,
	PLAYER2,
	MECHA,
	ALIEN,
	COIN
};

struct EntityData
{
	ENTITY_TYPE		type;
	iPoint			position;
};

class j1Entity
{
public:
	j1Entity(int x, int y, ENTITY_TYPE type);	// here we will set initial position

	//Must be virtual so when an entity is executed the compiler goes to the entity's methods instead of these
	virtual bool Awake(pugi::xml_node&);

	virtual bool Start();

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

public:
	//Entity Methods
	virtual bool Save(pugi::xml_node&) const;
	virtual bool Load(pugi::xml_node&);
	virtual void Restart();								//Maybe not needed THIS HERE REVISE, CHANGE
	
	virtual void BlitEntity(int x, int y, SDL_Rect entity_rect, bool flip);
	virtual void OnCollision(Collider* c1, Collider* c2);						//If {} are used then the OnCollision on the entity.cpp needs to be erased.
	virtual void LoadAnimationPushbacks();										//Loads an entity's specific animations.
	virtual void LoadEntityProperties();										//Loads an entity's specific properties.
	virtual void LoadEntityAudio();												//Loads an entity's specific audios.

	//Entity Variables
	ENTITY_TYPE		type;					//Type of the entity (ENTITY_TYPE::PLAYER...)
	p2SString		nameTag;				//Name of the entity in the xml file.
	fPoint			position;				//Initial position of the entity.
	fPoint			speed;					//Movement speed of the entity.
	uint			sprite_width;			//Width of the sprite. Applied to the entity's collider width.
	uint			sprite_height;			//Height of the sprite. Applied to the entity's collider height.

	SDL_Texture*	entity_sprite;			//Sprite / Spritesheet of the entity.
	Collider*		collider;				//Collider of the entity.
	Animation*		animation;				//Animation of the entity.
};

#endif // __j1ENTITY_H__
#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Module.h"
#include "Point.h"
#include "SDL/include/SDL_rect.h"
#include "Timer.h"


class Animation;

struct Collider;
struct SDL_Texture;	
struct SDL_Rect;

enum class ENTITY_TYPE
{
	UNKNOWN_TYPE,
	ROCK,
	ENEMY,
	GATHERER,
	TOWNHALL,
	BARRACKS
};

struct EntityData
{
	ENTITY_TYPE		type;
	iPoint			position;
};

class Entity
{
public:
	Entity(int x, int y, ENTITY_TYPE type);	// here we will set initial position

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

	//Entity Variables
	ENTITY_TYPE		type;					//Type of the entity (ENTITY_TYPE::PLAYER...)
	std::string		nameTag;				//Name of the entity in the xml file.
	fPoint			pixel_position;				//Initial position of the entity.
	iPoint			tile_position;

	SDL_Texture*	entity_sprite;			//Sprite / Spritesheet of the entity.
	Collider*		collider;				//Collider of the entity.
	Animation*		animation;				//Animation of the entity.
};

#endif // __ENTITY_H__
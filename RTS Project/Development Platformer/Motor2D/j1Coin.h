#ifndef __j1COIN_H__
#define __j1COIN_H__

#include "j1Module.h"
#include "j1Entity.h"
#include "Animation.h"
#include "p2Point.h"

class j1Coin : public j1Entity
{
public:
	j1Coin(int x, int y, ENTITY_TYPE type);

	~j1Coin();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool Update(float dt, bool doLogic);

	bool PostUpdate();

	bool CleanUp();

public:
	void OnCollision(Collider* C1, Collider* C2);
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void LoadAnimationPushbacks();
	void LoadEntityProperties();
	void LoadEntityAudio();
	void InitItem();									//Temporally here as there are no other items. Maybe create j1Item parent class.

public:
	SDL_Rect	coin_HitBox;
	int			points;

public:
	pugi::xml_document	config_file;					//Temporally here as there are no other items. Maybe create j1Item parent class or maybe declare it in j1Entity.
	pugi::xml_node		item_entity;					//Temporally here as there are no other items. Maybe create j1Item parent class or maybe declare it in j1Entity.

	Animation	idle;
};

#endif	// !__j1COIN_H__
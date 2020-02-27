#ifndef __j1Enemy_H__
#define __j1Enemy_H__

#include "j1Module.h"
#include "j1Entity.h"
#include "Animation.h"
#include "p2Point.h"

class j1Player;

enum class Entity_State
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

class j1Enemy : public j1Entity
{
public:
	j1Enemy(int x, int y, ENTITY_TYPE type);

	~j1Enemy();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt,  bool doLogic);

	bool PostUpdate();

	bool CleanUp();

	void OnCollision(Collider* C1, Collider* C2);

public:
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void LoadAnimationPushbacks();							//Loads a set of animation pushbacks.
	void LoadEntityProperties();							//Loads an entity's specific properties.
	void LoadEntityAudio();									//Loads an entity's specific audios/sfx.

	void InitEnemy();										//Initializes an enemy's specific data members.

	void EnemyDebugInputs();										//Debug inputs for moving all entities around at the same time.
	virtual void PathfindingLogic();								//Depending on how far a player is from an enemy entity, a path will be created and the enemy will path towards that player.
	virtual void PathfindingMovement(Entity_State state, float dt);	//Holds all the states a specific enemy entity can be in and the specific movement for each state.
	virtual void SetEnemyState(iPoint enemyPos, iPoint playerPos);	//Depending on where a player in pathfinding range is, the enemy will be set in a state or another (Entity_State::PATHING_RIGHT).
	int DistanceFromPlayer(j1Player* player) const;					//Calculates the distance between a player and an enemy entity.

public:
	Entity_State	state;									//State in which the entity is in any given moment.
	SDL_Rect		enemy_HitBox;							//Rectangle that will represent the enemy in the world. Used to create colliders, 
	int				detectionRadius;						//Threshold that the enemy will have to detect whether or not a player is inside its detection range. Distance in tiles, not pixels.
	int				pointsOnKill;							//Amount of points awarded to the players on enemy kill.

	bool grounded;											//
	bool airborne;											//
	bool flip;												//
	bool isGoingRight;										//
	bool isGoingLeft;										//
	bool fading;											//
	bool isAlive;											//
	bool isDying;											//
	bool againstRightWall;									//
	bool againstLeftWall;									//
	
	bool hasTarget;											//Keeps track whether an enemy entity already has a target or not.

	const p2DynArray<iPoint>* entity_path;					//Will store the path created between the enemy entity and a player.

public:
	pugi::xml_document	config_file;	//
	pugi::xml_node		enemy_entity;	//
	
	Animation	idle;					//Idle animation.
	Animation	running;				//Running animation.
};

#endif // __j1ENEMY_H__
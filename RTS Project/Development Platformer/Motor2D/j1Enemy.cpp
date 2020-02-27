#include "j1App.h"
#include "j1Module.h"
#include "p2Point.h"
#include "j1Render.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1FadeScene.h"
#include "j1Audio.h"
#include "j1Enemy.h"
#include "j1Player.h"
#include "j1EntityManager.h"

j1Enemy::j1Enemy(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, type)  //Constructor. Called at the first frame.
{

};

j1Enemy::~j1Enemy()  //Destructor. Called at the last frame.
{

};

bool j1Enemy::Awake(pugi::xml_node& config)
{
	return true;
};

bool j1Enemy::Start()
{
	return true;
};

bool j1Enemy::PreUpdate()
{
	return true;
};

bool j1Enemy::Update(float dt, bool doLogic)
{
	return true;
};

bool j1Enemy::PostUpdate()
{
	return true;
};

bool j1Enemy::CleanUp()
{
	return true;
};

// Collision Handling ---------------------------------------
void j1Enemy::OnCollision(Collider* C1, Collider* C2)
{
	return;
}

bool j1Enemy::Load(pugi::xml_node& data)
{
	return true;
}

bool j1Enemy::Save(pugi::xml_node&  data) const
{
	return true;
}

//---------------------------- Enemy methods ----------------------------
void j1Enemy::LoadAnimationPushbacks()
{
	return;
}

void j1Enemy::LoadEntityProperties()
{
	return;
}

void j1Enemy::LoadEntityAudio()
{
	return;
}

void j1Enemy::InitEnemy()
{
	enemy_HitBox.x = position.x;
	enemy_HitBox.y = position.y;
	enemy_HitBox.w = sprite_width;
	enemy_HitBox.h = sprite_height;

	collider = App->collisions->AddCollider(enemy_HitBox, Object_Type::ENEMY, App->entityManager);				//THIS HERE This one loads the enemy collider.

	// ------------------- ENEMY STATUS BOOLS -------------------
	isAlive				= true;
	grounded			= false;
	airborne			= false;
	flip				= false;
	isGoingRight		= false; 
	isGoingLeft			= false;
	fading				= false;
	isDying				= false;
	againstRightWall	= false;
	againstLeftWall		= false;

	hasTarget			= false;
}

void j1Enemy::EnemyDebugInputs()
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		state = Entity_State::PATHING_UP;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		state = Entity_State::PATHING_DOWN;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		state = Entity_State::PATHING_RIGHT;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		state = Entity_State::PATHING_LEFT;
	}
}

void j1Enemy::PathfindingLogic()
{
	return;
}

void j1Enemy::PathfindingMovement(Entity_State state, float dt)
{
	return;
}

void j1Enemy::SetEnemyState(iPoint enemyPos, iPoint playerPos)
{
	return;
}

int j1Enemy::DistanceFromPlayer(j1Player* player) const
{
	iPoint enemyPos(App->map->WorldToMap(position.x, position.y));					//Fills a p2Point<float> with the position coordinates of an enemy entity in tiles.
	iPoint playerPos(App->map->WorldToMap(player->position.x, player->position.y));	//Fills a p2Point<float> with the position coordinates of a player in tiles.

	//Used DistanceNoSqrt() because square roots have a really high computation cost over just multiplying. Calculus: (p1.x*p2.x) + (p1.y*p2.y).
	return enemyPos.DistanceNoSqrt(playerPos);										//Calculates and returns the distance between the enemyPos point (as the origin) and the playerPos point in tiles.
}
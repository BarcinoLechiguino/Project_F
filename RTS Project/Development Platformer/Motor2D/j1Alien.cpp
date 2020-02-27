#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collisions.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Audio.h"
#include "j1EntityManager.h"
#include "j1Textures.h"
#include "j1Player.h"
#include "j1Alien.h"
#include "j1PathFinding.h"
#include "Brofiler\Brofiler.h"
#include <time.h>

j1Alien::j1Alien(int x, int y, ENTITY_TYPE type) : j1Enemy(x, y, ENTITY_TYPE::ALIEN)
{
	position.x = x;
	position.y = y;
	
	LoadAnimationPushbacks();
}

j1Alien::~j1Alien()
{
}

bool j1Alien::Awake(pugi::xml_node& config)
{
	return true;
}

bool j1Alien::Start()
{
	entity_sprite = App->tex->Load("textures/Spritesheets/Enemies/alien-flying-enemy/alien-enemy-flying.png");
	
	//Load_Entity();
	LoadEntityProperties();
	InitEnemy();

	airborne = true;

	state = Entity_State::IDLE;

	return true;
}

bool j1Alien::Update(float dt, bool doLogic)
{
	BROFILER_CATEGORY("Alien Update", Profiler::Color::Pink);

	state = Entity_State::IDLE;
	
	if (isDying == false)
	{
		//ALIEN DEBUG INPUTS
		if (App->map->pathfindingMetaDebug == true)
		{
			EnemyDebugInputs();
		}

		if (doLogic == true || hasTarget == true)
		{
			if ((App->entityManager->player->player.GodMode == false || App->entityManager->player2->player.GodMode == false) && !App->pause)
			{
				PathfindingLogic();
			}
		}

		PathfindingMovement(state, dt);

		enemy_HitBox = animation->GetCurrentFrame(dt);				//Sets the animation cycle that the alien enemies will have. 
		collider->Set_Position(position.x, position.y);				//Resets the position of the colliders the alien enemies
		BlitEntity(position.x, position.y, enemy_HitBox, flip);		//Blits the alien enemies on screen.
	}

	return true;
}

bool j1Alien::PostUpdate()
{
	return true;
}

bool j1Alien::CleanUp()
{
	App->tex->UnLoad(entity_sprite);

	if (collider != nullptr)
	{
		collider->to_delete = true;
	}

	if (animation != nullptr)
	{
		animation = nullptr;
	}
	
	return true;
}

void j1Alien::OnCollision(Collider* C1, Collider* C2)
{
	//Entity_On collision function in entity
	if (C1->type == Object_Type::ENEMY)
	{
		//Enemy Colliding Against a Player
		if (C2->type == Object_Type::ATTACK)
		{
			if (App->entityManager->player->player.isAttacking == true || App->entityManager->player2->player.isAttacking == true)
			{
				App->audio->PlayFx(13, 1);

				isDying = true;
				collider->to_delete = true;
				int num = App->entityManager->entities.find(this);
				RELEASE(App->entityManager->entities.At(num)->data);
				App->entityManager->entities.del(App->entityManager->entities.At(num));

				App->entityManager->player->player.score += pointsOnKill;
				App->entityManager->player2->player.score += pointsOnKill;
			}
		}

		//Enemy colliding against solids
		if (C2->type == Object_Type::SOLID)
		{
			//Enemy is colliding from LEFT or RIGHT.
			if (C1->collider.y <= C2->collider.y + C2->collider.h && C1->collider.y + C1->collider.h - 4 >= C2->collider.y)		//The first part checks if C1 is contained in the Y axis of C2.
			{
				//Enemy is colliding from LEFT.
				if (C1->collider.x + C1->collider.w >= C2->collider.x && C1->collider.x <= C2->collider.x)						//This second part checks if C1 is actually colliding from the left side of the collider.
				{
					againstLeftWall = true;
					againstRightWall = false;
					//LOG("ALIEN IS COLLIDING WITH A SOLID FROM THE LEFT");
				}

				//Enemy is colliding from RIGHT.
				if (C1->collider.x <= C2->collider.x + C2->collider.w && C1->collider.x >= C2->collider.x)						// This second part checks if C1 is actually colliding from the right side of the collider.
				{
					againstRightWall = true;
					againstLeftWall = false;
					//LOG("ALIEN IS COLLIDING WITH A SOLID FROM THE RIGHT");
				}
			}
		}
	}
}

bool j1Alien::Load(pugi::xml_node&)
{
	return true;
}

bool j1Alien::Save(pugi::xml_node&) const
{
	return true;
}


// ------------------------ ALIEN ENTITY METHODS ------------------------
void j1Alien::LoadAnimationPushbacks()
{
	idle.LoadAnimation("alien", "idle");
	running.LoadAnimation("alien","running");

	return;
}

void j1Alien::LoadEntityProperties()
{
	config_file.load_file("config.xml");

	enemy_entity = config_file.child("config").child("entities").child("enemies").child("alien");
	
	//Gets all the required player variables from the config xml file
	sprite_width	= enemy_entity.child("sprite_size").attribute("w").as_int();
	sprite_height	= enemy_entity.child("sprite_size").attribute("h").as_int();

	speed.x			= enemy_entity.child("speed").attribute("x").as_float();
	speed.y			= enemy_entity.child("speed").attribute("y").as_float();

	pointsOnKill	= enemy_entity.child("points").attribute("value").as_int();

	detectionRadius = enemy_entity.child("detection_radius").attribute("radius").as_int();

	return;
}

void j1Alien::LoadEntityAudio()
{

}

void j1Alien::PathfindingLogic()
{
	iPoint enemyPos(App->map->WorldToMap(position.x, position.y));																//Enemy's position coordinates in tiles.
	iPoint player1Pos(App->map->WorldToMap(App->entityManager->player->position.x, App->entityManager->player->position.y));	//Player 1 position coordinates in tiles.
	iPoint player2Pos(App->map->WorldToMap(App->entityManager->player2->position.x, App->entityManager->player2->position.y));	//Player 2 position coordinates in tiles.

	enemyPos.y += 1;																											//Sets the origin position to the center of the alien enemy entity.

	if (DistanceFromPlayer(App->entityManager->player) <= detectionRadius && DistanceFromPlayer(App->entityManager->player) <= detectionRadius)
	{
		if (DistanceFromPlayer(App->entityManager->player) < DistanceFromPlayer(App->entityManager->player2))
		{
			App->pathfinding->CreatePath(enemyPos, player1Pos);
		}
		else
		{
			App->pathfinding->CreatePath(enemyPos, player2Pos);
		}

		hasTarget == true;
	}

	
	if (DistanceFromPlayer(App->entityManager->player) <= detectionRadius)
	{
		App->pathfinding->CreatePath(enemyPos, player1Pos);
		hasTarget = true;

		if (hasTarget == false)
		{
			hasTarget = true;
		}

		if (hasTarget == true)
		{
			App->pathfinding->CreatePath(enemyPos, player1Pos);

			entity_path = App->pathfinding->GetLastPath();

			for (int i = 0; i < entity_path->Count(); ++i)																		//While there are still elements in the path.
			{
				if (enemyPos.x != entity_path->At(i)->x || enemyPos.y != entity_path->At(i)->y)									//If the position of the enemy in x is different than the position of the i tile of the path.
				{
					iPoint nextStep(entity_path->At(i)->x, entity_path->At(i)->y);												//Sets an iPoint with the coordinates of the i tile of the path. Done so it can be passed as argument to SetEnemyState().

					SetEnemyState(enemyPos, nextStep);																			//Sets the enemy state according to the arguments passed (two iPoints).
				}
			}
		}
	}

	if (DistanceFromPlayer(App->entityManager->player2) <= detectionRadius)
	{
		App->pathfinding->CreatePath(enemyPos, player1Pos);
		hasTarget = true;

		if (hasTarget == false)
		{
			hasTarget = true;
		}

		if (hasTarget == true)
		{
			App->pathfinding->CreatePath(enemyPos, player1Pos);

			entity_path = App->pathfinding->GetLastPath();

			for (int i = 0; i < entity_path->Count(); ++i)																		//While there are still elements in the path.
			{
				if (enemyPos.x != entity_path->At(i)->x || enemyPos.y != entity_path->At(i)->y)									//If the position of the enemy in x is different than the position of the i tile of the path.
				{
					iPoint nextStep(entity_path->At(i)->x, entity_path->At(i)->y);												//Sets an iPoint with the coordinates of the i tile of the path. Done so it can be passed as argument to SetEnemyState().

					SetEnemyState(enemyPos, nextStep);																			//Sets the enemy state according to the arguments passed (two iPoints).
				}
			}
		}
	}
}

void j1Alien::PathfindingMovement(Entity_State state, float dt)
{
	switch (state)
	{
	case Entity_State::IDLE:

		animation = &idle;

		break;

	case Entity_State::PATHING_UP:

		position.y -= speed.y * dt;
		animation = &running;
		App->audio->PlayFx(18, 0);

		break;

	case Entity_State::PATHING_DOWN:

		position.y += speed.y * dt;
		animation = &running;
		App->audio->PlayFx(18, 0);

		break;

	case Entity_State::PATHING_RIGHT:

		position.x += speed.x * dt;
		flip = true;						//Change the alien's orientation in the spritesheet so this can be false.
		animation = &running;
		App->audio->PlayFx(18, 0);

		break;

	case Entity_State::PATHING_LEFT:

		position.x -= speed.x * dt;
		flip = false;
		animation = &running;
		App->audio->PlayFx(18, 0);

		break;

	case Entity_State::PATHING_UP_RIGHT:

		position.x += speed.x * dt;
		position.y -= speed.y * dt;
		flip = true;
		animation = &running;
		App->audio->PlayFx(18, 0);

		break;

	case Entity_State::PATHING_UP_LEFT:

		position.x -= speed.x * dt;
		position.y -= speed.y * dt;
		flip = false;
		animation = &running;
		App->audio->PlayFx(18, 0);

		break;

	case Entity_State::PATHING_DOWN_RIGHT:

		position.x += speed.x * dt;
		position.y += speed.y * dt;
		flip = true;
		animation = &running;
		App->audio->PlayFx(18, 0);

		break;

	case Entity_State::PATHING_DOWN_LEFT:

		position.x -= speed.x * dt;
		position.y += speed.y * dt;
		flip = false;
		animation = &running;
		App->audio->PlayFx(18, 0);

		break;
	}
}

void j1Alien::SetEnemyState(iPoint enemyPos, iPoint playerPos)
{
	iPoint checkRight(enemyPos.x + 1, enemyPos.y);
	iPoint checkLeft(enemyPos.x - 1, enemyPos.y);
	iPoint checkUp(enemyPos.x, enemyPos.y - 1);
	iPoint checkDown(enemyPos.x, enemyPos.y + 1);
	
	if (playerPos.y < enemyPos.y && playerPos.x == enemyPos.x && App->pathfinding->IsWalkable(checkUp))
	{
		state = Entity_State::PATHING_UP;
	}

	if (playerPos.y > enemyPos.y && playerPos.x == enemyPos.x && App->pathfinding->IsWalkable(checkDown))
	{
		state = Entity_State::PATHING_DOWN;
	}

	if (playerPos.x > enemyPos.x && playerPos.y == enemyPos.y && App->pathfinding->IsWalkable(checkRight))
	{
		state = Entity_State::PATHING_RIGHT;
	}

	if (playerPos.x < enemyPos.x && playerPos.y == enemyPos.y && App->pathfinding->IsWalkable(checkLeft))
	{
		state = Entity_State::PATHING_LEFT;
	}

	if (playerPos.x > enemyPos.x && playerPos.y < enemyPos.y)
	{
		if (App->pathfinding->IsWalkable(checkUp) && App->pathfinding->IsWalkable(checkRight))
		{
			state = Entity_State::PATHING_UP_RIGHT;
		}
		else if (!App->pathfinding->IsWalkable(checkUp) && App->pathfinding->IsWalkable(checkRight))
		{
			state = Entity_State::PATHING_RIGHT;
		}
		else if (App->pathfinding->IsWalkable(checkUp) && !App->pathfinding->IsWalkable(checkRight))
		{
			state = Entity_State::PATHING_UP;
		}
	}

	if (playerPos.x < enemyPos.x && playerPos.y < enemyPos.y)
	{
		if (App->pathfinding->IsWalkable(checkUp) && App->pathfinding->IsWalkable(checkLeft))
		{
			state = Entity_State::PATHING_UP_LEFT;
		}
		else if (!App->pathfinding->IsWalkable(checkUp) && App->pathfinding->IsWalkable(checkLeft))
		{
			state = Entity_State::PATHING_LEFT;
		}
		else if (App->pathfinding->IsWalkable(checkUp) && !App->pathfinding->IsWalkable(checkLeft))
		{
			state = Entity_State::PATHING_UP;
		}
	}

	if (playerPos.x > enemyPos.x && playerPos.y > enemyPos.y)
	{
		if (App->pathfinding->IsWalkable(checkDown) && App->pathfinding->IsWalkable(checkRight))
		{
			state = Entity_State::PATHING_DOWN_RIGHT;
		}
		else if (!App->pathfinding->IsWalkable(checkDown) && App->pathfinding->IsWalkable(checkRight))
		{
			state = Entity_State::PATHING_RIGHT;
		}
		else if (App->pathfinding->IsWalkable(checkDown) && !App->pathfinding->IsWalkable(checkRight))
		{
			state = Entity_State::PATHING_DOWN;
		}
	}

	if (playerPos.x < enemyPos.x && playerPos.y > enemyPos.y)
	{
		if (App->pathfinding->IsWalkable(checkDown) && App->pathfinding->IsWalkable(checkLeft))
		{
			state = Entity_State::PATHING_DOWN_LEFT;
		}
		else if (!App->pathfinding->IsWalkable(checkDown) && App->pathfinding->IsWalkable(checkLeft))
		{
			state = Entity_State::PATHING_LEFT;
		}
		else if (App->pathfinding->IsWalkable(checkDown) && !App->pathfinding->IsWalkable(checkLeft))
		{
			state = Entity_State::PATHING_DOWN;
		}
	}
}
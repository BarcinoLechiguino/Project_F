#include "j1App.h"
#include "j1Player.h"
#include "j1Player2.h"
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
#include "j1Mecha.h"
#include "j1EntityManager.h"
#include "j1Pathfinding.h"
#include "Brofiler\Brofiler.h"

j1Mecha::j1Mecha(int x, int y, ENTITY_TYPE type) : j1Enemy(x, y, ENTITY_TYPE::MECHA)
{	
	position.x = x;
	position.y = y;

	LoadAnimationPushbacks();
}

j1Mecha::~j1Mecha()
{
}

bool j1Mecha::Awake(pugi::xml_node& config)
{
	return true;
}

bool j1Mecha::Start()
{
	entity_sprite = App->tex->Load("textures/Spritesheets/Enemies/mech-unit/mech-unit2.png");
	
	//Load_Entity();
	LoadEntityProperties();
	InitEnemy();

	airborne = true;

	state = Entity_State::IDLE;

	return true;
}

bool j1Mecha::Update(float dt, bool doLogic)
{
	BROFILER_CATEGORY("Mecha Update", Profiler::Color::AliceBlue);

	state = Entity_State::IDLE;
	
	//MECHA DEBUG INPUTS
	if (isDying == false)
	{
		if (App->map->pathfindingMetaDebug == true)
		{
			EnemyDebugInputs();
		}

		if (doLogic == true || hasTarget == true)		//Revise this
		{
			if ((App->entityManager->player->player.GodMode == false || App->entityManager->player2->player.GodMode == false) && !App->pause)
			{
				PathfindingLogic();
			}
		}

		PathfindingMovement(state, dt);

		if (airborne == true)
		{
			ApplyMechaGravity();
		}

		if (isDying == true)
		{
			App->audio->PlayFx(12, 0);	    //Mecha Death SFX
		}

		enemy_HitBox = animation->GetCurrentFrame(dt);						//REVISE THIS HERE.
		
		if (flip == false)
		{
			collider->Set_Position(position.x, position.y);
		}
		else
		{
			collider->Set_Position(position.x - FLIP_MARGIN, position.y);
		}
		
		BlitEntity(position.x, position.y, enemy_HitBox, flip);
	}

	return true;
}

bool j1Mecha::PostUpdate()
{
	return true;
}

bool j1Mecha::CleanUp()
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

void j1Mecha::OnCollision(Collider* C1, Collider* C2)
{
	//Entity_On collision function in entity
	if (C1->type == Object_Type::ENEMY)
	{	
		//Enemy Colliding Against a Player
		if (C2->type == Object_Type::ATTACK)
		{
			if (App->entityManager->player->player.isAttacking == true || App->entityManager->player2->player.isAttacking == true)
			{
				//App->audio->PlayFx(7, 0);	    //Mecha Death SFX

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
			//Enemy Colliding from TOP or BOTTOM.
			if (C1->collider.x + C1->collider.w >= C2->collider.x && C1->collider.x <= C2->collider.x + C2->collider.y)		//The first part checks if C1 is contained in the X axis of C2. 
			{
				//Enemy Colliding from TOP.
				if (C1->collider.y + C1->collider.h >= C2->collider.y && C1->collider.y < C2->collider.y && speed.y != 0)			//This second part checks if C1 is actually colliding vertically down.
				{
					speed.y = 0;
					position.y = C2->collider.y - C1->collider.h + 1;		//THIS HERE
					grounded = true;
					//LOG("MECHA IS COLLIDING WITH A SOLID FROM ABOVE");
				}
			}

			//Enemy is colliding from LEFT or RIGHT.
			if (C1->collider.y <= C2->collider.y + C2->collider.h && C1->collider.y + C1->collider.h - 4 >= C2->collider.y)		//The first part checks if C1 is contained in the Y axis of C2.
			{
				//Enemy is colliding from LEFT.
				if (C1->collider.x + C1->collider.w >= C2->collider.x && C1->collider.x <= C2->collider.x)						//This second part checks if C1 is actually colliding from the left side of the collider.
				{
					againstLeftWall = true;
					againstRightWall = false;
					//LOG("MECHA IS COLLIDING WITH A SOLID FROM THE LEFT");
				}

				//Enemy is colliding from RIGHT.
				if (C1->collider.x <= C2->collider.x + C2->collider.w && C1->collider.x >= C2->collider.x)						// This second part checks if C1 is actually colliding from the right side of the collider.
				{
					againstRightWall = true;
					againstLeftWall = false;
					//LOG("MECHA IS COLLIDING WITH A SOLID FROM THE RIGHT");
				}
			}
		}
	}
}

bool j1Mecha::Load(pugi::xml_node&)
{
	return true;
}

bool j1Mecha::Save(pugi::xml_node&) const
{
	return true;
}


// ------------------------ MECHA ENTITY METHODS ------------------------
void j1Mecha::LoadAnimationPushbacks()
{
	idle.LoadAnimation("mecha", "idle");
	//idle.LoadAnimation("mecha", "running");
	running.LoadAnimation("mecha", "running");

	return;
}

void j1Mecha::LoadEntityProperties()
{
	config_file.load_file("config.xml");

	enemy_entity = config_file.child("config").child("entities").child("enemies").child("mecha");

	//Gets all the required enemy variables from the config xml file
	sprite_width			= enemy_entity.child("sprite_size").attribute("w").as_int();
	sprite_height			= enemy_entity.child("sprite_size").attribute("h").as_int();

	speed.x					= enemy_entity.child("speed").attribute("x").as_float();
	speed.y					= enemy_entity.child("speed").attribute("y").as_float();
	mecha_max_speed.x		= enemy_entity.child("max_speed").attribute("x").as_float();
	mecha_max_speed.y		= enemy_entity.child("max_speed").attribute("y").as_float();

	mecha_acceleration.x	= enemy_entity.child("acceleration").attribute("x").as_float();
	mecha_acceleration.y	= enemy_entity.child("acceleration").attribute("y").as_float();
	mecha_gravity			= enemy_entity.child("gravity").attribute("value").as_float();

	pointsOnKill			= enemy_entity.child("points").attribute("value").as_int();

	detectionRadius			= enemy_entity.child("detection_radius").attribute("radius").as_int();

	return;
}

void j1Mecha::LoadEntityAudio()
{

}

void j1Mecha::PathfindingLogic()
{	
	iPoint enemyPos(App->map->WorldToMap(position.x, position.y));																//Enemy's position coordinates in tiles.
	iPoint player1Pos(App->map->WorldToMap(App->entityManager->player->position.x, App->entityManager->player->position.y));	//P1's position coordinates in tiles.
	iPoint player2Pos(App->map->WorldToMap(App->entityManager->player2->position.x, App->entityManager->player2->position.y));	//P2's position coordinates in tiles.

	if (DistanceFromPlayer(App->entityManager->player) <= detectionRadius && DistanceFromPlayer(App->entityManager->player) <= detectionRadius)	//If both players are inside the detection range
	{
		if (DistanceFromPlayer(App->entityManager->player) < DistanceFromPlayer(App->entityManager->player2))					//If P1 is closer than P2.
		{
			App->pathfinding->CreatePath(enemyPos, player1Pos);																	//Creates a path with P1 as the goal.
		}
		else																													//If P2 is closer than P1.
		{
			App->pathfinding->CreatePath(enemyPos, player2Pos);																	//Creates a path with P2 as the goal.
		}
	}
	
	if (DistanceFromPlayer(App->entityManager->player) <= detectionRadius)														//P1 is inside the detection range.
	{
		if (hasTarget == false)																									//If the enemy does not have a target.
		{	
			hasTarget = true;
		}

		if (hasTarget == true)																									//If the enemy has a target.
		{
			App->pathfinding->CreatePath(enemyPos, player1Pos);																	//Creates a path with the target as the goal.

			entity_path = App->pathfinding->GetLastPath();

			for (int i = 0; i < entity_path->Count(); ++i)																		//While there are still elements in the path.
			{	
				if (enemyPos.x != entity_path->At(i)->x)																		//If the position of the enemy in x is different than the position of the i tile of the path.
				{
					iPoint nextStep(entity_path->At(i)->x, entity_path->At(i)->y);												//Sets an iPoint with the coordinates of the i tile of the path. Done so it can be passed as argument to SetEnemyState().
					
					SetEnemyState(enemyPos, nextStep);																			//Sets the enemy state according to the arguments passed (two iPoints).
				}
			}
		}
	}

	if (DistanceFromPlayer(App->entityManager->player2) <= detectionRadius)														//P2 is inside the detection range.
	{
		if (hasTarget == false)																									//If the enemy does not have a target.
		{
			hasTarget = true;
		}

		if (hasTarget == true)																									//If the enemy has a target.
		{
			App->pathfinding->CreatePath(enemyPos, player2Pos);																	//Creates a path with the target as the goal.

			entity_path = App->pathfinding->GetLastPath();

			for (int i = 0; i < entity_path->Count(); ++i)																		//While there are still elements in the path.
			{
				if (enemyPos.x != entity_path->At(i)->x)																		//If the position of the enemy in x is different than the position of the i tile of the path.
				{
					iPoint nextStep(entity_path->At(i)->x, entity_path->At(i)->y);												//Sets an iPoint with the coordinates of the i tile of the path. Done so it can be passed as argument to SetEnemyState().

					SetEnemyState(enemyPos, nextStep);																			//Sets the enemy state according to the arguments passed (two iPoints).
				}
			}
		}
	}
}

void j1Mecha::PathfindingMovement(Entity_State state, float dt)
{
	switch (state)
	{
	case Entity_State::IDLE:				//STAYING STILL

		animation = &idle;

		break;

	case Entity_State::PATHING_UP:			//GOING UP

		position.y -= speed.y * dt;
		animation = &running;
		App->audio->PlayFx(14, 1);

		break;

	case Entity_State::PATHING_DOWN:		//GOING DOWN

		position.y += speed.y * dt;
		animation = &running;
		App->audio->PlayFx(14, 1);

		break;

	case Entity_State::PATHING_RIGHT:		//GOING TO THE RIGHT

		position.x += speed.x * dt;
		flip = false;
		animation = &running;
		App->audio->PlayFx(14, 1);

		break;

	case Entity_State::PATHING_LEFT:		//GOING TO THE LEFT

		position.x -= speed.x * dt;
		flip = true;
		animation = &running;
		App->audio->PlayFx(14, 1);

		break;
	}
}

void j1Mecha::SetEnemyState(iPoint enemyPos, iPoint playerPos)
{
	iPoint checkRight(enemyPos.x + 1, enemyPos.y + 1);								//Coordinates of one tile to the right and one tile down from the enemy entity coordinates.
	iPoint checkLeft(enemyPos.x - 1, enemyPos.y + 1);								//Coordinates of one tile to the left and one tile down from the enemy entity coordinates.
	
	if (playerPos.x < enemyPos.x && App->pathfinding->IsWalkable(checkLeft))		//Check where the player in range is and if there is any unwalkable tiles to the left.
	{
		state = Entity_State::PATHING_LEFT;
	}
	else if (playerPos.x > enemyPos.x  && App->pathfinding->IsWalkable(checkRight))	//Check where the player in range is and if there is any unwalkable tiles to the left.
	{
		state = Entity_State::PATHING_RIGHT;
	}
}

void j1Mecha::ApplyMechaGravity()
{
	speed.y += mecha_gravity * App->GetDt();

	if (speed.y > mecha_max_speed.y * App->GetDt())
	{
		speed.y = mecha_max_speed.y * App->GetDt();
	}

	position.y += speed.y;
}
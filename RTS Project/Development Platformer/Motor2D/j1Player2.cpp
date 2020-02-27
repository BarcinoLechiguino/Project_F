#include "j1App.h"
#include "j1Player2.h"
#include "j1Player1.h"
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
#include "j1EntityManager.h"
#include "j1Console.h"

j1Player2::j1Player2(int x, int y, ENTITY_TYPE type) : j1Player(x, y, type) //Constructor. Called at the first frame.
{
	LoadAnimationPushbacks();
};

j1Player2::~j1Player2()  //Destructor. Called at the last frame.
{

};

bool j1Player2::Awake(pugi::xml_node& config)
{
	return true;
};

bool j1Player2::Start()
{
	entity_sprite = App->tex->Load("textures/Spritesheets/Character 2/Character_Spritesheet2_Buena.png");

	SetPlayer2Position();									//Sets P1's origin position in the map.
	LoadEntityProperties();									//Loads the player's properties from the xml file. //THIS HERE
	InitPlayer();											//Loads P2 in game.
	LoadEntityAudio();
	
	player.airborne = true;
	player.item_activated = false;

	player.state = Player_State::Idle;

	return true;
};

bool j1Player2::PreUpdate()
{
	SetPlayerState(player.state);

	return true;
};

bool j1Player2::Update(float dt, bool doLogic)
{
	PlayerMovement(player.state, dt);
	
	//If P2 is in the air then this function brings them back down to the floor.
	if (player.airborne == true)
	{
		ApplyGravity();
	}

	if (player.isAttacking == true)
	{
		animation = &attacking;

		if (animation->Finished())
		{
			animation->Reset();
			player.isAttacking = false;
		}
	}

	if (player.isDying == true)
	{
		animation = &death;

		if (animation->Finished())
		{
			animation->Reset();
			player.isDying = false;
			RespawnP2ToP1();
		}
	}

	//--------------------------------------- Tp skill Cooldown ---------------------------------------
	if (player.tpInCd == true)
	{
		SkillCooldown(player.tpInCd, player.tpCdCount, player.tpCdTime);
	}
	
	//THIS HERE
	player.HitBox = animation->GetCurrentFrame(dt);											//Sets the animation cycle that P2 will have. 
	collider->Set_Position(position.x, position.y);											//Makes P2's collider follow P2.
	
	if (player.flip == false)
	{
		player.atkCollider->Set_Position(position.x + sprite_width, position.y);				//Makes P2's attack collider follow P2.
	}
	else
	{
		player.atkCollider->Set_Position(position.x - sprite_width, position.y);				//Makes P2's attack collider follow P2.
	}

	if (player.coins % 10 == 0 && !player.extraLife && player.coins > 0)
	{
		player.lives++;
		player.extraLife = true;
		player.score += 100;
		App->entityManager->player->player.score += 100;

		App->audio->PlayFx(20, 0);

	}

	if (player.coins % 10 != 0)
	{
		player.extraLife = false;
	}

	BlitEntity(position.x, position.y, player.HitBox, player.flip);							//Blits the player on screen with the data we have given the Blit() function.

	return true;
};

bool j1Player2::PostUpdate()
{
	//Resetting the collider related bools after the collision happened in Update() so it can be checked the next update/frame.
	player.againstCeiling = false;
	player.againstRightWall = false;
	player.againstLeftWall = false;

	return true;
};

bool j1Player2::Load(pugi::xml_node& data) {

	position.x = data.child("player2").child("position").attribute("x").as_int();
	position.y = data.child("player2").child("position").attribute("y").as_int();

	return true;
}

// Save game state
bool j1Player2::Save(pugi::xml_node& data) const {

	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}

bool j1Player2::CleanUp()
{
	App->tex->UnLoad(entity_sprite);

	if (collider != nullptr)
	{
		collider->to_delete = true;						//THIS HERE
	}

	if (animation != nullptr)
	{
		animation = nullptr;					//THIS HERE
	}

	return true;
};

//Collision Handling ---------------------------------------
void j1Player2::OnCollision(Collider* C1, Collider* C2)
{
	if (player.GodMode == false)
	{	
		if (C2->type == Object_Type::PLAYER)
		{
			Collider* temp = C1;
			C1 = C2;
			C2 = temp;
		}
		if (C1->type != Object_Type::PLAYER)
		{
			return;
		}

		if (C1->type == Object_Type::PLAYER)
		{	
			//Player Colliding Against Another Player
			if (C2->type == Object_Type::PLAYER /*|| C1->type == Object_Type::PLAYER*/)
			{	
				if (C1->collider.x + C1->collider.w > C2->collider.x || C1->collider.x < C2->collider.x + C2->collider.w) //As the boost can be done even if P1 is static, this allows for more precise jumps... hopefully.
				{
					if (App->entityManager->player->player.state == Player_State::Crouching)
					{	
						if (player.grounded == true)
						{
							speed.y -= player.boost_jump.y;
							player.isBoostJumping = true;
							player.airborne = true;
							player.grounded = false;
							player.platformDrop = false;
							App->audio->PlayFx(8, 0);

							//LOG("P2 IS BOOST JUMPING");
						}
					}
					//LOG("P2 IS COLLIDING WITH P1 FROM THE LEFT");
				}
			}

			//Player colliding against solids
			if (C2->type == Object_Type::SOLID)
			{
				//Player Colliding from TOP or BOTTOM. 
				if (C1->collider.x + C1->collider.w >= C2->collider.x && C1->collider.x <= C2->collider.x + C2->collider.y)		//The first part checks if C1 is contained in the X axis of C2. 
				{
					//Player Colliding from TOP.
					if (C1->collider.y + C1->collider.h >= C2->collider.y && C1->collider.y < C2->collider.y && speed.y != 0)			//This second part checks if C1 is actually colliding vertically down.
					{
						speed.y = 0;
						position.y = C2->collider.y - C1->collider.h + 1;		//THIS HERE
						player.isJumping = false;
						player.isBoostJumping = false;
						player.grounded = true;
						//LOG("P2 IS COLLIDING WITH A SOLID FROM ABOVE");
					}

					//Player Colliding from BOTTOM.
					else if (C1->collider.y < C2->collider.y + C2->collider.h && C1->collider.y + 20 > C2->collider.y + C2->collider.h && C1->collider.y > C2->collider.y)	//This second part checks if C1 is actually colliding vertically down.
					{
						speed.y = 0;
						position.y = C2->collider.y + C2->collider.h + 1;		//THIS HERE

						player.againstCeiling = true;
						//LOG("P2 IS COLLIDING WITH A SOLID FROM BELOW");
					}
				}

				//Player is colliding from LEFT or RIGHT.
				if (C1->collider.y <= C2->collider.y + C2->collider.h && C1->collider.y + C1->collider.h - 4 >= C2->collider.y)		//The first part checks if C1 is contained in the Y axis of C2.
				{
					//Player is colliding from LEFT.
					if (C1->collider.x + C1->collider.w >= C2->collider.x && C1->collider.x <= C2->collider.x)						//This second part checks if C1 is actually colliding from the left side of the collider.
					{
						player.againstLeftWall = true;
						player.againstRightWall = false;
						//LOG("P2 IS COLLIDING WITH A SOLID FROM THE LEFT");
					}

					//Player is colliding from RIGHT.
					if (C1->collider.x <= C2->collider.x + C2->collider.w && C1->collider.x >= C2->collider.x)						// This second part checks if C1 is actually colliding from the right side of the collider.
					{
						player.againstRightWall = true;
						player.againstLeftWall = false;
						//LOG("P2 IS COLLIDING WITH A SOLID FROM THE RIGHT");
					}
				}
			}

			if (C2->type == Object_Type::PLATFORM)
			{
				if (player.platformDrop == false)
				{
					if (C1->collider.x >= C2->collider.x && C1->collider.x + C1->collider.w <= C2->collider.x + C2->collider.w)
					{
						//Player Colliding from Above the Solid, ergo colliding with the ground. This second part checks if C1 is actually colliding vertically down.
						if (C1->collider.y + C1->collider.h >= C2->collider.y && C1->collider.y < C2->collider.y)
						{
							speed.y = 0;
							position.y = C2->collider.y - C1->collider.h + 1;
							player.isJumping = false;
							player.isBoostJumping = false;
							player.grounded = true;
							//LOG("P2 IS COLLIDING WITH A SOLID FROM ABOVE");
						}
					}
				}
			}

			if (C2->type == Object_Type::HAZARD)
			{
				if (player.isDying == false)
				{
					player.state = Player_State::Dying;
				}
			}

			if (C2->type == Object_Type::ENEMY)
			{
				if (player.isDying == false)
				{
					player.state = Player_State::Dying;
				}
				
			}

			//Player Colliding against an Activable Item
			if (C2->type == Object_Type::SWITCH)
			{
				if (!player.item_activated)
				{
					App->audio->PlayFx(9, 0);
				}

				player.item_activated = true;				//Records that P1 (or P2) has activated the item.
				App->entityManager->player->player.item_activated = true;		//Activates P2's boolean as well. THIS HERE Change all player2 by App->entitymanager->player2->player.pos...
			}

			//Player colliding against Deactivable surfaces. 
			if (C2->type == Object_Type::DEACTIVABLE)
			{
				if ((player.item_activated == false || App->entityManager->player->player.item_activated == false) && player.isDying == false)
				{
					player.state = Player_State::Dying;
				}
			}

			if (C2->type == Object_Type::ITEM)
			{
				//player.points += 100;
				//LOG("Player2 points %d", player.points);
			}

			if (C2->type == Object_Type::CHECKPOINT)
			{
				LOG("P2 HAS REACHED A CHECKPOINT");															//Call Safe() method here.
				
				if (player.checkpointReached == false)
				{
					App->SaveGame("save_game.xml");
					App->audio->PlayFx(15, 1);

					player.spawn_position = position;

					player.checkpointReached = true;
				}
			}

			//Player colliding against the Goal
			if (C2->type == Object_Type::GOAL)
			{
				LoadNextMap();													//Loads the next map

				App->audio->PlayFx(13, 0);										//Sound effect of the Goal / Protal.
			}
		}
	}
}

//----------------------------------------------- PLAYER 2 INPUTS -----------------------------------------------
void j1Player2::SetPlayerState(Player_State& player_state)
{
	if (player.GodMode == false && !App->pause && !App->console->ConsoleIsOpen())
	{
		player.state = Player_State::Idle;

		if (SpeedFactor().y > Y_SPEED_LIMIT)								//For some reason the player's speed when colliding against the ground is 1.4f aprox instead of 0.
		{
			player.state = Player_State::Falling;
		}

		if (App->input->GetKey(SDL_SCANCODE_KP_6) == KEY_REPEAT)			//Move Right
		{
			player.state = Player_State::Going_Right;
		}

		if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_REPEAT)			//Move Left
		{
			player.state = Player_State::Going_Left;
		}

		if (App->input->GetKey(SDL_SCANCODE_KP_5) == KEY_REPEAT)			//Crouch
		{
			player.state = Player_State::Crouching;
		}

		if (App->input->GetKey(SDL_SCANCODE_KP_0) == KEY_DOWN)				//Jump
		{
			if (player.grounded == true)
			{
				player.state = Player_State::Jumping;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_KP_5) == KEY_REPEAT)				//Drop from platform
		{
			player.platformDrop = true;
		}
		else
		{
			player.platformDrop = false;
		}

		if (App->input->GetKey(SDL_SCANCODE_KP_8) == KEY_DOWN)
		{
			if (player.isDying == false)
			{
				player_state = Player_State::Attacking;
			}
		}
		
		if (App->input->GetKey(SDL_SCANCODE_KP_7) == KEY_DOWN)				//Teleport
		{
			player.state = Player_State::Teleporting;
		}

		if (App->input->GetKey(SDL_SCANCODE_N) == KEY_REPEAT)				//Suicide Button
		{
			player.state = Player_State::Dying;
		}
	}
	else
	{
		GodModeInput();
	}
}

void j1Player2::PlayerMovement(Player_State player_state, float dt)
{
	switch (player_state)
	{

	case Player_State::Idle:

		animation = &idle;
		player.isCrouching = false;

		break;

	case Player_State::Going_Right:

		if (player.againstLeftWall == false && player.isAttacking == false && player.isDying == false)
		{
			position.x += speed.x * dt;

			player.flip = false;
			player.isGoingRight = true;

			if (SpeedFactor().y > Y_SPEED_LIMIT)
			{
				animation = &falling;
			}
			else
			{
				animation = &running;
			}
		}

		break;

	case Player_State::Going_Left:

		if (player.againstRightWall == false && player.isAttacking == false && player.isDying == false)
		{
			position.x -= speed.x * dt;

			player.flip = true;
			player.isGoingLeft = true;

			if (SpeedFactor().y > Y_SPEED_LIMIT)
			{
				animation = &falling;
			}
			else
			{
				animation = &running;
			}
		}

		break;

	case Player_State::Crouching:

		animation = &crouching;
		player.isCrouching = true;

		break;

	case Player_State::Jumping:

		App->audio->PlayFx(10, 0);
		speed.y = -player.acceleration.y;
		player.isJumping = true;						//Boolean for animations
		player.airborne = true;
		player.grounded = false;

		break;

	case Player_State::Falling:							//When dropping from platforms

		player.airborne = true;
		//p1.grounded = false;							//With this commented, jumping after falling off platforms is allowed.
		animation = &falling;

		break;

	case Player_State::Attacking:

		player.isAttacking = true;
		App->audio->PlayFx(19, 0);

		break;

	case Player_State::Teleporting:

		TeleportP1ToP2();

		break;

	case Player_State::Dying:

		player.lives--;

		App->audio->PlayFx(7, 0);

		LivesCheck(player.lives);

		player.isDying = true;

		break;
	}

	//Sorry for this dirty fix.
	if (position.DistanceNoSqrt(App->entityManager->player->position) <= player.boostThreshold && App->entityManager->player->player.state == Player_State::Crouching) //LAST THING TO USE
	{
		if (player.grounded == true)
		{
			speed.y -= player.boost_jump.y;
			//LOG("boost jump speed is");
			player.isBoostJumping = true;
			player.airborne = true;
			player.grounded = false;
			player.platformDrop = false;
			App->audio->PlayFx(8, 0);
		}
	}
}

void j1Player2::GodModeInput()
{
	if (!App->pause && !App->console->ConsoleIsOpen())
	{
		player.airborne = false;

		if (App->input->GetKey(SDL_SCANCODE_KP_6) == KEY_REPEAT)		//THIS HERE
		{
			position.x += player.godModeSpeed * App->GetDt();
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_REPEAT)
		{
			position.x -= player.godModeSpeed * App->GetDt();
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_8) == KEY_REPEAT)
		{
			position.y -= player.godModeSpeed * App->GetDt();
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_5) == KEY_REPEAT)
		{
			position.y += player.godModeSpeed * App->GetDt();
		}
	}
}

// ---------------------------------------------- PLAYER 2 METHODS ----------------------------------------------
void j1Player2::SetPlayer2Position()
{
	config_file.load_file("config.xml");				//REVISE THIS HERE  Can a pugi object be reused as a copy in another class?

	player_entity = config_file.child("config").child("entities").child("player").child("player_2");

	nameTag = player_entity.attribute("name").as_string();

	if (App->scene->firstMap == true)
	{
		player.mapTag = player_entity.child("factoryMap").attribute("mapName").as_string();
		LoadPlayerPosition(nameTag.GetString(), player.mapTag.GetString());
	}
	if (App->scene->secondMap == true)
	{
		player.mapTag = player_entity.child("forestMap").attribute("mapName").as_string();
		LoadPlayerPosition(nameTag.GetString(), player.mapTag.GetString());
	}
}

void j1Player2::TeleportP1ToP2()
{
	//App->entityManager->player->p1.grounded = false;
	
	if (!player.tpInCd)
	{
		if (player.flip == false)			//The players will be always teleported directly in front of one another. 
		{
			if (player.againstLeftWall == false)
			{
				App->entityManager->player->position.x = position.x + collider->collider.w;
				App->entityManager->player->position.y = this->position.y - 60;
				App->audio->PlayFx(6, 0);
				player.tpInCd = true;
			}
			else
			{
				App->audio->PlayFx(16, 0);     //Sfx indicating that teleport cannot be used.
			}
		}
		else
		{
			if (player.againstRightWall == false)
			{	
				App->entityManager->player->position.x = position.x - collider->collider.w / 2;	//THIS HERE
				App->entityManager->player->position.y = position.y - 60;
				App->audio->PlayFx(6, 0);
				player.tpInCd = true;
			}
			else
			{
				App->audio->PlayFx(17, 0);     //Sfx indicating that teleport cannot be used.
			}
		}
	}
}

void j1Player2::RespawnP2ToP1()			//Method that, on death, will respawn P2 behind P1.
{
	if (player.flip == true)			//The players will be always respawned directly behind of one another. 
	{
		position.x = App->entityManager->player->position.x + App->entityManager->player->collider->collider.w;
		position.y = App->entityManager->player->position.y - 40;
	}
	else
	{
		position.x = App->entityManager->player->position.x + App->entityManager->player->collider->collider.w / 2;
		position.y = App->entityManager->player->position.y - 40;
	}
}

//---------------------------------------------- General Checks ----------------------------------------------
void j1Player2::LivesCheck(int lives)
{
	LOG("Lives at live check %d", lives);
	
	if (lives <= 0)
	{
		player.isAlive = false;
		App->entityManager->player->player.isAlive = false;

		if (player.isAlive == false && App->entityManager->player->player.isAlive == false)
		{
			App->audio->PlayFx(9, 0);					//THIS HERE Death sound
			Restart();
			App->entityManager->player->Restart();
			player.lives = player.max_lives;
		}
	}
}

void j1Player2::Restart()
{
	position = player.spawn_position;		//THIS HERE
	player.isAlive = true;
}
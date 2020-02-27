#include "j1App.h"
#include "j1Player1.h"
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
#include "j1Module.h"
#include "j1EntityManager.h"
#include "j1Console.h"

j1Player1::j1Player1(int x, int y, ENTITY_TYPE type) : j1Player(x, y, type) //THIS HERE //Constructor. Called at the first frame.
{
	LoadAnimationPushbacks();
};

j1Player1::~j1Player1()  //Destructor. Called at the last frame.
{

};

bool j1Player1::Awake(pugi::xml_node& config)
{
	return true;
};

bool j1Player1::Start()
{
	entity_sprite = App->tex->Load("textures/Spritesheets/Character 1/character_spritesheet_I_Buena.png");

	SetPlayer1Position();									//Sets P1's origin position in the map.
	LoadEntityProperties();									//Loads the player's properties from the xml file. //THIS HERE
	InitPlayer();											//Loads P1 in game.
	LoadEntityAudio();
							
	//LoadPlayer1Textures();		//Loads P1's textures in game.

	player.airborne = true;
	player.item_activated = false;

	player.state = Player_State::Idle;

	return true;
};

bool j1Player1::PreUpdate()
{	
	SetPlayerState(player.state);

	return true;
};

bool j1Player1::Update(float dt, bool doLogic)
{
	PlayerMovement(player.state, dt);

	//If P1 is in the air then this function brings them back down to the floor.
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
			RespawnP1ToP2();
		}
	}

	//--------------------------------------- Tp skill Cooldown ---------------------------------------
	if (player.tpInCd == true)
	{
		SkillCooldown(player.tpInCd, player.tpCdCount, player.tpCdTime);
	}

	player.HitBox = animation->GetCurrentFrame(dt);											//Sets the animation cycle that P1 will have. 
	collider->Set_Position(position.x, position.y);											//Makes P1's collider follow P1.
	
	if (player.flip == false)
	{
		player.atkCollider->Set_Position(position.x + sprite_width, position.y);			//Makes P1's attack collider follow P1.
	}
	else
	{
		player.atkCollider->Set_Position(position.x - sprite_width, position.y);
	}
	
	if (player.coins % 10 == 0 && !player.extraLife && player.coins > 0)
	{
		player.lives++;
		player.extraLife = true;
		player.score += 100;
		App->entityManager->player2->player.score += 100;

		//EXTRA LIFE SFX HERE

	}

	if (player.coins % 10 != 0)
	{
		player.extraLife = false;
	}

	BlitEntity(position.x, position.y, player.HitBox, player.flip);							//Blits the player on screen with the data we have given the Blit() function.

	return true;
};

bool j1Player1::PostUpdate()
{
	//Resetting the collider related bools after the collision happened in Update() so it can be checked the next update/frame.
	player.againstCeiling = false;
	player.againstRightWall = false;
	player.againstLeftWall = false;

	return true;
};

bool j1Player1::Load(pugi::xml_node& data) {

	position.x = data.child("player").child("position").attribute("x").as_int();
	position.y = data.child("player").child("position").attribute("y").as_int();

	return true;
}

// Save game state
bool j1Player1::Save(pugi::xml_node& data) const {

	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}

bool j1Player1::CleanUp()
{
	LOG("Unloading Player 1");
	App->tex->UnLoad(entity_sprite);

	//entity_sprite = NULL;

	if (collider != nullptr)
	{
		collider->to_delete = true;
	}
	
	if (animation != nullptr)
	{
		animation = nullptr;
	}

	return true;
};

// --------------------------------------- COLLISION HANDLING ---------------------------------------
void j1Player1::OnCollision(Collider* C1, Collider* C2)
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
			if (C2->type == Object_Type::PLAYER)
			{
				if (C1->collider.x + C1->collider.w > C2->collider.x || C1->collider.x < C2->collider.x + C2->collider.w) //As the boost can be done even if P1 is static, this allows for more precise jumps... hopefully.
				{
					if (App->entityManager->player2->player.state == Player_State::Crouching)
					{
						if (player.grounded == true)
						{
							speed.y -= player.boost_jump.y;
							player.isBoostJumping = true;
							player.airborne = true;
							player.grounded = false;
							player.platformDrop = false;
							App->audio->PlayFx(8, 0);
						}
					}
					//LOG("P1 IS COLLIDING WITH P2 FROM THE LEFT");
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
						//LOG("P1 IS COLLIDING WITH A SOLID FROM ABOVE");
					}

					//Player Colliding from BOTTOM.
					else if (C1->collider.y < C2->collider.y + C2->collider.h && C1->collider.y + 20 > C2->collider.y + C2->collider.h && C1->collider.y > C2->collider.y)	//This second part checks if C1 is actually colliding vertically down.
					{
						speed.y = 0;
						position.y = C2->collider.y + C2->collider.h + 1;		//THIS HERE

						player.againstCeiling = true;
						//LOG("P1 IS COLLIDING WITH A SOLID FROM BELOW");
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
						//LOG("P1 IS COLLIDING WITH A SOLID FROM THE LEFT");
					}

					//Player is colliding from RIGHT.
					if (C1->collider.x <= C2->collider.x + C2->collider.w && C1->collider.x >= C2->collider.x)						// This second part checks if C1 is actually colliding from the right side of the collider.
					{
						player.againstRightWall = true;
						player.againstLeftWall = false;
						//LOG("P1 IS COLLIDING WITH A SOLID FROM THE RIGHT");
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
							//LOG("P1 IS COLLIDING WITH A SOLID FROM ABOVE");
						}
					}
				}
			}

			if (C2->type == Object_Type::HAZARD)
			{
				if (player.isDying == false)
				{
					App->audio->PlayFx(7, 0);
					player.state = Player_State::Dying;
				}
			}

			if (C2->type == Object_Type::ENEMY)		//Change this when using the attack collider? Change it in the enemies.cpp.
			{
				if (player.isDying == false)
				{
					App->audio->PlayFx(7, 0);
					player.state = Player_State::Dying;
				}
			}

			//Player Colliding against an Activable Item
			if (C2->type == Object_Type::SWITCH)
			{
				if (!player.item_activated)
				{
					App->audio->PlayFx(9, 0);									//Item Activation sfx.
				}

				player.item_activated = true;									//Records that P1 (or P2) has activated the item.
				App->entityManager->player2->player.item_activated = true;		//Activates P2's boolean as well. THIS HERE Change all player2 by App->entitymanager->player2->player.pos...
			}

			//Player colliding against Deactivable surfaces. 
			if (C2->type == Object_Type::DEACTIVABLE)
			{
				if ((player.item_activated == false || App->entityManager->player->player.item_activated == false) && player.isDying == false)
				{
					App->audio->PlayFx(7, 0);
					player.state = Player_State::Dying;
				}
			}

			if (C2->type == Object_Type::ITEM)
			{
				//player.points += 100;
				//LOG("Player1 points %d", player.points);
			}

			if (C2->type == Object_Type::CHECKPOINT)
			{
				LOG("P1 HAS REACHED A CHECKPOINT");									//Call Save() method here.

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
				LoadNextMap();														//Loads the next map.

				App->audio->PlayFx(11, 1);											//Sound effect of the Goal / Protal.
			}
		}
	}
}

//----------------------------------------------- PLAYER 1 INPUTS -----------------------------------------------
void j1Player1::SetPlayerState(Player_State& player_state)
{
	if (player.GodMode == false && !App->pause && !App->console->ConsoleIsOpen())
	{
		player_state = Player_State::Idle;

		if (SpeedFactor().y > Y_SPEED_LIMIT)									//For some reason the player's speed when colliding against the ground is not 0.0f.
		{
			player_state = Player_State::Falling;
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)				//Move Right
		{
			player_state = Player_State::Going_Right;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)				//Move Left
		{
			player_state = Player_State::Going_Left;
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)				//Crouch
		{
			player_state = Player_State::Crouching;
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)				//Jump
		{
			if (player.grounded == true)
			{
				player_state = Player_State::Jumping;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)				//Drop from platform
		{
			player.platformDrop = true;
		}
		else
		{
			player.platformDrop = false;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
		{
			if (player.isDying == false)
			{
				player_state = Player_State::Attacking;
			}	
		}

		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)					//Teleport
		{
			player_state = Player_State::Teleporting;
		}

		if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)					//Suicide Button
		{
			player_state = Player_State::Dying;
		}
	}
	else
	{
		GodModeInput();														//GodMode 
	}
}

void j1Player1::PlayerMovement(Player_State player_state, float dt)
{
	switch (player.state)							//REVISE THIS HERE Can this be put in the  j1Player? Maybe it is simmilar to OnCollision and needs to be here.
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
		speed.y = -player.acceleration.y;										//FRAME-MOVEMENT SEPARATION
		player.isJumping = true;					//Boolean for animations
		player.airborne = true;
		player.grounded = false;

		break;

	case Player_State::Falling:		//When dropping from platforms

		player.airborne = true;
		//p1.grounded = false;					//With this commented, jumping after falling off platforms is allowed.
		
		animation = &falling;

		break;

	case Player_State::Attacking:
		
		player.isAttacking = true;
		App->audio->PlayFx(19, 0);

		break;

	case Player_State::Teleporting:

		TeleportP2ToP1();

		break;

	case Player_State::Dying:

		player.lives--;
		
		App->audio->PlayFx(7, 0);
		
		LivesCheck(player.lives);
		
		player.isDying = true;

		break;
	}
}

void j1Player1::GodModeInput()
{
	if (!App->pause && !App->console->ConsoleIsOpen())
	{
		player.airborne = false;

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)		//THIS HERE
		{
			position.x += player.godModeSpeed * App->GetDt();
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			position.x -= player.godModeSpeed * App->GetDt();
		}
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			position.y -= player.godModeSpeed * App->GetDt();
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			position.y += player.godModeSpeed * App->GetDt();
		}
	}
}

// ---------------------------------------------- PLAYER 1 METHODS ----------------------------------------------
void j1Player1::SetPlayer1Position()
{
	config_file.load_file("config.xml");

	player_entity = config_file.child("config").child("entities").child("player").child("player_1");

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

void j1Player1::TeleportP2ToP1()		//Method that teleports P2 directly in front of P1. Takes into account which direction P1 is facing. Can trigger some fun interactions between players :)
{
	if (!player.tpInCd)
	{
		if (player.flip == false)			//The players will be always teleported directly in front of one another. 
		{
			if (player.againstLeftWall == false)
			{
				App->entityManager->player2->position.x = position.x + collider->collider.w;		//THIS HERE
				App->entityManager->player2->position.y = position.y - 60;
				App->audio->PlayFx(6, 0);
				player.tpInCd = true;
			}
			else
			{
				App->audio->PlayFx(16, 0);	//Sfx indicating that teleport cannot be used.
			}
		}
		else
		{
			if (player.againstRightWall == false)
			{
				App->entityManager->player2->position.x = position.x - collider->collider.w / 2;
				App->entityManager->player2->position.y = position.y - 60;
				App->audio->PlayFx(6, 0);
				player.tpInCd = true;
			}
			else
			{
				App->audio->PlayFx(16, 0);   //Sfx indicating that teleport cannot be used.
			}
		}
	}
}

void j1Player1::RespawnP1ToP2()		//Method that, on death, will respawn P1 behind P2.
{
	if (player.flip == true)			//The players will be always respawned directly behind of one another. 
	{
		position.x = App->entityManager->player2->position.x + App->entityManager->player2->collider->collider.w;
		position.y = App->entityManager->player2->position.y - 40;
	}
	else
	{
		position.x = App->entityManager->player2->position.x + App->entityManager->player2->collider->collider.w / 2;
		position.y = App->entityManager->player2->position.y - 40;
	}
}

//---------------------------------------------- GENERAL CHECKS ----------------------------------------------
void j1Player1::LivesCheck(int lives)	//REVISE THIS HERE. Can it be put in the j1Player?
{
	LOG("Lives at live check %d", lives);

	if (lives <= 0)
	{
		player.isAlive = false;
		App->entityManager->player2->player.isAlive = false;

		if (player.isAlive == false && App->entityManager->player2->player.isAlive == false)
		{
			App->audio->PlayFx(7, 0);					//THIS HERE Death sound
			Restart();
			App->entityManager->player2->Restart();	
			player.lives = player.max_lives;
		}
	}
}

void j1Player1::Restart()
{
	position = player.spawn_position;		//THIS HERE
	player.isAlive = true;
}
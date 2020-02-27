#include "j1App.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Collisions.h"
#include "j1Map.h"
#include "j1EntityManager.h"
#include "j1Player.h"
#include "j1Coin.h"

j1Coin::j1Coin(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, type)
{
	position.x = x;
	position.y = y;

	LoadAnimationPushbacks();
}

j1Coin::~j1Coin()
{
}

bool j1Coin::Awake(pugi::xml_node &)
{
	return true;
}

//int j = 0;
bool j1Coin::Start()
{
	entity_sprite = App->tex->Load("textures/Spritesheets/Coins/Coins.png");

	LoadEntityProperties();
	InitItem();
	
	//LOG("Coin Start Calls %d", j);
	//j++;
	
	animation = &idle;
	
	return true;
}

bool j1Coin::Update(float dt, bool doLogic)
{
	coin_HitBox = animation->GetCurrentFrame(dt);

	BlitEntity(position.x, position.y, coin_HitBox, false);
	//BlitEntity(position.x, position.y, static_coin_rect, false);
	
	return true;
}

bool j1Coin::PostUpdate()
{	
	return true;
}

bool j1Coin::CleanUp()
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

void j1Coin::OnCollision(Collider* C1, Collider* C2)
{
	if (C1->type == Object_Type::ITEM)
	{
		if (C2->type == Object_Type::ATTACK)														//Temporal measure as for some reason C2 will never be a Player collider.
		{
			if (App->entityManager->player->player.atkCollider == C2)
			{
				if (!App->entityManager->player->player.flip)
				{
					if (C1->collider.x < C2->collider.x - (C2->collider.w * 0.4f))
					{
						App->audio->PlayFx(15, 0);	    //Coin Acquisition sfx.

						collider->to_delete = true;
						int num = App->entityManager->entities.find(this);
						RELEASE(App->entityManager->entities.At(num)->data);
						App->entityManager->entities.del(App->entityManager->entities.At(num));
	
						App->entityManager->player->player.coins += 1;
						App->entityManager->player2->player.coins += 1;
						App->entityManager->player->player.score += points;
						App->entityManager->player2->player.score += points;
						LOG("Player1 points %d", App->entityManager->player->player.coins);
					}
				}
				else
				{
					if (C1->collider.x + C1->collider.w > C2->collider.x + C2->collider.w + (C2->collider.w * 0.4f))
					{
						App->audio->PlayFx(15, 0);	    //Coin Acquisition sfx.
	
						collider->to_delete = true;
						int num = App->entityManager->entities.find(this);
						RELEASE(App->entityManager->entities.At(num)->data);
						App->entityManager->entities.del(App->entityManager->entities.At(num));

						App->entityManager->player->player.coins += 1;
						App->entityManager->player2->player.coins += 1;
						App->entityManager->player->player.score += points;
						App->entityManager->player2->player.score += points;
					}
				}
			}
	
			if (App->entityManager->player2->player.atkCollider == C2)
			{
				if (!App->entityManager->player2->player.flip)
				{
					if (C1->collider.x < C2->collider.x - (C2->collider.w * 0.4f))
					{
						App->audio->PlayFx(15, 0);	    //Coin Acquisition sfx.
	
						collider->to_delete = true;
						int num = App->entityManager->entities.find(this);
						RELEASE(App->entityManager->entities.At(num)->data);
						App->entityManager->entities.del(App->entityManager->entities.At(num));

						App->entityManager->player->player.coins += 1;
						App->entityManager->player2->player.coins += 1;
						App->entityManager->player->player.score += points;
						App->entityManager->player2->player.score += points;
						LOG("Player2 points %d", App->entityManager->player2->player.coins);
					}
				}
				else
				{
					if (C1->collider.x + C1->collider.w > C2->collider.x + C2->collider.w + (C2->collider.w * 0.4f))
					{
						App->audio->PlayFx(15, 0);	    //Coin Acquisition sfx.
	
						collider->to_delete = true;
						int num = App->entityManager->entities.find(this);
						RELEASE(App->entityManager->entities.At(num)->data);
						App->entityManager->entities.del(App->entityManager->entities.At(num));

						App->entityManager->player->player.coins += 1;
						App->entityManager->player2->player.coins += 1;
						App->entityManager->player->player.score += points;
						App->entityManager->player2->player.score += points;
					}
				}
			}
	}
	}
	}
bool j1Coin::Load(pugi::xml_node&)
{
	return true;
}

bool j1Coin::Save(pugi::xml_node&) const
{
	return true;
}

void j1Coin::LoadAnimationPushbacks()
{
	idle.LoadAnimation("coin", "idle");
}

void j1Coin::LoadEntityProperties()
{
	config_file.load_file("config.xml");

	item_entity = config_file.child("config").child("entities").child("items").child("coin");
	
	// Get all the required coin entity variables from the xml file.
	sprite_width		= item_entity.child("sprite_size").attribute("w").as_int();
	sprite_height		= item_entity.child("sprite_size").attribute("h").as_int();
	
	points				= item_entity.child("points").attribute("value").as_int();
}

void j1Coin::LoadEntityAudio()
{

}

void j1Coin::InitItem()
{
	coin_HitBox.x = position.x;
	coin_HitBox.y = position.y;
	coin_HitBox.w = sprite_width;
	coin_HitBox.h = sprite_height;

	collider = App->collisions->AddCollider(coin_HitBox, Object_Type::ITEM, App->entityManager);

}
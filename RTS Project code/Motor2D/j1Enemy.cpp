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
#include "j1EntityManager.h"

j1Enemy::j1Enemy(int x, int y, ENTITY_TYPE type) : j1Dynamic_Object(x, y, type)  //Constructor. Called at the first frame.
{
	entity_sprite = App->tex->Load("maps/debug_enemy_tile.png");

	pixel_position = App->map->MapToWorld(x, y);

	speed = 1;
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
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		target_tile = iPoint(tile_position.x,tile_position.y + 1);

		unit_state = dynamic_state::WALKING;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
	{
		target_tile = iPoint(tile_position.x + 1, tile_position.y);

		unit_state = dynamic_state::WALKING;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
	{
		target_tile = iPoint(tile_position.x - 1, tile_position.y);

		unit_state = dynamic_state::WALKING;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
	{
		target_tile = iPoint(tile_position.x, tile_position.y - 1);

		unit_state = dynamic_state::WALKING;
	}



	if (unit_state == dynamic_state::WALKING)
	{
		pixel_position = App->map->MapToWorld(target_tile.x, target_tile.y);

		tile_position = target_tile;

		
	}

	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y, nullptr);

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
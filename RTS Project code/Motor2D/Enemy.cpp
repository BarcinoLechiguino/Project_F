#include "App.h"
#include "Module.h"
#include "p2Point.h"
#include "Render.h"
#include "Input.h"
#include "p2Log.h"
#include "Map.h"
#include "Scene.h"
#include "Textures.h"
#include "Collisions.h"
#include "FadeScene.h"
#include "Audio.h"
#include "Enemy.h"
#include "EntityManager.h"
#include "j1Pathfinding.h"

Enemy::Enemy(int x, int y, ENTITY_TYPE type) : Dynamic_Object(x, y, type)  //Constructor. Called at the first frame.
{
	entity_sprite = App->tex->Load("maps/debug_enemy_tile.png");

	pixel_position = App->map->MapToWorld(x, y);

	speed = 1;
};

Enemy::~Enemy()  //Destructor. Called at the last frame.
{

};

bool Enemy::Awake(pugi::xml_node& config)
{
	return true;
};

bool Enemy::Start()
{
	return true;
};

bool Enemy::PreUpdate()
{
	return true;
};

bool Enemy::Update(float dt, bool doLogic)
{
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		if (App->pathfinding->IsWalkable(iPoint(tile_position.x, tile_position.y + 1)))
		{
			target_tile = iPoint(tile_position.x, tile_position.y + 1);

			unit_state = dynamic_state::WALKING;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
	{
		if (App->pathfinding->IsWalkable(iPoint(tile_position.x + 1, tile_position.y)))
		{
			target_tile = iPoint(tile_position.x + 1, tile_position.y);

			unit_state = dynamic_state::WALKING;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
	{
		if (App->pathfinding->IsWalkable(iPoint(tile_position.x - 1, tile_position.y)))
		{
			target_tile = iPoint(tile_position.x - 1, tile_position.y);

			unit_state = dynamic_state::WALKING;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
	{
		if (App->pathfinding->IsWalkable(iPoint(tile_position.x, tile_position.y - 1)))
		{
			target_tile = iPoint(tile_position.x, tile_position.y - 1);

			unit_state = dynamic_state::WALKING;
		}
	}

	if (unit_state == dynamic_state::WALKING)
	{
		pixel_position = App->map->MapToWorld(target_tile.x, target_tile.y);

		tile_position = target_tile;

		unit_state = dynamic_state::IDLE;
	}

	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y, nullptr);

	return true;
};

bool Enemy::PostUpdate()
{
	return true;
};

bool Enemy::CleanUp()
{
	return true;
};

// Collision Handling ---------------------------------------
void Enemy::OnCollision(Collider* C1, Collider* C2)
{
	return;
}

bool Enemy::Load(pugi::xml_node& data)
{
	return true;
}

bool Enemy::Save(pugi::xml_node&  data) const
{
	return true;
}
#include "Application.h"
#include "Module.h"
#include "Point.h"
#include "Render.h"
#include "Input.h"
#include "p2Log.h"
#include "Map.h"
#include "Scene1.h"
#include "Textures.h"
#include "Collisions.h"
#include "Audio.h"
#include "Pathfinding.h"

#include "EntityManager.h"

Enemy::Enemy(int x, int y, ENTITY_TYPE type) : Dynamic_Object(x, y, type)  //Constructor. Called at the first frame.
{
	entity_sprite = App->tex->Load("maps/debug_enemy_tile.png");

	selectable_unit = false;

	speed = 100;
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
	HandleMovement(dt);

	selection_collider.x = pixel_position.x;
	selection_collider.y = pixel_position.y;

	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y, nullptr);

	App->render->DrawQuad(selection_collider, 255, 255, 0, 100);

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
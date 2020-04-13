//#include "Map.h"
//#include "Collisions.h"
#include "Audio.h"

#include "EntityManager.h"
#include "Enemy.h"

Enemy::Enemy(int x, int y, ENTITY_TYPE type) : Dynamic_Object(x, y, type)  //Constructor. Called at the first frame.
{
	entity_sprite = App->entity_manager->GetEnemyTexture();

	is_selectable = false;

	speed = 100;
	damage = 30;

	target = nullptr;
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
	SetTarget();
	
	return true;
};

bool Enemy::Update(float dt, bool doLogic)
{
	HandleMovement(dt);

	selection_collider.x = pixel_position.x;
	selection_collider.y = pixel_position.y;

	
	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
	{
		if (target != nullptr)
		{
			ApplyDamage(target);
		}
	}

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

void Enemy::SetTarget()
{
	if (App->entity_manager->infantries.size() != 0)
	{
		target = (Entity*)*App->entity_manager->infantries.begin();
	}
}
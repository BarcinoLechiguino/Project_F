#include "ProjectileManager.h"
#include "Render.h"
#include "Entity.h"
#include "Log.h"

Projectile::Projectile()
{

}

Projectile::Projectile(fPoint position, float speed,int damage, Entity* target)
{
	this->position = position;
	this->speed = speed;
	this->damage = damage;
	this->target = target;
	is_target_alive = true;
	this->life.Start();
	LOG("My target is %s", target->name_tag.c_str());
}

void Projectile::CleanUp()
{
	target = nullptr;
}

bool Projectile::Update(float dt)
{
	if(is_target_alive)
		target_position = target->center_point;

	fPoint velocity;
	fPoint position_difference(target_position.x - position.x, target_position.y - position.y);

	float h = sqrt( (position_difference.x * position_difference.x) + (position_difference.y * position_difference.y) );

	velocity.x = (position_difference.x / h) * speed;
	velocity.y = (position_difference.y / h) * speed;

	position.x += velocity.x * dt;
	position.y += velocity.y * dt;

	if(position.DistanceTo(target_position) < 10)
	{
		if (is_target_alive)
		{
			target->RecieveDamage(damage);
		}

		App->projectile_manager->DestroyProjectile(this);
		App->projectile_manager->iterator--;
	}

	if (life.Read() > 5000)
	{
		App->projectile_manager->DestroyProjectile(this);
		App->projectile_manager->iterator--;
	}

	return true;
}

void Projectile::Draw()
{
	App->render->DrawQuad(SDL_Rect{(int)position.x , (int)position.y,5,5}, 255, 0, 0, 255);
}

ProjectileManager::ProjectileManager()
{
	name = ("projectile_manager");
}

ProjectileManager::~ProjectileManager()
{

}

bool ProjectileManager::Update(float dt)
{
	std::vector<Projectile*>::iterator projectile = projectiles.begin();

	projectile = projectiles.begin();

	for (iterator = 0 ; iterator < projectiles.size() ; iterator++)
	{
		projectile[iterator]->Update(dt);
	}

	return true;
}

bool ProjectileManager::PostUpdate()
{
	Draw();

	return true;
}

void ProjectileManager::CreateProjectile(fPoint position, float speed,int damage,Entity* target)
{
	projectiles.push_back(new Projectile(position, speed,damage,target));
}

void ProjectileManager::DestroyProjectile(Projectile* projectile)
{
	std::vector<Projectile*>::iterator item = projectiles.begin();

	for (; item != projectiles.end(); ++item)
	{
		if ((*item) == projectile)
		{
			(*item)->CleanUp();
			RELEASE((*item));

			projectiles.erase(item);

			break;
		}
	}
}

void ProjectileManager::ClearTargetProjectiles(Entity* target)
{
	std::vector<Projectile*>::iterator projectile = projectiles.begin();

	for (; projectile != projectiles.end(); projectile++)
	{
		(*projectile)->is_target_alive = false;
	}

}

void ProjectileManager::Draw()
{
	for (std::vector<Projectile*>::iterator projectile = projectiles.begin(); projectile != projectiles.end(); projectile++)
	{
		(*projectile)->Draw();
	}
}
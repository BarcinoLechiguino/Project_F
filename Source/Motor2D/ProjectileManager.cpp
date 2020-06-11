#include "ProjectileManager.h"
#include "Render.h"
#include "Entity.h"

Projectile::Projectile()
{

}

Projectile::Projectile(fPoint position, float speed,int damage, Entity* target)
{
	this->position = position;
	this->speed = speed;
	this->damage = damage;
	this->target = target;
}

bool Projectile::Update(float dt)
{
	//Movement
	//position = target->center_point;

	fPoint velocity;
	fPoint position_difference(target->center_point.x - position.x, target->center_point.y - position.y);

	float h = sqrt( (position_difference.x * position_difference.x) + (position_difference.y * position_difference.y) );

	velocity.x = (position_difference.x / h) * speed;
	velocity.y = (position_difference.y / h) * speed;

	position.x += velocity.x * dt;
	position.y += velocity.y * dt;

	if(position.DistanceTo(target->center_point) < 20)
	{
		target->RecieveDamage(this->damage);

		App->projectile_manager->DestroyProjectile(this);
	}

	return true;
}

void Projectile::Draw()
{
	App->render->DrawQuad(SDL_Rect{(int)position.x , (int)position.y,10,10}, 255, 0, 0, 255);
}

ProjectileManager::ProjectileManager()
{

}

ProjectileManager::~ProjectileManager()
{

}

bool ProjectileManager::Update(float dt)
{
	std::vector<Projectile*>::iterator projectile = projectiles.begin();

	for (int i=0; i < projectiles.size() ; i++)
	{
		projectile[i]->Update(dt);
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
			//(*item)->CleanUp();
			//RELEASE((*item));

			projectiles.erase(item);

			break;
		}
	}
}

void ProjectileManager::ClearTargetProjectiles(Entity* target)
{
	std::vector<Projectile*>::iterator projectile = projectiles.begin();

	for (int i = 0; i < projectiles.size(); i++)
	{
		if (projectile[i]->target == target)
		{
			projectiles.erase(projectile);

			i--;
		}
	}
}

void ProjectileManager::Draw()
{
	for (std::vector<Projectile*>::iterator projectile = projectiles.begin(); projectile != projectiles.end(); projectile++)
	{
		(*projectile)->Draw();
	}
}
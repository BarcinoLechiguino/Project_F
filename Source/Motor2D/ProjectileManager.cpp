#include "ProjectileManager.h"

Projectile::Projectile()
{

}

Projectile::Projectile(fPoint position, float speed)
{
	this->position = position;
	this->speed = speed;
}

ProjectileManager::ProjectileManager()
{

}

ProjectileManager::~ProjectileManager()
{

}


bool ProjectileManager::Update(float dt)
{



	return true;
}

void ProjectileManager::CreateProjectile(fPoint position, fPoint speed)
{
	projectiles.push_back(new Projectile());
}

void ProjectileManager::Draw()
{

}
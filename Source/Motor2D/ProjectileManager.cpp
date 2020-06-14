#include "ProjectileManager.h"
#include "Render.h"
#include "Entity.h"
#include "ParticleManager.h"
#include "Emitter.h"
#include "Log.h"
#include "Textures.h"
#include "EntityManager.h"
#include <math.h>

#define PI 3.1415

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

	bullet_section = new SDL_Rect{};
	if (App->entity_manager->IsAllyEntity(target))
	{
		bullet_section->y = 14;
	}
	else
	{
		bullet_section->y = 0;
	}

	if (this->damage > 50)
	{
		bullet_section->w = 13;
		bullet_section->x = 16;
		bullet_section->h = 12;
	}
	else if (this->damage > 20)
	{
		bullet_section->w = 7;
		bullet_section->x = 7;
		bullet_section->h = 10;
	}
	else
	{
		bullet_section->w = 5;
		bullet_section->x = 0;
		bullet_section->h = 7;
	}

	trail = App->particle_manager->SpawnEmitter(this->position, EMITTER_BULLET);
	//LOG("My target is %s", target->name_tag.c_str());
}

void Projectile::CleanUp()
{
	delete bullet_section;
	App->particle_manager->DeleteEmitter(trail);

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

	trail->UpdatePos(position);

	//Find angle
	double radiant = atan2(position_difference.x, position_difference.y);

	if (radiant < 0)
	{
		radiant = abs(radiant);
	}
	else
	{
		radiant = 2 * PI - radiant;
	}

	angle = radiant * (180/PI);

	//LOG("angle is %f", angle);

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
	//App->render->DrawQuad(SDL_Rect{(int)position.x , (int)position.y,5,5}, 255, 0, 0, 255);
	App->render->Blit(App->projectile_manager->bullet, (int)position.x, (int)position.y, bullet_section,false, 1.0F,1.0F, angle, NULL, NULL);
}

ProjectileManager::ProjectileManager()
{
	name = ("projectile_manager");
}

ProjectileManager::~ProjectileManager()
{

}

bool ProjectileManager::Start()
{
	iterator = 0;
	
	bullet = App->tex->Load("Assets/entities/laser_sprites.png");

	return true;
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

bool ProjectileManager::CleanUp()
{
	std::vector<Projectile*>::iterator projectile = projectiles.begin();

	for (; projectile != projectiles.end(); projectile++)
	{
		(*projectile)->CleanUp();

		delete (*projectile);
	}

	projectiles.clear();

	return true;
}

void ProjectileManager::Draw()
{
	for (std::vector<Projectile*>::iterator projectile = projectiles.begin(); projectile != projectiles.end(); projectile++)
	{
		(*projectile)->Draw();
	}
}

bool ProjectileManager::Load(pugi::xml_node& data)
{
	for (int i = 0; i < projectiles.size(); ++i)
	{
		projectiles[i]->CleanUp();
		RELEASE(projectiles[i]);
	}

	projectiles.clear();

	return true;
}

bool ProjectileManager::Save(pugi::xml_node& data) const
{
	

	return true;
}

void ProjectileManager::CreateProjectile(fPoint position, float speed, int damage, Entity* target)
{
	Projectile* projectile = new Projectile(position, speed, damage, target);
	
	if (projectile != nullptr)
	{
		projectiles.push_back(projectile);
	}

	//return projectile;
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

void ProjectileManager::ClearAllProjectiles()
{
	for (int i = 0; i < projectiles.size(); ++i)
	{
		projectiles[i]->CleanUp();
		RELEASE(projectiles[i]);
	}

	projectiles.clear();
}
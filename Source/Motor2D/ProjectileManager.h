#ifndef __PROJECTILE_MANAGER_H__
#define __PROJECTILE_MANAGER_H__

#include "Module.h"
#include "Point.h"
#include "Timer.h"

//class Color;
class Entity;


struct Projectile
{
	Projectile();
	Projectile(fPoint position, float speed,int damage, Entity* target);

	bool Update(float dt);
	void Draw();
	void CleanUp();

	fPoint position;
	fPoint target_position;
	
	float speed;
	int damage;
	
	bool is_target_alive;
	
	Entity* target;
	Timer life;

	//Color color;
	//int width;
	//int height;
};


class ProjectileManager : public Module
{
public:
	ProjectileManager();
	~ProjectileManager();

	bool Update(float dt);
	bool PostUpdate();

	void Draw();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	void CreateProjectile(fPoint position, float speed,int damage, Entity* target);
	void DestroyProjectile(Projectile* projectile);

	void ClearTargetProjectiles(Entity* target);

	void ClearAllProjectiles();

public:
	std::vector<Projectile*> projectiles;

	int iterator;

	SDL_Texture* bullet;
};

#endif // !__PROJECTILE_MANAGER_H__
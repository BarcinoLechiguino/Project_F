#include "Module.h"
#include "Point.h"
#include "Timer.h"

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
};


class ProjectileManager : public Module
{
public:

	ProjectileManager();
	~ProjectileManager();

	bool Update(float dt);
	bool PostUpdate();

	void CreateProjectile(fPoint position, float speed,int damage, Entity* target);
	void DestroyProjectile(Projectile* projectile);

	void ClearTargetProjectiles(Entity* target);

	void Draw();

public:

	std::vector<Projectile*> projectiles;

	int iterator;
};

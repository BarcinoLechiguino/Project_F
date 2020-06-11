#include "Module.h"
#include "Point.h"

class Entity;

struct Projectile
{
	Projectile();
	Projectile(fPoint position, float speed,int damage, Entity* target);

	bool Update(float dt);
	void Draw();

	fPoint position;
	float speed;
	int damage;
	Entity* target;
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
};

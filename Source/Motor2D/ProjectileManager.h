#include "Module.h"
#include "Point.h"

struct Projectile
{
	Projectile();
	Projectile(fPoint position, float speed);

	bool Update(float dt);
	bool Draw();

	fPoint position;
	float speed;
};


class ProjectileManager : public Module
{
public:

	ProjectileManager();
	~ProjectileManager();

	bool Update(float dt);

	void CreateProjectile(fPoint position,fPoint speed);

	void Draw();

public:

	std::vector<Projectile*> projectiles;
};

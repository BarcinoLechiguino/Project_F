#ifndef __ENEMY_WALL_H__
#define __ENEMY_WALL_H__

#include "Building.h"

class EnemyWall : public Building
{
public:
	EnemyWall(int x, int y, ENTITY_TYPE type, int level);
	~EnemyWall();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt, bool do_logic);
	bool PostUpdate();
	bool CleanUp();

	void Draw();

public:
	void InitEntity();

	void AttachHealthbarToEntity();

	void LevelChanges();

public:
	SDL_Rect enemy_wall_rect;
	SDL_Rect enemy_wall_rect_1;
	SDL_Rect enemy_wall_rect_2;

private:


};

#endif // !__WALL_H__
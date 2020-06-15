#ifndef __WALL_H__
#define __WALL_H__

#include "Building.h"

class Wall : public Building
{
public:
	Wall(int x, int y, ENTITY_TYPE type, int level);
	~Wall();

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
	void AttachCreationBarToEntity();

	void LevelChanges();

	void ConstructBuilding();

public:
	SDL_Rect wall_rect;
	SDL_Rect wall_rect_1;
	SDL_Rect wall_rect_2;

private:


};

#endif // !__WALL_H__
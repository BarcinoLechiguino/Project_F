#ifndef __BOULDER_H__
#define __BOULDER_H__

#include "Obstacle.h"

class Boulder : public Obstacle
{
public:
	Boulder(int x, int y, ENTITY_TYPE type, int level);
	~Boulder();

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

private:


};

#endif // !__BOULDER_H__
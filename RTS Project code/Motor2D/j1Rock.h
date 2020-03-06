#include "j1Module.h"
#include "j1EntityManager.h"
#include "j1Static_Object.h"


class j1Rock : public j1Static_Object
{
public:

	j1Rock(int x, int y, ENTITY_TYPE type);

	virtual bool Awake(pugi::xml_node&);

	virtual bool Start();

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();


private:

public:

	int tile_width;
	int tile_height;

private:

};

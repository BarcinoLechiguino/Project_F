
#include "j1Module.h"
#include "j1EntityManager.h"
#include "j1Entity.h"


class j1Static_Object : public j1Entity
{
public:

	j1Static_Object(int x, int y, ENTITY_TYPE type);

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
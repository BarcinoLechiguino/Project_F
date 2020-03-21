
#include "Module.h"
#include "EntityManager.h"
#include "Entity.h"


class Static_Object : public Entity
{
public:

	Static_Object(int x, int y, ENTITY_TYPE type);

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
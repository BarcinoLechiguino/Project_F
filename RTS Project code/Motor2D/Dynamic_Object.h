#include "Module.h"
#include "EntityManager.h"
#include "Entity.h"

enum class dynamic_state
{
	IDLE,
	WALKING
};

class Dynamic_Object : public Entity
{
public:

	Dynamic_Object(int x, int y, ENTITY_TYPE type);

	virtual bool Awake(pugi::xml_node&);

	virtual bool Start();

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool doLogic);

	virtual bool PostUpdate();

	virtual bool CleanUp();


private:

public:

	dynamic_state unit_state;

	int speed;

	iPoint target_tile;

};
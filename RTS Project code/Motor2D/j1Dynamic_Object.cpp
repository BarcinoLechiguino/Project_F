
#include "j1Dynamic_Object.h"



j1Dynamic_Object::j1Dynamic_Object(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, type)
{
	unit_state = dynamic_state::IDLE;
}

bool j1Dynamic_Object::Awake(pugi::xml_node&)
{
	return true;
}

bool j1Dynamic_Object::Start()
{
	return true;
}

bool j1Dynamic_Object::PreUpdate()
{
	return true;
}

bool j1Dynamic_Object::Update(float dt, bool doLogic)
{
	return true;
}

bool j1Dynamic_Object::PostUpdate()
{
	return true;
}

bool j1Dynamic_Object::CleanUp()
{
	return true;
}
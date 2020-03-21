
#include "Dynamic_Object.h"


Dynamic_Object::Dynamic_Object(int x, int y, ENTITY_TYPE type) : Entity(x, y, type)
{
	unit_state = dynamic_state::IDLE;
}

bool Dynamic_Object::Awake(pugi::xml_node&)
{
	return true;
}

bool Dynamic_Object::Start()
{
	return true;
}

bool Dynamic_Object::PreUpdate()
{
	return true;
}

bool Dynamic_Object::Update(float dt, bool doLogic)
{
	return true;
}

bool Dynamic_Object::PostUpdate()
{
	return true;
}

bool Dynamic_Object::CleanUp()
{
	return true;
}
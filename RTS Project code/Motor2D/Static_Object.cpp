#include "EntityManager.h"

Static_Object::Static_Object(int x, int y, ENTITY_TYPE type) : Entity(x, y, type)
{
	
}

bool Static_Object::Awake(pugi::xml_node&)
{
	return true;
}

bool Static_Object::Start()
{
	return true;
}

bool Static_Object::PreUpdate()
{
	

	return true;
}

bool Static_Object::Update(float dt, bool doLogic)
{
	return true;
}

bool Static_Object::PostUpdate()
{
	return true;
}

bool Static_Object::CleanUp()
{
	return true;
}
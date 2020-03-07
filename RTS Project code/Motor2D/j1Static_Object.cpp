#include "j1Static_Object.h"



j1Static_Object::j1Static_Object(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, type)
{

}

bool j1Static_Object::Awake(pugi::xml_node&)
{
	return true;
}

bool j1Static_Object::Start()
{
	return true;
}

bool j1Static_Object::PreUpdate()
{
	return true;
}

bool j1Static_Object::Update(float dt, bool doLogic)
{
	return true;
}

bool j1Static_Object::PostUpdate()
{
	return true;
}

bool j1Static_Object::CleanUp()
{
	return true;
}
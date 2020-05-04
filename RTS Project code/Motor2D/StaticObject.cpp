#include "Application.h"
#include "Pathfinding.h"

#include "StaticObject.h"


StaticObject::StaticObject(int x, int y, ENTITY_TYPE type, int level) : Entity(x, y, type, level)
{
	
}

StaticObject::~StaticObject()
{

}

bool StaticObject::Awake(pugi::xml_node&)
{
	return true;
}

bool StaticObject::Start()
{
	return true;
}

bool StaticObject::PreUpdate()
{
	return true;
}

bool StaticObject::Update(float dt, bool do_logic)
{
	return true;
}

bool StaticObject::PostUpdate()
{
	return true;
}

bool StaticObject::CleanUp()
{
	return true;
}

void StaticObject::Draw()
{
	return;
}
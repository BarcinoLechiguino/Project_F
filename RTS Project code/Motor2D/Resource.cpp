#include "Application.h"
#include "Pathfinding.h"

#include "Resource.h"

Resource::Resource(int x, int y, ENTITY_TYPE type, int level) : StaticObject(x, y, type, level)
{

}

Resource::~Resource()
{

}

bool Resource::Awake(pugi::xml_node&)
{
	return true;
}

bool Resource::Start()
{
	return true;
}

bool Resource::PreUpdate()
{
	return true;
}

bool Resource::Update(float dt, bool do_logic)
{
	return true;
}

bool Resource::PostUpdate()
{
	return true;
}

bool Resource::CleanUp()
{
	return true;
}

void Resource::Draw()
{
	return;
}
#include "Application.h"
#include "Pathfinding.h"

#include "Building.h"

Building::Building(int x, int y, ENTITY_TYPE type, int level) : StaticObject(x, y, type, level)
{

}

Building::~Building()
{

}

bool Building::Awake(pugi::xml_node& config)
{
	return true;
}

bool Building::Start()
{	
	return true;
}

bool Building::PreUpdate()
{
	return true;
}

bool Building::Update(float dt, bool do_logic)
{
	return true;
}

bool Building::PostUpdate()
{
	return true;
}

bool Building::CleanUp()
{
	return true;
}

void Building::Draw()
{
	return;
}
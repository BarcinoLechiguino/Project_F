#include "Application.h"

#include "AllyUnit.h"

AllyUnit::AllyUnit(int x, int y, ENTITY_TYPE type, int level) : DynamicObject(x, y, type, level)
{

}

AllyUnit::~AllyUnit()
{

}

bool AllyUnit::Awake(pugi::xml_node& config)
{
	return true;
}

bool AllyUnit::Start()
{
	return true;
}

bool AllyUnit::PreUpdate()
{
	return true;
}

bool AllyUnit::Update(float dt, bool do_logic)
{
	return true;
}

bool AllyUnit::PostUpdate()
{
	return true;
}

bool AllyUnit::CleanUp()
{
	return true;
}

void AllyUnit::Draw()
{

}
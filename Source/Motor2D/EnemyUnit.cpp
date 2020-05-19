#include "Application.h"

#include "EnemyUnit.h"

EnemyUnit::EnemyUnit(int x, int y, ENTITY_TYPE type, int level) : DynamicObject(x, y, type, level)
{

}

EnemyUnit::~EnemyUnit()
{

}

bool EnemyUnit::Awake(pugi::xml_node& config)
{
	return true;
}

bool EnemyUnit::Start()
{
	return true;
}

bool EnemyUnit::PreUpdate()
{
	return true;
}

bool EnemyUnit::Update(float dt, bool do_logic)
{
	return true;
}

bool EnemyUnit::PostUpdate()
{
	return true;
}

bool EnemyUnit::CleanUp()
{
	return true;
}

void EnemyUnit::Draw()
{

}
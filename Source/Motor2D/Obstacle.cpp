#include "Application.h"

#include "Obstacle.h"

Obstacle::Obstacle(int x, int y, ENTITY_TYPE type, int level) : StaticObject(x, y, type, level)
{

}

Obstacle::~Obstacle()
{

}

bool Obstacle::Awake(pugi::xml_node& config)
{
	return true;
}

bool Obstacle::Start()
{
	return true;
}

bool Obstacle::PreUpdate()
{
	return true;
}

bool Obstacle::Update(float dt, bool do_logic)
{
	return true;
}

bool Obstacle::PostUpdate()
{
	return true;
}

bool Obstacle::CleanUp()
{
	return true;
}

void Obstacle::Draw()
{

}
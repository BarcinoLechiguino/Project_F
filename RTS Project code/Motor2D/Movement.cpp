#include "Pathfinding.h"
#include "EntityManager.h"
#include "Entity.h"

#include "Movement.h"

Movement::Movement()
{

}

Movement::~Movement()
{

}

bool Movement::Awake(pugi::xml_node& config)
{
	return true;
}

bool Movement::Start()
{
	return true;
}

bool Movement::PreUpdate()
{
	return true;
}

bool Movement::Update()
{
	return true;
}

bool Movement::PostUpdate()
{
	return true;
}

bool Movement::CleanUp()
{
	return true;
}
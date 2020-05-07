#include "Application.h"
#include "Point.h"

#include "FowManager.h"

FowManager::FowManager()
{

}

FowManager::~FowManager()
{

}

bool FowManager::Awake(pugi::xml_node& config)
{
	return true;
}

bool FowManager::Start()
{
	return true;
}

bool FowManager::PreUpdate()
{
	return true;
}

bool FowManager::Update(float dt)
{
	return true;
}

bool FowManager::PostUpdate()
{
	return true;
}

bool FowManager::CleanUp()
{
	return true;
}

void FowManager::SetVisibilityMap(uint width, uint height)
{

}

void FowManager::ChangeVisibilityMap(iPoint tile_position, uchar fow_tile_state)
{

}

void FowManager::ResetVisibilityMap()
{

}

// --- FOW_Entity methods ---
FOW_Entity::FOW_Entity()
{

}

FOW_Entity::FOW_Entity(iPoint position, bool provides_visibility) : position(position), provides_visibility(provides_visibility)
{

}

void FOW_Entity::SetPos(iPoint new_position)
{
	position = new_position;
}
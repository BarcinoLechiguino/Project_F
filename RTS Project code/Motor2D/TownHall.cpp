#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Gui.h"
#include "UI.h"
#include "UI_Healthbar.h"
#include "EntityManager.h"

#include "TownHall.h"


TownHall::TownHall(int x, int y, ENTITY_TYPE type, int level) : Static_Object(x, y, type, level)
{
	InitEntity();

	center_point = iPoint(pixel_position.x, pixel_position.y + (((tiles_occupied_x -1) * App->map->data.tile_height/2) + ((tiles_occupied_y - 1) * App->map->data.tile_height / 2)) / 2 );
}

bool TownHall::Awake(pugi::xml_node&)
{
	return true;
}

bool TownHall::PreUpdate()
{

	return true;
}

bool TownHall::Update(float dt, bool doLogic)
{
	return true;
}

bool TownHall::PostUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->DeleteEntity(this);
	}

	return true;
}

bool TownHall::CleanUp()
{
	App->pathfinding->ChangeWalkability(tile_position, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	App->gui->DeleteGuiElement(healthbar);

	return true;
}

void TownHall::Draw()
{
	App->render->Blit(entity_sprite, pixel_position.x - 51, pixel_position.y - 20, &hall_rect);
}

void TownHall::InitEntity()
{
	entity_sprite = App->entity_manager->GetTownHallTexture();

	is_selected = false;

	hall_rect_1 = { 0, 0, 155, 138 };
	hall_rect_2 = { 155, 0, 155, 138 };
	hall_rect = hall_rect_1;

	iPoint world_position = App->map->MapToWorld(tile_position.x, tile_position.y);

	pixel_position.x = world_position.x;
	pixel_position.y = world_position.y;

	tiles_occupied_x = 3;
	tiles_occupied_y = 3;

	unit_level = 1;

	max_health = 900;
	current_health = max_health;

	if (App->entity_manager->CheckTileAvailability(tile_position, this))
	{
		AttachHealthbarToEntity();
	}
}

void TownHall::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -6;
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 618, 1, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };
	healthbar_rect = { 618, 23, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in UI_Healthbar.

	healthbar = (UI_Healthbar*)App->gui->CreateHealthbar(UI_ELEMENT::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}

void TownHall::GenerateUnit(ENTITY_TYPE type, int level)
{
	iPoint pos = App->pathfinding->FindNearbyPoint(iPoint(tile_position.x, tile_position.y + 2));
	switch (type)
	{
	
	case ENTITY_TYPE::GATHERER:
		(Gatherer*)App->entity_manager->CreateEntity(ENTITY_TYPE::GATHERER, pos.x, pos.y, level);
		break;
	/*case ENTITY_TYPE::INFANTRY:
		(Infantry*)App->entity_manager->CreateEntity(ENTITY_TYPE::INFANTRY, pos.x, pos.y, level);
		break;*/
	}
}

void TownHall::LevelChanges()						//Updates the building stats when leveling up
{
	switch (level)
	{
	case 1:
		hall_rect = hall_rect_1;
		break;
	case 2:
		hall_rect = hall_rect_2;
		max_health = 1200;
		current_health = max_health;
		break;
	default:
		hall_rect = hall_rect_2;
		break;
	}
}
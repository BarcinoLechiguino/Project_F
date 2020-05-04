#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Map.h"
#include "Pathfinding.h"
#include "GuiManager.h"
#include "UI.h"
#include "UI_Healthbar.h"
#include "UI_CreationBar.h"
#include "EntityManager.h"
#include "Gatherer.h"

#include "TownHall.h"


TownHall::TownHall(int x, int y, ENTITY_TYPE type, int level) : Static_Object(x, y, type, level)
{
	InitEntity();	
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
	if (creation_queue.size() != 0)
	{
		if (!creating_unit)
		{
			StartUnitCreation();
		}
		else
		{
			creation_bar->UpdateCreationBarValue();

			if (creation_bar->creation_finished)
			{
				GenerateUnit(created_unit_type, unit_level);

				created_unit_type = ENTITY_TYPE::UNKNOWN;

				creation_queue.erase(creation_queue.begin());

				creating_unit = false;
			}
		}
	}
	else
	{
		creation_bar->is_visible = false;
	}
	
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

	App->gui_manager->DeleteGuiElement(healthbar);
	App->gui_manager->DeleteGuiElement(creation_bar);

	return true;
}

void TownHall::Draw()
{
	App->render->Blit(entity_sprite, (int)pixel_position.x - 51, (int)pixel_position.y - 20, &hall_rect);
}

void TownHall::StartUnitCreation()
{
	creation_bar->is_visible = true;

	creating_unit = true;

	created_unit_type = (*creation_queue.begin());

	switch (created_unit_type)															// Used a switch taking into account scalability.
	{
	case ENTITY_TYPE::GATHERER:
		creation_bar->SetNewCreationTime(gatherer_creation_time);
		break;
	}
}

void TownHall::GenerateUnit(ENTITY_TYPE type, int level)
{
	iPoint pos = App->pathfinding->FindNearbyPoint(iPoint(tile_position.x, tile_position.y + 2));

	switch (type)
	{
	case ENTITY_TYPE::GATHERER:
		(Gatherer*)App->entity_manager->CreateEntity(ENTITY_TYPE::GATHERER, pos.x, pos.y, level);
		break;
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

void TownHall::InitEntity()
{
	entity_sprite = App->entity_manager->GetTownHallTexture();

	is_selected = false;

	created_unit_type = ENTITY_TYPE::UNKNOWN;

	// --- SPRITE SECTIONS ---
	hall_rect_1 = { 0, 0, 155, 138 };
	hall_rect_2 = { 155, 0, 155, 138 };
	hall_rect = hall_rect_1;

	// --- CREATION TIMERS ---
	gatherer_creation_time = 1.0f;														//Magic

	// --- POSITION AND SIZE ---
	iPoint world_position = App->map->MapToWorld(tile_position.x, tile_position.y);

	pixel_position.x = (float)world_position.x;
	pixel_position.y = (float)world_position.y;

	tiles_occupied.x = 3;
	tiles_occupied.y = 3;

	// --- STATS & HEALTHBAR ---
	max_health = 900;
	current_health = max_health;

	unit_level = 1;

	if (App->entity_manager->CheckTileAvailability(tile_position, this))
	{
		AttachHealthbarToEntity();
		AttachCreationBarToEntity();
	}

	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height + App->map->data.tile_height * 0.5f);
}

void TownHall::AttachHealthbarToEntity()
{
	// HP Healthbar
	healthbar_position_offset.x = -6;
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 618, 1, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };
	healthbar_rect = { 618, 23, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in UI_Healthbar.

	healthbar = (UI_Healthbar*)App->gui_manager->CreateHealthbar(UI_ELEMENT::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);

}

void TownHall::AttachCreationBarToEntity()
{
	creation_bar_position_offset.x = -6;															// Magic
	creation_bar_position_offset.y = 16;

	creation_bar_background_rect = { 618, 1, MAX_CREATION_BAR_WIDTH, 9 };
	creation_bar_rect = { 618, 23, MAX_CREATION_BAR_WIDTH, 9 };

	int creation_bar_position_x = (int)pixel_position.x + creation_bar_position_offset.x;
	int creation_bar_position_y = (int)pixel_position.y + creation_bar_position_offset.y;

	creation_bar = (UI_CreationBar*)App->gui_manager->CreateCreationBar(UI_ELEMENT::CREATIONBAR, creation_bar_position_x, creation_bar_position_y, false
		, &creation_bar_rect, &creation_bar_background_rect, this);
}
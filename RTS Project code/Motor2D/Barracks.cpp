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
#include "Scout.h"
#include "Infantry.h"
#include "Heavy.h"

#include "Barracks.h"


Barracks::Barracks(int x, int y, ENTITY_TYPE type, int level) : Building(x, y, type, level)
{
	InitEntity();
}

Barracks::~Barracks()
{

}

bool Barracks::Awake(pugi::xml_node&)
{
	return true;
}

bool Barracks::Start()
{
	App->pathfinding->ChangeWalkability(tile_position, this, NON_WALKABLE);
	
	return true;
}

bool Barracks::PreUpdate()
{
	return true;
}

bool Barracks::Update(float dt, bool do_logic)
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

bool Barracks::PostUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->DeleteEntity(this);
	}
	
	return true;
}

bool Barracks::CleanUp()
{
	App->pathfinding->ChangeWalkability(tile_position, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	App->gui_manager->DeleteGuiElement(healthbar);
	App->gui_manager->DeleteGuiElement(creation_bar);
	
	return true;
}

void Barracks::Draw()
{
	App->render->Blit(entity_sprite, (int)pixel_position.x - 27, (int)pixel_position.y - 18, &barracks_rect); //Magic
}

void Barracks::StartUnitCreation()
{
	creation_bar->is_visible = true;
	
	creating_unit = true;
	
	created_unit_type = (*creation_queue.begin());
	
	switch (created_unit_type)
	{
	case ENTITY_TYPE::SCOUT:
		creation_bar->SetNewCreationTime(scout_creation_time);
		break;

	case ENTITY_TYPE::INFANTRY:
		creation_bar->SetNewCreationTime(infantry_creation_time);
		break;

	case ENTITY_TYPE::HEAVY:
		creation_bar->SetNewCreationTime(heavy_creation_time);
		break;
	}
}

void Barracks::GenerateUnit(ENTITY_TYPE type, int level)
{	
	iPoint pos = App->pathfinding->FindNearbyPoint(iPoint(tile_position.x, tile_position.y + 2));

	switch (type)
	{
	case ENTITY_TYPE::SCOUT:
		(Scout*)App->entity_manager->CreateEntity(ENTITY_TYPE::SCOUT, pos.x, pos.y, level);
		break;

	case ENTITY_TYPE::INFANTRY:
		(Infantry*)App->entity_manager->CreateEntity(ENTITY_TYPE::INFANTRY, pos.x, pos.y, level);
		break;

	case ENTITY_TYPE::HEAVY:
		(Heavy*)App->entity_manager->CreateEntity(ENTITY_TYPE::HEAVY, pos.x, pos.y, level);
		break;
	}
}

void Barracks::LevelChanges()				//Updates the building stats when leveling up
{
	switch (level)
	{
	case 1:
		barracks_rect = barracks_rect_1;
		break;
	case 2:
		barracks_rect = barracks_rect_2;
		max_health = 800;
		current_health = max_health;
		//unit_level++;									// TALK ABOUT THIS
		break;
	default:
		barracks_rect = barracks_rect_2;
		break;
	}
}

void Barracks::InitEntity()
{
	entity_sprite = App->entity_manager->GetBarracksTexture();

	is_selected = false;

	created_unit_type = ENTITY_TYPE::UNKNOWN;

	// --- SPRITE SECTIONS ---
	barracks_rect_1 = { 0, 0, 106, 95 };
	barracks_rect_2 = { 108, 0, 106, 95 };
	barracks_rect = barracks_rect_1;

	// --- CREATION TIMERS & VARS ---
	scout_creation_time = 1.0f;
	infantry_creation_time = 2.0f;
	heavy_creation_time = 5.0f;

	// --- POSITION AND SIZE ---
	iPoint world_position = App->map->MapToWorld(tile_position.x, tile_position.y);

	pixel_position.x = (float)world_position.x;
	pixel_position.y = (float)world_position.y;

	tiles_occupied.x = 2;
	tiles_occupied.y = 2;

	// --- STATS & BARS ---
	max_health = 600;
	current_health = max_health;

	unit_level = 1;

	if (App->entity_manager->CheckTileAvailability(tile_position, this))
	{
		AttachHealthbarToEntity();
		AttachCreationBarToEntity();
	}

	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height);
}

void Barracks::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -6;																//Magic
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 618, 1, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };
	healthbar_rect = { 618, 23, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in UI_Healthbar.

	healthbar = (UI_Healthbar*)App->gui_manager->CreateHealthbar(UI_ELEMENT::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}

void Barracks::AttachCreationBarToEntity()
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
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
#include "EntityManager.h"
#include "EnemyInfantry.h"

#include "EnemyTownHall.h"


EnemyTownHall::EnemyTownHall(int x, int y, ENTITY_TYPE type, int level) : Static_Object(x, y, type, level)
{
	InitEntity();
}

bool EnemyTownHall::Awake(pugi::xml_node&)
{
	return true;
}

bool EnemyTownHall::PreUpdate()
{

	return true;
}

bool EnemyTownHall::Update(float dt, bool doLogic)
{
	

	return true;
}

bool EnemyTownHall::PostUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->DeleteEntity(this);
	}
	
	return true;
}

bool EnemyTownHall::CleanUp()
{
	App->pathfinding->ChangeWalkability(tile_position, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	App->gui_manager->DeleteGuiElement(healthbar);

	return true;
}

void EnemyTownHall::Draw()
{
	App->render->Blit(entity_sprite, (int)pixel_position.x - 51, (int)pixel_position.y - 20, &hall_rect);
}

void EnemyTownHall::InitEntity()
{
	entity_sprite = App->entity_manager->GetEnemyTownHallTexture();

	is_selected = false;

	// --- SPRITE SECTIONS ---
	hall_rect_1 = { 0, 0, 155, 138 };
	hall_rect_2 = { 155, 0, 155, 138 };
	hall_rect = hall_rect_1;

	// --- CREATION TIMERS ---
	/*accumulated_creation_time = 0.0f;
	building_creation_time = 5.0f;*/
	
	enemy_gatherer_creation_time = 1.0f;														//Magic

	// --- POSITION AND SIZE ---
	iPoint world_position = App->map->MapToWorld(tile_position.x, tile_position.y);

	pixel_position.x = (float)world_position.x;
	pixel_position.y = (float)world_position.y;

	tiles_occupied.x = 3;
	tiles_occupied.y = 3;

	// --- STATS & HEALTHBAR ---
	max_health = 900;
	current_health = max_health;

	if (App->entity_manager->CheckTileAvailability(tile_position, this))
	{
		AttachHealthbarToEntity();
	}

	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height + App->map->data.tile_height * 0.5f);
}

void EnemyTownHall::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -20;
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 618, 1, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };
	healthbar_rect = { 618, 12, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in UI_Healthbar.

	healthbar = (UI_Healthbar*)App->gui_manager->CreateHealthbar(UI_ELEMENT::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}

void EnemyTownHall::GenerateUnit(ENTITY_TYPE type, int level)
{
	switch (type)
	{
	case ENTITY_TYPE::ENEMY_INFANTRY:
		(EnemyInfantry*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_INFANTRY, tile_position.x, tile_position.y + 2, level);
		break;
	}
}

void EnemyTownHall::LevelChanges()				//Updates the building stats when leveling up
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
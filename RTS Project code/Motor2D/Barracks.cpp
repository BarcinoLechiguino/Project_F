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

#include "Barracks.h"


Barracks::Barracks(int x, int y, ENTITY_TYPE type, int level) : Static_Object(x, y, type, level)
{
	InitEntity();
}

bool Barracks::Awake(pugi::xml_node&)
{
	return true;
}

bool Barracks::PreUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->DeleteEntity(this);
	}

	return true;
}

bool Barracks::Update(float dt, bool doLogic)
{


	return true;
}

bool Barracks::PostUpdate()
{
	return true;
}

bool Barracks::CleanUp()
{
	App->pathfinding->ChangeWalkability(tile_position, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	App->gui->DeleteGuiElement(healthbar);
	
	return true;
}

void Barracks::Draw()
{
	App->render->Blit(entity_sprite, pixel_position.x - 27, pixel_position.y - 18, &barracks_rect);
}

void Barracks::InitEntity()
{
	entity_sprite = App->entity_manager->GetBarracksTexture();

	is_selected = false;

	barracks_rect_1 = { 0, 0, 106, 95 };
	barracks_rect_2 = { 108, 0, 106, 95 };

	barracks_rect = barracks_rect_1;

	iPoint world_position = App->map->MapToWorld(tile_position.x, tile_position.y);

	pixel_position.x = world_position.x;
	pixel_position.y = world_position.y;

	tiles_occupied_x = 2;
	tiles_occupied_y = 2;

	max_health = 600;
	current_health = max_health;

	if (App->entity_manager->CheckTileAvailability(tile_position, this))
	{
		AttachHealthbarToEntity();
	}

	center_point = iPoint(pixel_position.x, pixel_position.y + (((tiles_occupied_x - 1) * App->map->data.tile_height / 2) + ((tiles_occupied_y - 1) * App->map->data.tile_height / 2)) / 2);
}

void Barracks::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -6;
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 618, 1, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };
	healthbar_rect = { 618, 23, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in UI_Healthbar.

	healthbar = (UI_Healthbar*)App->gui->CreateHealthbar(UI_ELEMENT::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}

void Barracks::GenerateUnit(ENTITY_TYPE type, int level)
{
	switch (type)
	{
	case ENTITY_TYPE::ENEMY:
		
		break;
	case ENTITY_TYPE::GATHERER:
		(Gatherer*)App->entity_manager->CreateEntity(ENTITY_TYPE::GATHERER, tile_position.x, tile_position.y + 2, level);
		break;
	case ENTITY_TYPE::INFANTRY:
		(Infantry*)App->entity_manager->CreateEntity(ENTITY_TYPE::INFANTRY, tile_position.x, tile_position.y + 2, level);
		break;
	}
}

void Barracks::LevelChanges()
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
		break;
	default:
		barracks_rect = barracks_rect_2;
		break;
	}
}
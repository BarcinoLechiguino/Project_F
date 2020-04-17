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

#include "EnemyBarracks.h"


EnemyBarracks::EnemyBarracks(int x, int y, ENTITY_TYPE type) : Static_Object(x, y, type)
{
	entity_sprite = App->entity_manager->GetEnemyBarracksTexture();

	barracks_rect_1 = { 0, 0, 106, 95 };
	barracks_rect_2 = { 108, 0, 106, 95 };

	pixel_position.x = App->map->MapToWorld(x, y).x;
	pixel_position.y = App->map->MapToWorld(x, y).y;

	tiles_occupied_x = 2;
	tiles_occupied_y = 2;

	if (App->entity_manager->CheckTileAvailability(iPoint(x, y), this))
	{
		healthbar_position_offset.x = -6;
		healthbar_position_offset.y = -6;

		healthbar_background_rect = { 618, 1, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };
		healthbar_rect = { 618, 12, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };

		int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
		int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in UI_Healthbar.

		healthbar = (UI_Healthbar*)App->gui->CreateHealthbar(UI_ELEMENT::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
	}
}

bool EnemyBarracks::Awake(pugi::xml_node&)
{
	return true;
}

bool EnemyBarracks::PreUpdate()
{

	return true;
}

bool EnemyBarracks::Update(float dt, bool doLogic)
{
	App->render->Blit(entity_sprite, pixel_position.x - 27, pixel_position.y - 18, &barracks_rect_1);

	return true;
}

bool EnemyBarracks::PostUpdate()
{
	return true;
}

bool EnemyBarracks::CleanUp()
{
	App->pathfinding->ChangeWalkability(tile_position, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	App->gui->DeleteGuiElement(healthbar);

	return true;
}

void EnemyBarracks::GenerateUnit(ENTITY_TYPE type)
{
	switch (type)
	{
	case ENTITY_TYPE::ENEMY:

		break;
	case ENTITY_TYPE::GATHERER:
		(Gatherer*)App->entity_manager->CreateEntity(ENTITY_TYPE::GATHERER, tile_position.x, tile_position.y + 2);
		break;
	case ENTITY_TYPE::INFANTRY:
		(Infantry*)App->entity_manager->CreateEntity(ENTITY_TYPE::INFANTRY, tile_position.x, tile_position.y + 2);
		break;
	}
}
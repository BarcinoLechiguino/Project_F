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
	//entity_sprite = App->tex->Load("maps/town_hall_holder.png");
	entity_sprite = App->entity_manager->GetTownHallTexture();
	
	hall_rect = {0, 0, 155, 138};

	pixel_position.x = App->map->MapToWorld(x, y).x;
	pixel_position.y = App->map->MapToWorld(x, y).y;

	tiles_occupied_x = 3;
	tiles_occupied_y = 3;

	if (App->entity_manager->CheckTileAvailability(iPoint(x, y), this))
	{
		healthbar_position_offset.x = -6;
		healthbar_position_offset.y = -6;

		healthbar_background_rect = { 618, 12, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };
		healthbar_rect = { 618, 23, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };

		int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
		int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in UI_Healthbar.

		healthbar = (UI_Healthbar*)App->gui->CreateHealthbar(UI_ELEMENT::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
	}
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
	App->render->Blit(entity_sprite, pixel_position.x - 51, pixel_position.y - 20, &hall_rect);

	return true;
}

bool TownHall::PostUpdate()
{
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

void TownHall::GenerateUnit(ENTITY_TYPE type, int level)
{
	switch (type)
	{
	case ENTITY_TYPE::ENEMY:

		break;
	case ENTITY_TYPE::GATHERER:
		(Gatherer*)App->entity_manager->CreateEntity(ENTITY_TYPE::GATHERER, tile_position.x + 1, tile_position.y + 3, level);
		break;
	case ENTITY_TYPE::INFANTRY:
		(Infantry*)App->entity_manager->CreateEntity(ENTITY_TYPE::INFANTRY, tile_position.x + 1, tile_position.y + 3, level);
		break;
	}
}
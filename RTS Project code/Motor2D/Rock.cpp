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

#include "Rock.h"


Rock::Rock(int x, int y, ENTITY_TYPE type, int level) : Static_Object(x, y, type, level)
{
	InitEntity();

	int rock_version = (rand() % 4) * 54;

	blit_section = new SDL_Rect{rock_version,0,54,35};

}

bool Rock::Awake(pugi::xml_node&)
{
	return true;
}


bool Rock::PreUpdate()
{
	return true;
}

bool Rock::Update(float dt, bool doLogic)
{
	
	App->render->Blit(entity_sprite, pixel_position.x, pixel_position.y, blit_section);

	return true;
}

bool Rock::PostUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->DeleteEntity(this);
		App->audio->PlayFx(App->entity_manager->finished_gather_fx);
	}
	return true;
}

bool Rock::CleanUp()
{
	App->pathfinding->ChangeWalkability(tile_position, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	if (collider != nullptr)
	{
		collider->to_delete = true;
	}
	App->gui->DeleteGuiElement(healthbar);
	
	delete blit_section;

	return true;
}

void Rock::InitEntity()
{
	entity_sprite = App->entity_manager->GetRockTexture();

	is_selected = false;

	iPoint world_position = App->map->MapToWorld(tile_position.x, tile_position.y);

	pixel_position.x = world_position.x;
	pixel_position.y = world_position.y;

	tiles_occupied_x = 1;
	tiles_occupied_y = 1;

	selection_collider = { (int)pixel_position.x + 20, (int)pixel_position.y + 20 , 35, 25 };

	ore = 20;

	gather_time = 1;

	max_health = 300;
	current_health = max_health;

	if (App->entity_manager->CheckTileAvailability(iPoint(tile_position), this))
	{
		healthbar_position_offset.x = -6;
		healthbar_position_offset.y = -6;

		healthbar_background_rect = { 618, 1, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };
		healthbar_rect = { 618, 34, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };

		int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
		int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in UI_Healthbar.

		healthbar = (UI_Healthbar*)App->gui->CreateHealthbar(UI_ELEMENT::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
	}
}
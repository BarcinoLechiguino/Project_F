#include "Brofiler/Brofiler.h"

#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Collisions.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Gui.h"
#include "UI.h"
#include "UI_Healthbar.h"
#include "EntityManager.h"
#include "SceneManager.h"

#include "Gatherer.h"


Gatherer::Gatherer(int x, int y, ENTITY_TYPE type, int level) : Dynamic_Object(x, y, type, level)
{
	target = nullptr;

	entity_sprite = App->entity_manager->GetGathererTexture();

	InitUnitSpriteSections();

	is_selectable = true;
	path_full = false;

	speed = 500.0f;

	if (App->entity_manager->CheckTileAvailability(iPoint(x, y), this))
	{
		healthbar_position_offset.x = -6;
		healthbar_position_offset.y = -6;

		healthbar_background_rect = { 967, 1, MAX_UNIT_HEALTHBAR_WIDTH, 6 };
		healthbar_rect = { 967, 13, MAX_UNIT_HEALTHBAR_WIDTH, 6 };

		int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
		int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in UI_Healthbar.

		healthbar = (UI_Healthbar*)App->gui->CreateHealthbar(UI_ELEMENT::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
	}
}

Gatherer::~Gatherer()
{

}

bool Gatherer::Awake(pugi::xml_node&)
{
	return true;
}

bool Gatherer::Start()
{
	return true;
}

bool Gatherer::PreUpdate()
{
	return true;
}

bool Gatherer::Update(float dt, bool doLogic)
{
	BROFILER_CATEGORY("Gatherer Update", Profiler::Color::Black);
	
	//MoveInput();

	/*if ( path_full )
	{
		target_tile = entity_path.back();
	}*/

	//LOG("target_tile %d %d", target_tile.x, target_tile.y);

	HandleMovement(dt);

	DataMapSafetyCheck();

	UpdateUnitSpriteSection();


	selection_collider.x = pixel_position.x + 10;
	selection_collider.y = pixel_position.y + 10;

	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y - 14, &entity_sprite_section);


	if (App->scene_manager->god_mode)
	{
		App->render->DrawQuad(selection_collider, 255, 255, 0, 100);
	}

	return true;
}

bool Gatherer::PostUpdate()
{
	return true;
}

bool Gatherer::CleanUp()
{
	App->pathfinding->ChangeWalkability(occupied_tile, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	if (collider != nullptr)
	{
		collider->to_delete = true;
	}
	
	App->gui->DeleteGuiElement(healthbar);

	return true;
}

void Gatherer::InitUnitSpriteSections()
{
	entity_sprite_section		= { 52, 0, 52, 49 };
	
	pathing_up_section			= { 0, 49, 39, 42 };
	pathing_down_section		= { 39, 49, 38, 45 };
	pathing_rigth_section		= { 123, 49, 43, 42 };
	pathing_left_section		= { 78, 49, 43, 42 };
	pathing_up_right_section	= { 104, 0, 52, 49 };
	pathing_up_left_section		= { 156, 0, 52, 49 };
	pathing_down_right_section	= { 52, 0, 52 ,49 };
	pathing_down_left_section	= { 0, 0, 52, 49 };
}

void Gatherer::UpdateUnitSpriteSection()
{
	//change section according to pathing. 
	switch (unit_state) 
	{
	case ENTITY_STATE::PATHING_UP:
		entity_sprite_section = pathing_up_section;
		break;
	case ENTITY_STATE::PATHING_DOWN:
		entity_sprite_section = pathing_down_section;
		break;
	case ENTITY_STATE::PATHING_RIGHT:
		entity_sprite_section = pathing_rigth_section;
		break;
	case ENTITY_STATE::PATHING_LEFT:
		entity_sprite_section = pathing_left_section;
		break;					  						  
	case ENTITY_STATE::PATHING_UP_RIGHT:
		entity_sprite_section = pathing_up_right_section;
		break;
	case ENTITY_STATE::PATHING_UP_LEFT:
		entity_sprite_section = pathing_up_left_section;
		break;
	case ENTITY_STATE::PATHING_DOWN_RIGHT:
		entity_sprite_section = pathing_down_right_section;
		break;
	case ENTITY_STATE::PATHING_DOWN_LEFT:
		entity_sprite_section = pathing_down_left_section;
		break;
	}
}

void Gatherer::OnCollision(Collider* C1, Collider* C2)
{

}
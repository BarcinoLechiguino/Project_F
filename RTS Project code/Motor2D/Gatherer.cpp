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


Gatherer::Gatherer(int x, int y, ENTITY_TYPE type) : Dynamic_Object(x, y, type)
{
	target = nullptr;

	entity_sprite = App->entity_manager->GetGathererTexture();

	AssignEntityIndex();

	InitUnitSpriteSections();

	is_selectable = true;
	path_full = false;

	speed = 500.0f;

	healthbar_background_rect = { 618, 12, MAX_UNIT_HEALTHBAR_WIDTH, 9 };
	healthbar_rect = { 618, 23, MAX_UNIT_HEALTHBAR_WIDTH, 9 };

	healthbar = (UI_Healthbar*)App->gui->CreateHealthbar(UI_ELEMENT::HEALTHBAR, (int)pixel_position.x, (int)pixel_position.y - 30, true, &healthbar_rect, &healthbar_background_rect, this);
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
	//MoveInput();

	/*if ( path_full )
	{
		target_tile = entity_path.back();
	}*/

	//LOG("target_tile %d %d", target_tile.x, target_tile.y);

	HandleMovement(dt);

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
	App->pathfinding->ChangeWalkability(occupied_tile, WALKABLE);																	//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);																//The entity is cleared from the entity_map.
	
	std::vector<Entity*>::iterator entity = App->entity_manager->entities.begin() + entity_index;

	App->entity_manager->entities.erase(entity);																					//The entity is erased from the entities vector.

	for (; entity != App->entity_manager->entities.end(); ++entity)
	{
		(*entity)->entity_index--;

	}

	std::vector<Dynamic_Object*>::iterator dynamic_object = App->entity_manager->dynamic_objects.begin() + dynamic_object_index;

	App->entity_manager->dynamic_objects.erase(dynamic_object);																		//The entity is erased from the dynamic_object vector.

	for (; dynamic_object != App->entity_manager->dynamic_objects.end(); ++dynamic_object)
	{
		(*dynamic_object)->dynamic_object_index--;
	}


	std::vector<Gatherer*>::iterator gatherer = App->entity_manager->gatherers.begin() + gatherer_index;

	App->entity_manager->gatherers.erase(gatherer);																					//The entity is erased from the gatherers vector.

	for (; gatherer != App->entity_manager->gatherers.end(); ++gatherer)
	{
		(*gatherer)->gatherer_index--;
	}

	entity_sprite = nullptr;

	if (collider != nullptr)
	{
		collider->to_delete = true;
	}
	
	App->gui->DeleteGuiElement(healthbar);

	return true;
}

void Gatherer::AssignEntityIndex()
{
	entity_index			= App->entity_manager->entities.size();
	dynamic_object_index	= App->entity_manager->dynamic_objects.size();
	gatherer_index			= App->entity_manager->gatherers.size();
}

void Gatherer::InitUnitSpriteSections()
{
	entity_sprite_section = { 52, 0, 52, 49 };
	
	/*pathing_up_section = ;
	pathing_down_section = ;
	pathing_rigth_section = ;
	pathing_left_section = ;
	pathing_up_right_section = ;
	pathing_up_left_section = ;
	pathing_down_right_section = ;
	pathing_down_left_section = ;*/

}

void Gatherer::UpdateUnitSpriteSection()
{
	//change section according to pathing. 
	switch (this->unit_state) {
	case ENTITY_STATE::PATHING_DOWN:
		entity_sprite_section = { 39, 49, 38, 45 };
		break;						  
	case ENTITY_STATE::PATHING_RIGHT: 
		entity_sprite_section = { 123 ,49, 43, 42 };
		break;						  
	case ENTITY_STATE::PATHING_LEFT:  
		entity_sprite_section = { 78, 49, 43, 42 };
		break;
	case ENTITY_STATE::PATHING_UP:
		entity_sprite_section = { 0, 49, 39, 42 };
		break;
	case ENTITY_STATE::PATHING_DOWN_RIGHT:
		entity_sprite_section = { 52, 0, 52 ,49 };
		break;
	case ENTITY_STATE::PATHING_DOWN_LEFT:
		entity_sprite_section = { 0, 0, 52, 49 };
		break;
	case ENTITY_STATE::PATHING_UP_RIGHT:
		entity_sprite_section = { 104, 0, 52, 49 };
		break;
	case ENTITY_STATE::PATHING_UP_LEFT:
		entity_sprite_section = { 156, 0, 52, 49 };
		break;
	}
}

void Gatherer::OnCollision(Collider* C1, Collider* C2)
{

}
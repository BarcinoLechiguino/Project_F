#include "p2Log.h"

#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Collisions.h"
#include "Map.h"
#include "Pathfinding.h"
#include "EntityManager.h"
#include "Gui.h"
#include "UI.h"
#include "UI_Healthbar.h"
#include "SceneManager.h"

#include "Infantry.h"


Infantry::Infantry(int x, int y, ENTITY_TYPE type) : Dynamic_Object(x, y, type)  //Constructor. Called at the first frame.
{
	entity_sprite = App->entity_manager->GetInfantryTexture();
	
	is_selectable = true;

	AssignEntityIndex();

	speed = 500.0f;

	max_health = 300;
	current_health = max_health;
	damage = 30;

	InitUnitSpriteSections();

	healthbar_background_rect = { 618, 12, MAX_UNIT_HEALTHBAR_WIDTH, 9 };
	healthbar_rect = { 618, 23, MAX_UNIT_HEALTHBAR_WIDTH, 9 };

	healthbar = (UI_Healthbar*)App->gui->CreateHealthbar(UI_ELEMENT::HEALTHBAR, (int)pixel_position.x, (int)pixel_position.y - 30, true, &healthbar_rect, &healthbar_background_rect, this); //Magic Number
};

Infantry::~Infantry()  //Destructor. Called at the last frame.
{

};

bool Infantry::Awake(pugi::xml_node& config)
{
	return true;
};

bool Infantry::Start()
{
	return true;
};

bool Infantry::PreUpdate()
{
	return true;
};

bool Infantry::Update(float dt, bool doLogic)
{
	HandleMovement(dt);

	selection_collider.x = pixel_position.x;
	selection_collider.y = pixel_position.y;

	UpdateUnitSpriteSection();

	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y-15, &entity_sprite_section);

	if (App->scene_manager->god_mode)
	{
		App->render->DrawQuad(selection_collider, 255, 255, 0, 100);
	}

	return true;
};

bool Infantry::PostUpdate()
{
	if (current_health <= 0)
	{	
		CleanUp();
	}

	return true;
};

bool Infantry::CleanUp()
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


	std::vector<Infantry*>::iterator infantry = App->entity_manager->infantries.begin() + infantry_index;

	App->entity_manager->infantries.erase(infantry);																					//The entity is erased from the gatherers vector.

	for (; infantry != App->entity_manager->infantries.end(); ++infantry)
	{
		(*infantry)->infantry_index--;
	}

	entity_sprite = nullptr;

	if (collider != nullptr)
	{
		collider->to_delete = true;
	}
	
	App->gui->DeleteGuiElement(healthbar);

	return true;
};

void Infantry::AssignEntityIndex()
{
	entity_index = App->entity_manager->entities.size();
	dynamic_object_index = App->entity_manager->dynamic_objects.size();
	infantry_index = App->entity_manager->infantries.size();
}

void Infantry::InitUnitSpriteSections()
{
	entity_sprite_section = { 58, 0, 58, 47 }; //Down Right
}

void Infantry::UpdateUnitSpriteSection()
{
	//change section according to pathing. 
	switch (this->unit_state)
	{
	case ENTITY_STATE::PATHING_DOWN:
		entity_sprite_section = { 71, 47, 70, 52 };
		break;
	case ENTITY_STATE::PATHING_RIGHT:
		entity_sprite_section = { 202, 47, 59, 52 };
		break;
	case ENTITY_STATE::PATHING_LEFT:
		entity_sprite_section = { 142, 47, 59, 52 };
		break;
	case ENTITY_STATE::PATHING_UP:
		entity_sprite_section = { 0, 47, 70, 52 };
		break;
	case ENTITY_STATE::PATHING_DOWN_RIGHT:
		entity_sprite_section = { 58, 0, 58, 47 };
		break;
	case ENTITY_STATE::PATHING_DOWN_LEFT:
		entity_sprite_section = { 0, 0, 58, 47 };
		break;
	case ENTITY_STATE::PATHING_UP_RIGHT:
		entity_sprite_section = { 116, 0, 60, 47 };
		break;
	case ENTITY_STATE::PATHING_UP_LEFT:
		entity_sprite_section = { 176, 0, 59, 47 };
		break;
	}
}

void Infantry::ApplyDamage(Entity* objective)
{
	return;
}

// Collision Handling ---------------------------------------
void Infantry::OnCollision(Collider* C1, Collider* C2)
{
	return;
}
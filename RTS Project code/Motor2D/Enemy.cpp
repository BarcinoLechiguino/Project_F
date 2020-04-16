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

#include "Enemy.h"


Enemy::Enemy(int x, int y, ENTITY_TYPE type) : Dynamic_Object(x, y, type)  //Constructor. Called at the first frame.
{
	entity_sprite = App->entity_manager->GetEnemyTexture();

	is_selectable = false;

	speed = 500.0f;
	
	max_health = 300;
	current_health = max_health;
	damage = 30;

	InitUnitSpriteSections();

	target = nullptr;

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
};

Enemy::~Enemy()  //Destructor. Called at the last frame.
{

};

bool Enemy::Awake(pugi::xml_node& config)
{
	return true;
};

bool Enemy::Start()
{
	return true;
};

bool Enemy::PreUpdate()
{
	SetTarget();
	
	return true;
};

bool Enemy::Update(float dt, bool doLogic)
{
	HandleMovement(dt);

	DataMapSafetyCheck();

	UpdateUnitSpriteSection();

	selection_collider.x = pixel_position.x;
	selection_collider.y = pixel_position.y;

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
	{
		if (target != nullptr)
		{
			ApplyDamage(target);
		}
	}

	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y, &entity_sprite_section);

	App->render->DrawQuad(selection_collider, 255, 255, 0, 100);

	return true;
};

bool Enemy::PostUpdate()
{
	if (current_health <= 0)
	{
		CleanUp();
	}
	
	return true;
};

bool Enemy::CleanUp()
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
};

void Enemy::InitUnitSpriteSections()
{
	entity_sprite_section		= { 58, 0, 58, 47 };

	pathing_up_section			= { 0, 47, 70, 52 };
	pathing_down_section		= { 71, 47, 70, 52 };
	pathing_rigth_section		= { 202, 47, 59, 52 };
	pathing_left_section		= { 142, 47, 59, 52 };
	pathing_up_right_section	= { 116, 0, 60, 47 };
	pathing_up_left_section		= { 176, 0, 59, 47 };
	pathing_down_right_section	= { 58, 0, 58, 47 };
	pathing_down_left_section	= { 0, 0, 58, 47 };
}

void Enemy::UpdateUnitSpriteSection()
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

void Enemy::SetTarget()
{
	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();
	
	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if ((*item)->type == ENTITY_TYPE::GATHERER || (*item)->type == ENTITY_TYPE::INFANTRY)
		{
			target = (*item);
			break;
		}
	}
}

// Collision Handling ---------------------------------------
void Enemy::OnCollision(Collider* C1, Collider* C2)
{
	return;
}
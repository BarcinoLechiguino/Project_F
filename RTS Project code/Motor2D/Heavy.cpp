#include "p2Log.h"

#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Collisions.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Player.h"
#include "EntityManager.h"
#include "GuiManager.h"
#include "UI.h"
#include "UI_Healthbar.h"
#include "SceneManager.h"

#include "Heavy.h"


Heavy::Heavy(int x, int y, ENTITY_TYPE type, int level) : Dynamic_Object(x, y, type, level)  //Constructor. Called at the first frame.
{
	LOG("x %d and y %d", x, y);
	InitEntity();
};

Heavy::~Heavy()  //Destructor. Called at the last frame.
{

};

bool Heavy::Awake(pugi::xml_node& config)
{
	return true;
};

bool Heavy::Start()
{
	return true;
};

bool Heavy::PreUpdate()
{
	return true;
};

bool Heavy::Update(float dt, bool doLogic)
{
	HandleMovement(dt);

	DataMapSafetyCheck();

	if (path_full)
	{
		UpdateUnitSpriteSection();
	}
	else
	{
		UpdateUnitOrientation();
	}

	selection_collider.x = (int)pixel_position.x;
	selection_collider.y = (int)pixel_position.y;

	if (doLogic)
	{
		if (target == nullptr && !path_full)
		{
			SetEntityTargetByProximity();
		}
	}

	if (target != nullptr)
	{
		if (TargetIsInRange())
		{
			DealDamage();
		}
		else
		{
			if (!path_full)
			{
				ChaseTarget();
			}
		}
	}

	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height / 2);

	return true;
};

bool Heavy::PostUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->DeleteEntity(this);
	}

	return true;
};

bool Heavy::CleanUp()
{
	App->pathfinding->ChangeWalkability(occupied_tile, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	if (collider != nullptr)
	{
		collider->to_delete = true;
	}

	App->gui_manager->DeleteGuiElement(healthbar);

	return true;
};

void Heavy::Draw()
{
	App->render->Blit(this->entity_sprite, (int)pixel_position.x, (int)pixel_position.y - 15, &entity_sprite_section);

	if (App->player->god_mode)
	{
		App->render->DrawQuad(selection_collider, 255, 255, 0, 100);
	}
}

void Heavy::InitEntity()
{
	entity_sprite = App->entity_manager->GetHeavyTexture();

	InitUnitSpriteSections();

	is_selectable = true;
	is_selected = false;
	path_full = false;

	target = nullptr;
	attack_in_cooldown = false;
	accumulated_cooldown = 0.0f;

	speed = 250.0f;

	max_health = 600;
	current_health = max_health;
	attack_damage = 60;

	attack_speed = 1.5f;
	attack_range = 7;

	if (App->entity_manager->CheckTileAvailability(tile_position, this))
	{
		AttachHealthbarToEntity();
	}
}

void Heavy::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -6;			// Magic
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 967, 1, MAX_UNIT_HEALTHBAR_WIDTH, 6 };
	healthbar_rect = { 967, 13, MAX_UNIT_HEALTHBAR_WIDTH, 6 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in UI_Healthbar.

	healthbar = (UI_Healthbar*)App->gui_manager->CreateHealthbar(UI_ELEMENT::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}

void Heavy::InitUnitSpriteSections()
{
	entity_sprite_section = { 58, 0, 58, 47 };						//Down Right

	pathing_up_section = { 0, 47, 70, 52 };
	pathing_down_section = { 71, 47, 70, 52 };
	pathing_rigth_section = { 202, 47, 59, 52 };
	pathing_left_section = { 142, 47, 59, 52 };
	pathing_up_right_section = { 116, 0, 60, 47 };
	pathing_up_left_section = { 176, 0, 59, 47 };
	pathing_down_right_section = { 58, 0, 58, 47 };
	pathing_down_left_section = { 0, 0, 58, 47 };
}

void Heavy::UpdateUnitSpriteSection()
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

void Heavy::SetEntityTargetByProximity()
{
	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if (App->entity_manager->IsEnemyEntity((*item)))
		{
			if (tile_position.DistanceNoSqrt((*item)->tile_position) * 0.1f <= attack_range)
			{
				target = (*item);
				break;
			}
		}
	}
}

void Heavy::GetShortestPathWithinAttackRange()
{
	std::vector<iPoint> tmp;

	if (target != nullptr)
	{
		for (int i = 0; i < (int)entity_path.size(); ++i)
		{
			tmp.push_back(entity_path[i]);

			if ((entity_path[i].DistanceNoSqrt(target->tile_position) * 0.1f) <= attack_range)
			{
				entity_path.clear();

				entity_path = tmp;

				target_tile = entity_path.back();
				current_path_tile = entity_path.begin();

				tmp.clear();

				break;
			}
		}
	}
}

void Heavy::UpdateUnitOrientation()
{
	if (unit_state == ENTITY_STATE::IDLE)
	{
		if (target != nullptr)
		{
			if (tile_position.x > target->tile_position.x && tile_position.y > target->tile_position.y)					// next_tile is (--x , --y)
			{
				entity_sprite_section = pathing_up_section;
				return;
			}

			if (tile_position.x < target->tile_position.x && tile_position.y < target->tile_position.y)					// next_tile is (++x , ++y)
			{
				entity_sprite_section = pathing_down_section;
				return;
			}

			if (tile_position.x < target->tile_position.x && tile_position.y > target->tile_position.y)					// next_tile is (--x , ++y)
			{
				entity_sprite_section = pathing_rigth_section;
				return;
			}

			if (tile_position.x > target->tile_position.x && tile_position.y < target->tile_position.y)					// next_tile is (++x, --y)
			{
				entity_sprite_section = pathing_left_section;
				return;
			}

			if (tile_position.x == target->tile_position.x && tile_position.y > target->tile_position.y)					// next_tile is (== , --y)
			{
				entity_sprite_section = pathing_up_right_section;
				return;
			}

			if (tile_position.x > target->tile_position.x && tile_position.y == target->tile_position.y)					// next tile is (--x, ==)
			{
				entity_sprite_section = pathing_up_left_section;
				return;
			}

			if (tile_position.x < target->tile_position.x && tile_position.y == target->tile_position.y)					// next tile is (++x, ==)
			{
				entity_sprite_section = pathing_down_right_section;
				return;
			}

			if (tile_position.x == target->tile_position.x && tile_position.y < target->tile_position.y)					// next tile is (==, ++y)
			{
				entity_sprite_section = pathing_down_left_section;
				return;
			}
		}
	}
}

bool Heavy::TargetIsInRange()
{
	if (target != nullptr)
	{
		float distance = tile_position.DistanceNoSqrt(target->tile_position) * 0.1f;

		if (distance <= attack_range)
		{
			return true;
		}
	}

	return false;
}

void Heavy::ChaseTarget()
{
	std::vector<Dynamic_Object*> tmp;
	tmp.push_back(this);

	App->pathfinding->ChangeWalkability(occupied_tile, this, WALKABLE);

	//GiveNewTargetTile(target->tile_position);
	App->pathfinding->MoveOrder(target->tile_position, tmp);
}

void Heavy::DealDamage()
{
	if (!attack_in_cooldown)
	{
		ApplyDamage(target);
		App->audio->PlayFx(App->entity_manager->infantry_shot_fx);
		attack_in_cooldown = true;
	}
	else
	{
		accumulated_cooldown += App->GetDt();

		if (accumulated_cooldown >= attack_speed)
		{
			attack_in_cooldown = false;
			accumulated_cooldown = 0.0f;
		}
	}

	if (target->current_health <= 0)
	{
		target = nullptr;
	}
}

// Collision Handling ---------------------------------------
void Heavy::OnCollision(Collider* C1, Collider* C2)
{
	return;
}
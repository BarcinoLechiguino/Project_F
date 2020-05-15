#include "Log.h"

#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Collisions.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Player.h"
#include "GuiManager.h"
#include "UI.h"
#include "UI_Healthbar.h"
#include "SceneManager.h"
#include "FowManager.h"
#include "EntityManager.h"

#include "Infantry.h"


Infantry::Infantry(int x, int y, ENTITY_TYPE type, int level) : DynamicObject(x, y, type, level)  //Constructor. Called at the first frame.
{
	LOG("x %d and y %d", x, y);
	InitEntity();
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

bool Infantry::Update(float dt, bool do_logic)
{
	HandleMovement(dt);

	DataMapSafetyCheck();

	if (!entity_path.empty())
	{
		UpdateUnitSpriteSection();
	}
	else
	{
		UpdateUnitOrientation();
	}

	selection_collider.x = (int)pixel_position.x;
	selection_collider.y = (int)pixel_position.y;

	if (do_logic)
	{
		if (target == nullptr && entity_path.empty())
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
			if (entity_path.empty())
			{
				ChaseTarget();
			}
		}
	}

	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height * 0.5f);

	// FOG OF WAR
	is_visible = fow_entity->is_visible;

	fow_entity->SetPos(tile_position);

	return true;
};

bool Infantry::PostUpdate()
{
	if (current_health <= 0)
	{	
		App->entity_manager->DeleteEntity(this);
	}

	return true;
};

bool Infantry::CleanUp()
{	
	App->pathfinding->ChangeWalkability(occupied_tile, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	if (collider != nullptr)
	{
		collider->to_delete = true;
	}
	
	App->gui_manager->DeleteGuiElement(healthbar);

	App->fow_manager->DeleteFowEntity(fow_entity);

	return true;
};

void Infantry::Draw()
{
	App->render->Blit(this->entity_sprite, (int)pixel_position.x, (int)pixel_position.y - 15, &entity_sprite_section);

	if (App->player->god_mode)
	{
		App->render->DrawQuad(selection_collider, 255, 255, 0, 100);
	}
}

void Infantry::InitEntity()
{
	// TEXTURE & SECTIONS
	entity_sprite = App->entity_manager->GetInfantryTexture();
	InitUnitSpriteSections();

	// FLAGS
	is_selectable = true;
	is_selected = false;
	path_full = false;

	target = nullptr;
	attack_in_cooldown = false;
	accumulated_cooldown = 0.0f;

	// STATS
	speed = 350.0f;

	max_health = 300;
	current_health = max_health;
	attack_damage = 30;

	attack_speed = 0.75f;
	attack_range = 5;

	// HEALTHBAR
	if (App->entity_manager->CheckTileAvailability(tile_position, this))
	{
		AttachHealthbarToEntity();
	}

	//FOG OF WAR
	is_visible = true;
	provides_visibility = true;
	range_of_vision = 5;

	fow_entity = App->fow_manager->CreateFowEntity(tile_position, provides_visibility);

	//fow_entity->frontier = App->fow_manager->CreateRectangularFrontier(range_of_vision, range_of_vision, tile_position);
	fow_entity->frontier = App->fow_manager->CreateCircularFrontier(range_of_vision, tile_position);

	fow_entity->line_of_sight = App->fow_manager->GetLineOfSight(fow_entity->frontier);
}

void Infantry::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -6;			// Magic
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 967, 1, MAX_UNIT_HEALTHBAR_WIDTH, 6 };
	healthbar_rect = { 967, 13, MAX_UNIT_HEALTHBAR_WIDTH, 6 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in UI_Healthbar.

	healthbar = (UI_Healthbar*)App->gui_manager->CreateHealthbar(UI_ELEMENT::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}

void Infantry::InitUnitSpriteSections()
{
	entity_sprite_section		= { 58, 0, 58, 47 };						//Down Right

	pathing_up_section			= { 0, 47, 70, 52 };
	pathing_down_section		= { 71, 47, 70, 52 };
	pathing_rigth_section		= { 202, 47, 59, 52 };
	pathing_left_section		= { 142, 47, 59, 52 };
	pathing_up_right_section	= { 116, 0, 60, 47 };
	pathing_up_left_section		= { 176, 0, 59, 47 };
	pathing_down_right_section	= { 58, 0, 58, 47 };
	pathing_down_left_section	= { 0, 0, 58, 47 };
}

void Infantry::UpdateUnitSpriteSection()
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

void Infantry::SetEntityTargetByProximity()
{
	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if (App->entity_manager->IsEnemyEntity((*item)))
		{
			if (App->pathfinding->DistanceInTiles(tile_position, (*item)->tile_position) <= attack_range)
			{
				target = (*item);
				break;
			}
		}
	}
}

//void Infantry::GetShortestPathWithinAttackRange()
//{
//	std::vector<iPoint> tmp;
//
//	if (target != nullptr)
//	{
//		for (int i = 0; i < (int)entity_path.size(); ++i)
//		{
//			tmp.push_back(entity_path[i]);
//
//			if ((entity_path[i].DistanceNoSqrt(target->tile_position) * 0.1f) <= attack_range)
//			{
//				entity_path.clear();
//
//				entity_path = tmp;
//
//				target_tile = entity_path.back();
//				current_path_tile = entity_path.begin();
//
//				tmp.clear();
//
//				break;
//			}
//		}
//	}
//}

void Infantry::UpdateUnitOrientation()
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

bool Infantry::TargetIsInRange()
{
	if (target != nullptr)
	{
		if (App->pathfinding->DistanceInTiles(tile_position, target->tile_position) <= attack_range)
		{
			return true;
		}
	}

	return false;
}

void Infantry::ChaseTarget()
{
	std::vector<DynamicObject*> tmp;
	tmp.push_back(this);

	App->pathfinding->ChangeWalkability(occupied_tile, this, WALKABLE);

	//GiveNewTargetTile(target->tile_position);
	App->pathfinding->AttackOrder(target->tile_position, tmp);
}

void Infantry::DealDamage()
{
	if (target->current_health > 0)
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
	}
	{
		target = nullptr;

		attack_in_cooldown = false;												// Reseting the attack for the next time the unit has a target.
		accumulated_cooldown = 0.0f;

		return;
	}
}

// Collision Handling ---------------------------------------
void Infantry::OnCollision(Collider* C1, Collider* C2)
{
	return;
}

Entity* Infantry::GetTarget()
{
	return target;
}

int Infantry::GetAttackRange()
{
	return attack_range;
}
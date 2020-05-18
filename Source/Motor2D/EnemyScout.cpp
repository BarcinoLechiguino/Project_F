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
#include "GuiElement.h"
#include "GuiHealthbar.h"
#include "SceneManager.h"
#include "FowManager.h"
#include "EnemyAIManager.h"
#include "EntityManager.h"

#include "EnemyScout.h"


EnemyScout::EnemyScout(int x, int y, ENTITY_TYPE type, int level) : EnemyUnit(x, y, type, level)  //Constructor. Called at the first frame.
{
	LOG("x %d and y %d", x, y);
	InitEntity();
};

EnemyScout::~EnemyScout()  //Destructor. Called at the last frame.
{

};

bool EnemyScout::Awake(pugi::xml_node& config)
{
	return true;
};

bool EnemyScout::Start()
{
	return true;
};

bool EnemyScout::PreUpdate()
{
	return true;
};

bool EnemyScout::Update(float dt, bool do_logic)
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

bool EnemyScout::PostUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->kill_count++;
		App->entity_manager->DeleteEntity(this);
	}

	return true;
};

bool EnemyScout::CleanUp()
{
	App->pathfinding->ChangeWalkability(occupied_tile, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	if (collider != nullptr)
	{
		collider->to_delete = true;
	}

	if (is_selected)
	{
		App->player->DeleteEntityFromBuffers(this);
	}

	App->gui_manager->DeleteGuiElement(healthbar);

	App->fow_manager->DeleteFowEntity(fow_entity);

	App->enemy_AI_manager->DeleteEnemyAIEntity(enemy_AI_entity);

	return true;
};

void EnemyScout::Draw()
{
	App->render->Blit(this->entity_sprite, (int)pixel_position.x, (int)pixel_position.y - 15, &entity_sprite_section);

	if (App->player->god_mode)
	{
		App->render->DrawQuad(selection_collider, 255, 255, 0, 100);
	}
}

void EnemyScout::InitEntity()
{
	// POSITION VARIABLES
	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height * 0.5f);
	
	// TEXTURE & SECTIONS
	entity_sprite = App->entity_manager->GetEnemyScoutTexture();
	InitUnitSpriteSections();

	// FLAGS
	is_selectable = false;
	is_selected = false;
	path_full = false;

	target = nullptr;
	attack_in_cooldown = false;
	accumulated_cooldown = 0.0f;

	// STATS
	speed = 450.0f;

	max_health = 200;
	current_health = max_health;
	attack_damage = 10;

	attack_speed = 0.5f;
	attack_range = 5;

	// HEALTHBAR
	if (App->entity_manager->CheckTileAvailability(tile_position, this))
	{
		AttachHealthbarToEntity();
	}

	// FOG OF WAR
	is_visible = false;
	provides_visibility = false;

	fow_entity = App->fow_manager->CreateFowEntity(tile_position, provides_visibility);

	// ENEMY AI
	enemy_AI_entity = App->enemy_AI_manager->CreateEnemyAIEntity(this);
}

void EnemyScout::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -6;										//Magic
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 967, 1, MAX_UNIT_HEALTHBAR_WIDTH, 6 };
	healthbar_rect = { 967, 7, MAX_UNIT_HEALTHBAR_WIDTH, 6 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in GuiHealthbar.

	healthbar = (GuiHealthbar*)App->gui_manager->CreateHealthbar(GUI_ELEMENT_TYPE::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}

void EnemyScout::InitUnitSpriteSections()
{
	//entity_sprite_section = { 58, 0, 58, 47 };						//Down Right

	//pathing_up_section = { 0, 47, 70, 52 };
	//pathing_down_section = { 71, 47, 70, 52 };
	//pathing_rigth_section = { 202, 47, 59, 52 };
	//pathing_left_section = { 142, 47, 59, 52 };
	//pathing_up_right_section = { 116, 0, 60, 47 };
	//pathing_up_left_section = { 176, 0, 59, 47 };
	//pathing_down_right_section = { 58, 0, 58, 47 };
	//pathing_down_left_section = { 0, 0, 58, 47 };

	//	 --- LOADING FROM XML ---
	pugi::xml_node sections = App->entities_file.child("entities").child("units").child("enemies").child("scout").child("sprite_sections");

	pathing_up_section.x = sections.child("pathing_up").attribute("x").as_int();
	pathing_up_section.y = sections.child("pathing_up").attribute("y").as_int();
	pathing_up_section.w = sections.child("pathing_up").attribute("w").as_int();
	pathing_up_section.h = sections.child("pathing_up").attribute("h").as_int();

	pathing_down_section.x = sections.child("pathing_down").attribute("x").as_int();
	pathing_down_section.y = sections.child("pathing_down").attribute("y").as_int();
	pathing_down_section.w = sections.child("pathing_down").attribute("w").as_int();
	pathing_down_section.h = sections.child("pathing_down").attribute("h").as_int();

	pathing_rigth_section.x = sections.child("pathing_right").attribute("x").as_int();
	pathing_rigth_section.y = sections.child("pathing_right").attribute("y").as_int();
	pathing_rigth_section.w = sections.child("pathing_right").attribute("w").as_int();
	pathing_rigth_section.h = sections.child("pathing_right").attribute("h").as_int();

	pathing_left_section.x = sections.child("pathing_left").attribute("x").as_int();
	pathing_left_section.y = sections.child("pathing_left").attribute("y").as_int();
	pathing_left_section.w = sections.child("pathing_left").attribute("w").as_int();
	pathing_left_section.h = sections.child("pathing_left").attribute("h").as_int();

	pathing_up_right_section.x = sections.child("pathing_up_right").attribute("x").as_int();
	pathing_up_right_section.y = sections.child("pathing_up_right").attribute("y").as_int();
	pathing_up_right_section.w = sections.child("pathing_up_right").attribute("w").as_int();
	pathing_up_right_section.h = sections.child("pathing_up_right").attribute("h").as_int();

	pathing_up_left_section.x = sections.child("pathing_up_left").attribute("x").as_int();
	pathing_up_left_section.y = sections.child("pathing_up_left").attribute("y").as_int();
	pathing_up_left_section.w = sections.child("pathing_up_left").attribute("w").as_int();
	pathing_up_left_section.h = sections.child("pathing_up_left").attribute("h").as_int();

	pathing_down_right_section.x = sections.child("pathing_down_right").attribute("x").as_int();
	pathing_down_right_section.y = sections.child("pathing_down_right").attribute("y").as_int();
	pathing_down_right_section.w = sections.child("pathing_down_right").attribute("w").as_int();
	pathing_down_right_section.h = sections.child("pathing_down_right").attribute("h").as_int();

	pathing_down_left_section.x = sections.child("pathing_down_left").attribute("x").as_int();
	pathing_down_left_section.y = sections.child("pathing_down_left").attribute("y").as_int();
	pathing_down_left_section.w = sections.child("pathing_down_left").attribute("w").as_int();
	pathing_down_left_section.h = sections.child("pathing_down_left").attribute("h").as_int();

	//Default section
	entity_sprite_section = pathing_down_right_section;
}


void EnemyScout::UpdateUnitSpriteSection()
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

void EnemyScout::SetEntityTargetByProximity()
{
	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if (App->entity_manager->IsAllyEntity((*item)))
		{
			if (App->pathfinding->DistanceInTiles(tile_position, (*item)->tile_position) <= attack_range)
			{
				target = (*item);
				break;
			}
		}
	}
}

void EnemyScout::GetShortestPathWithinAttackRange()
{
	//std::vector<iPoint> tmp;

	//if (target != nullptr)
	//{
	//	for (int i = 0; i < (int)entity_path.size(); ++i)
	//	{
	//		tmp.push_back(entity_path[i]);

	//		if ((entity_path[i].DistanceNoSqrt(target->tile_position) * 0.1f) <= attack_range)
	//		{
	//			entity_path.clear();

	//			entity_path = tmp;

	//			target_tile = entity_path.back();
	//			current_path_tile = entity_path.begin();

	//			tmp.clear();

	//			break;
	//		}
	//	}
	//}
}

void EnemyScout::UpdateUnitOrientation()
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

bool EnemyScout::TargetIsInRange()
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

void EnemyScout::ChaseTarget()
{
	std::vector<DynamicObject*> tmp;
	tmp.push_back(this);

	App->pathfinding->ChangeWalkability(occupied_tile, this, WALKABLE);

	//GiveNewTargetTile(target->tile_position);
	App->pathfinding->AttackOrder(target->tile_position, tmp);
}

void EnemyScout::DealDamage()
{
	if (target->current_health > 0)
	{
		if (!attack_in_cooldown)
		{
			ApplyDamage(target);
			App->audio->PlayFx(App->entity_manager->infantry_shooting_fx);
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
	else
	{
		target = nullptr;

		attack_in_cooldown = false;
		accumulated_cooldown = 0.0f;

		return;
	}
}

// Collision Handling ---------------------------------------
void EnemyScout::OnCollision(Collider* C1, Collider* C2)
{
	return;
}

Entity* EnemyScout::GetTarget()
{
	return target;
}

int  EnemyScout::GetAttackRange()
{
	return attack_range;
}
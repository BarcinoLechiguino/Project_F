#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
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

#include "EnemyInfantry.h"


EnemyInfantry::EnemyInfantry(int x, int y, ENTITY_TYPE type, int level) : EnemyUnit(x, y, type, level)  //Constructor. Called at the first frame.
{
	InitEntity();
};

EnemyInfantry::~EnemyInfantry()  //Destructor. Called at the last frame.
{

};

bool EnemyInfantry::Awake(pugi::xml_node& config)
{
	return true;
};

bool EnemyInfantry::Start()
{
	return true;
};

bool EnemyInfantry::PreUpdate()
{	
	return true;
};

bool EnemyInfantry::Update(float dt, bool do_logic)
{
	HandleMovement(dt);

	DataMapSafetyCheck();

	UpdateRedState();

	if (!entity_path.empty())
	{
		UpdateUnitSpriteSection();
	}
	else
	{
		UpdateUnitOrientation();
	}

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

bool EnemyInfantry::PostUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->kill_count++;
		App->entity_manager->DeleteEntity(this);
	}
	
	return true;
};

bool EnemyInfantry::CleanUp()
{
	App->pathfinding->ChangeWalkability(occupied_tile, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	if (is_selected)
	{
		App->player->DeleteEntityFromBuffers(this);
	}

	App->gui_manager->DeleteGuiElement(healthbar);

	App->fow_manager->DeleteFowEntity(fow_entity);

	App->enemy_AI_manager->DeleteEnemyAIEntity(enemy_AI_entity);
	
	return true;
};

void EnemyInfantry::Draw()
{
	if (this->red_state == false) {
		App->render->Blit(this->entity_sprite, (int)pixel_position.x, (int)pixel_position.y - 15, &entity_sprite_section);
	}
	if (this->red_state == true) {
		App->render->Blit(this->entity_sprite, (int)pixel_position.x, (int)pixel_position.y - 15, &entity_sprite_section, false, 1.0F, 1.0F, 0.0, 0, 0, App->render->renderer, { 255, 128, 128, 255 });
	}
}

void EnemyInfantry::InitEntity()
{
	// POSITION VARIABLES
	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height * 0.5f);
	
	// TEXTURE & SECTIONS
	entity_sprite = App->entity_manager->GetEnemyInfantryTexture();
	InitUnitSpriteSections();

	// FLAGS
	is_selectable = false;
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

	// FOG OF WAR
	is_visible = false;
	is_neutral = false;
	provides_visibility = false;

	fow_entity = App->fow_manager->CreateFowEntity(tile_position, is_neutral, provides_visibility);

	// ENEMY AI
	enemy_AI_entity = App->enemy_AI_manager->CreateEnemyAIEntity(this);
}

void EnemyInfantry::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -6;										//Magic
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 967, 1, MAX_UNIT_HEALTHBAR_WIDTH, 6 };
	healthbar_rect = { 967, 7, MAX_UNIT_HEALTHBAR_WIDTH, 6 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in GuiHealthbar.

	healthbar = (GuiHealthbar*)App->gui_manager->CreateHealthbar(GUI_ELEMENT_TYPE::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}

void EnemyInfantry::InitUnitSpriteSections()
{
	/*entity_sprite_section		= { 58, 0, 58, 47 };

	pathing_up_section			= { 0, 47, 70, 52 };
	pathing_down_section		= { 71, 47, 70, 52 };
	pathing_rigth_section		= { 202, 47, 59, 52 };
	pathing_left_section		= { 142, 47, 59, 52 };
	pathing_up_right_section	= { 116, 0, 60, 47 };
	pathing_up_left_section		= { 176, 0, 59, 47 };
	pathing_down_right_section	= { 58, 0, 58, 47 };
	pathing_down_left_section	= { 0, 0, 58, 47 };*/

	//	 --- LOADING FROM XML ---
	pugi::xml_node sections = App->entities_file.child("entities").child("units").child("enemies").child("enemy_infantry").child("sprite_sections");

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

void EnemyInfantry::UpdateUnitSpriteSection()
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

void EnemyInfantry::SetEntityTargetByProximity()
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

void EnemyInfantry::GetShortestPathWithinAttackRange()
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

void EnemyInfantry::UpdateUnitOrientation()
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

		if (tile_position.x == target->tile_position.x && tile_position.y > target->tile_position.y)				// next_tile is (== , --y)
		{
			entity_sprite_section = pathing_up_right_section;
			return;
		}

		if (tile_position.x > target->tile_position.x && tile_position.y == target->tile_position.y)				// next tile is (--x, ==)
		{
			entity_sprite_section = pathing_up_left_section;
			return;
		}

		if (tile_position.x < target->tile_position.x && tile_position.y == target->tile_position.y)				// next tile is (++x, ==)
		{
			entity_sprite_section = pathing_down_right_section;
			return;
		}

		if (tile_position.x == target->tile_position.x && tile_position.y < target->tile_position.y)				// next tile is (==, ++y)
		{
			entity_sprite_section = pathing_down_left_section;
			return;
		}
	}
}

bool EnemyInfantry::TargetIsInRange()
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

void EnemyInfantry::ChaseTarget()
{
	std::vector<DynamicObject*> tmp;
	tmp.push_back(this);

	App->pathfinding->ChangeWalkability(occupied_tile, this, WALKABLE);

	//GiveNewTargetTile(target->tile_position);
	App->pathfinding->AttackOrder(target->tile_position, tmp);
}

void EnemyInfantry::DealDamage()
{
	if (target->current_health > 0)
	{
		if (!attack_in_cooldown)
		{
			ApplyDamage(target);
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

Entity* EnemyInfantry::GetTarget()
{
	return target;
}

int  EnemyInfantry::GetAttackRange()
{
	return attack_range;
}

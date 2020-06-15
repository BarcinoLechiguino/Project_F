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
#include "GuiCreationBar.h"
#include "FowManager.h"
#include "EnemyAIManager.h"
#include "EntityManager.h"
#include "EnemyInfantry.h"
#include "EnemyHeavy.h"

#include "EnemyBarracks.h"


EnemyBarracks::EnemyBarracks(int x, int y, ENTITY_TYPE type, int level) : Building(x, y, type, level)
{
	InitEntity();
}

EnemyBarracks::~EnemyBarracks()
{

}

bool EnemyBarracks::Awake(pugi::xml_node&)
{
	return true;
}

bool EnemyBarracks::Start()
{
	App->pathfinding->ChangeWalkability(tile_position, this, NON_WALKABLE);

	return true;
}

bool EnemyBarracks::PreUpdate()
{
	return true;
}

bool EnemyBarracks::Update(float dt, bool do_logic)
{
	UpdateRedState();
	
	if (creation_queue.size() != 0)
	{
		if (!creating_unit)
		{
			StartUnitCreation();
		}
		else
		{
			creation_bar->UpdateCreationBarValue();

			if (creation_bar->creation_finished)
			{
				GenerateUnitByType(created_unit_type);

				created_unit_type = ENTITY_TYPE::UNKNOWN;

				creation_queue.erase(creation_queue.begin());

				creating_unit = false;
			}
		}
	}
	else
	{
		creation_bar->is_visible = false;
	}

	// FOG OF WAR
	is_visible = fow_entity->is_visible;									// No fow_entity->SetPos(tile_position) as, obviously, a StaticObject entity will never move.

	return true;
}

bool EnemyBarracks::PostUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->DeleteEntity(this);
	}
	
	return true;
}

bool EnemyBarracks::CleanUp()
{
	App->pathfinding->ChangeWalkability(tile_position, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	if (is_selected)
	{
		App->player->DeleteEntityFromBuffers(this);
	}

	App->gui_manager->DeleteGuiElement(healthbar);
	App->gui_manager->DeleteGuiElement(creation_bar);

	App->fow_manager->DeleteFowEntity(fow_entity);

	App->enemy_AI_manager->DeleteEnemyAIEntity(enemy_AI_entity);

	return true;
}

void EnemyBarracks::Draw()
{
	if (this->red_state == false) {
		App->render->Blit(entity_sprite, (int)pixel_position.x - 27, (int)pixel_position.y - 18, &barracks_rect); //Magic
	}
	if (this->red_state == true) {
		App->render->Blit(entity_sprite, (int)pixel_position.x - 27, (int)pixel_position.y - 18, &barracks_rect, false, 1.0F, 1.0F, 0.0, 0, 0, App->render->renderer, { 255, 192, 192, 255 });
	}
}

void EnemyBarracks::StartUnitCreation()
{
	creation_bar->is_visible = true;

	creating_unit = true;

	created_unit_type = (*creation_queue.begin());

	switch (created_unit_type)
	{
	case ENTITY_TYPE::ENEMY_INFANTRY:
		creation_bar->SetNewCreationTime(enemy_infantry_creation_time);
		break;

	case ENTITY_TYPE::ENEMY_HEAVY:
		creation_bar->SetNewCreationTime(enemy_heavy_creation_time);
		break;
	}
}

void EnemyBarracks::GenerateUnit(ENTITY_TYPE type, int level)
{
	iPoint pos = App->pathfinding->FindNearbyPoint(iPoint(tile_position.x, tile_position.y + 2));
	
	switch (type)
	{
	case ENTITY_TYPE::ENEMY_INFANTRY:
		(EnemyInfantry*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_INFANTRY, pos.x, pos.y, level);
		break;

	case ENTITY_TYPE::ENEMY_HEAVY:
		(EnemyHeavy*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_HEAVY, pos.x, pos.y, level);
		break;
	}
}

void EnemyBarracks::GenerateUnitByType(ENTITY_TYPE type)
{
	iPoint pos = App->pathfinding->FindNearbyPoint(iPoint(tile_position.x, tile_position.y + 2));

	switch (type)
	{
	case ENTITY_TYPE::ENEMY_INFANTRY:
		(EnemyInfantry*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_INFANTRY, pos.x, pos.y, enemy_infantry_level);
		break;

	case ENTITY_TYPE::ENEMY_HEAVY:
		(EnemyHeavy*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_HEAVY, pos.x, pos.y, enemy_heavy_level);
		break;
	}
}

void EnemyBarracks::LevelChanges()
{
	switch (level)								//Updates the building stats when leveling up	
	{
	case 1:
		barracks_rect = barracks_rect_1;
		break;
	case 2:
		barracks_rect = barracks_rect_2;
		max_health = 800;
		current_health = max_health;
		enemy_infantry_level++;
		break;
	default:
		barracks_rect = barracks_rect_2;
		break;
	}
}

void EnemyBarracks::InitEntity()
{
	// POSITION & SIZE
	iPoint world_position = App->map->MapToWorld(tile_position.x, tile_position.y);

	pixel_position.x = (float)world_position.x;
	pixel_position.y = (float)world_position.y;

	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height);
	
	tiles_occupied.x = 2;
	tiles_occupied.y = 2;

	// TEXTURE & SECTIONS
	entity_sprite = App->entity_manager->GetEnemyBarracksTexture();
	barracks_rect_1 = { 0, 0, 106, 95 };
	barracks_rect_2 = { 108, 0, 106, 95 };
	barracks_rect = barracks_rect_1;

	// FLAGS
	is_selected = false;

	// BUILDING CONSTRUCTION VARIABLES
	construction_time = 5.0f;

	// UNIT CREATION VARIABLES
	creating_unit = false;
	created_unit_type = ENTITY_TYPE::UNKNOWN;

	enemy_infantry_creation_time = 2.0f;
	enemy_heavy_creation_time = 5.0f;

	// STATS
	max_health = 600;
	current_health = max_health;

	enemy_infantry_level = 1;
	enemy_heavy_level = 1;

	// HEALTHBAR & CREATION BAR
	if (App->entity_manager->CheckTileAvailability(tile_position, this))
	{
		AttachHealthbarToEntity();
		AttachCreationBarToEntity();
	}

	// FOG OF WAR
	is_visible = false;
	is_neutral = false;
	provides_visibility = false;

	fow_entity = App->fow_manager->CreateFowEntity(tile_position, is_neutral, provides_visibility);

	// ENEMY AI
	enemy_AI_entity = App->enemy_AI_manager->CreateEnemyAIEntity(this);
	App->enemy_AI_manager->enemy_barracks = this;
}

void EnemyBarracks::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -6;
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 618, 1, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };
	healthbar_rect = { 618, 12, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in GuiHealthbar.

	healthbar = (GuiHealthbar*)App->gui_manager->CreateHealthbar(GUI_ELEMENT_TYPE::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}

void EnemyBarracks::AttachCreationBarToEntity()
{
	creation_bar_position_offset.x = -6;															// Magic
	creation_bar_position_offset.y = 16;

	creation_bar_background_rect = { 618, 1, MAX_CREATION_BAR_WIDTH, 9 };
	creation_bar_rect = { 618, 23, MAX_CREATION_BAR_WIDTH, 9 };

	int creation_bar_position_x = (int)pixel_position.x + creation_bar_position_offset.x;
	int creation_bar_position_y = (int)pixel_position.y + creation_bar_position_offset.y;

	creation_bar = (GuiCreationBar*)App->gui_manager->CreateCreationBar(GUI_ELEMENT_TYPE::CREATIONBAR, creation_bar_position_x, creation_bar_position_y, false
		, &creation_bar_rect, &creation_bar_background_rect, this);
}
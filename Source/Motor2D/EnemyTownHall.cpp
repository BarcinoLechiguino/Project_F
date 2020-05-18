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
#include "EnemyGatherer.h"

#include "EnemyTownHall.h"


EnemyTownHall::EnemyTownHall(int x, int y, ENTITY_TYPE type, int level) : Building(x, y, type, level)
{
	InitEntity();
}

EnemyTownHall::~EnemyTownHall()
{

}

bool EnemyTownHall::Awake(pugi::xml_node&)
{
	return true;
}

bool EnemyTownHall::Start()
{
	App->pathfinding->ChangeWalkability(tile_position, this, NON_WALKABLE);

	return true;
}

bool EnemyTownHall::PreUpdate()
{
	return true;
}

bool EnemyTownHall::Update(float dt, bool do_logic)
{
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

bool EnemyTownHall::PostUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->DeleteEntity(this);
	}
	
	return true;
}

bool EnemyTownHall::CleanUp()
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

void EnemyTownHall::Draw()
{
	App->render->Blit(entity_sprite, (int)pixel_position.x - 51, (int)pixel_position.y - 20, &hall_rect);
}

void EnemyTownHall::StartUnitCreation()
{
	creation_bar->is_visible = true;

	creating_unit = true;

	created_unit_type = (*creation_queue.begin());

	switch (created_unit_type)															// Used a switch taking into account scalability.
	{
	case ENTITY_TYPE::ENEMY_GATHERER:
		creation_bar->SetNewCreationTime(enemy_gatherer_creation_time);
		break;
	}
}

void EnemyTownHall::GenerateUnit(ENTITY_TYPE type, int level)
{
	iPoint pos = App->pathfinding->FindNearbyPoint(iPoint(tile_position.x, tile_position.y + 2));
	
	switch (type)
	{
	case ENTITY_TYPE::ENEMY_GATHERER:
		(EnemyGatherer*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_GATHERER, pos.x, pos.y, level);
		break;
	}
}

void EnemyTownHall::GenerateUnitByType(ENTITY_TYPE type)
{
	iPoint pos = App->pathfinding->FindNearbyPoint(iPoint(tile_position.x, tile_position.y + 2));

	switch (type)
	{
	case ENTITY_TYPE::ENEMY_GATHERER:
		(EnemyGatherer*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_GATHERER, pos.x, pos.y, enemy_gatherer_level);
		break;
	}
}

void EnemyTownHall::LevelChanges()				//Updates the building stats when leveling up
{
	switch (level)
	{
	case 1:
		hall_rect = hall_rect_1;
		break;
	case 2:
		hall_rect = hall_rect_2;
		max_health = 1200;
		current_health = max_health;
		enemy_gatherer_level++;
		break;
	default:
		hall_rect = hall_rect_2;
		break;
	}
}

void EnemyTownHall::InitEntity()
{
	// POSITION & SIZE
	iPoint world_position = App->map->MapToWorld(tile_position.x, tile_position.y);

	pixel_position.x = (float)world_position.x;
	pixel_position.y = (float)world_position.y;

	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height + App->map->data.tile_height * 0.5f);

	tiles_occupied.x = 3;
	tiles_occupied.y = 3;

	// TEXTURE & SECTIONS
	hall_rect_1 = { 0, 0, 155, 138 };
	hall_rect_2 = { 155, 0, 155, 138 };
	hall_rect = hall_rect_1;
	entity_sprite = App->entity_manager->GetEnemyTownHallTexture();

	// FLAGS
	is_selected = false;

	// UNIT CREATION VARIABLES
	created_unit_type = ENTITY_TYPE::UNKNOWN;
	enemy_gatherer_creation_time = 1.0f;														//Magic

	// STATS
	max_health = 900;
	current_health = max_health;

	enemy_gatherer_level = 1;

	// HEALTHBAR & CREATION BAR
	if (App->entity_manager->CheckTileAvailability(tile_position, this))
	{
		AttachHealthbarToEntity();
		AttachCreationBarToEntity();
	}

	// FOG OF WAR
	/*is_visible = false;
	provides_visibility = false;

	fow_entity = App->fow_manager->CreateFowEntity(tile_position, provides_visibility);*/

	is_visible = true;
	provides_visibility = true;
	range_of_vision = 6;

	fow_entity = App->fow_manager->CreateFowEntity(tile_position + iPoint(1, 1), provides_visibility);

	fow_entity->frontier = App->fow_manager->CreateCircularFrontier(range_of_vision, tile_position + iPoint(1, 1));
	fow_entity->line_of_sight = App->fow_manager->GetLineOfSight(fow_entity->frontier);

	// ENEMY AI
	enemy_AI_entity = App->enemy_AI_manager->CreateEnemyAIEntity(this);
}

void EnemyTownHall::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -20;
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 618, 1, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };
	healthbar_rect = { 618, 12, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in GuiHealthbar.

	healthbar = (GuiHealthbar*)App->gui_manager->CreateHealthbar(GUI_ELEMENT_TYPE::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}

void EnemyTownHall::AttachCreationBarToEntity()
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
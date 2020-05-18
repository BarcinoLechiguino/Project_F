#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Player.h"
#include "GuiManager.h"
#include "GuiElement.h"
#include "GuiHealthbar.h"
#include "FowManager.h"
#include "EntityManager.h"

#include "EnemyWall.h"

EnemyWall::EnemyWall(int x, int y, ENTITY_TYPE type, int level) : Building(x, y, type, level)
{
	InitEntity();
}

EnemyWall::~EnemyWall()
{

}

bool EnemyWall::Awake(pugi::xml_node& config)
{
	return true;
}

bool EnemyWall::Start()
{
	App->pathfinding->ChangeWalkability(tile_position, this, NON_WALKABLE);
	
	return true;
}

bool EnemyWall::PreUpdate()
{
	return true;
}

bool EnemyWall::Update(float dt, bool do_logic)
{
	// FOG OF WAR
	is_visible = fow_entity->is_visible;
	
	return true;
}

bool EnemyWall::PostUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->DeleteEntity(this);
	}
	
	return true;
}

bool EnemyWall::CleanUp()
{
	App->pathfinding->ChangeWalkability(tile_position, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	if (is_selected)
	{
		App->player->DeleteEntityFromBuffers(this);
	}

	App->gui_manager->DeleteGuiElement(healthbar);

	App->fow_manager->DeleteFowEntity(fow_entity);

	return true;
}

void EnemyWall::Draw()
{
	//App->render->Blit(entity_sprite, (int)pixel_position.x, (int)pixel_position.y - 20, &enemy_wall_rect); //Magic
	App->render->Blit(entity_sprite, (int)pixel_position.x + 2, (int)pixel_position.y + 2, NULL); //Magic
}

void EnemyWall::LevelChanges()
{
	switch (level)
	{
	case 1:
		enemy_wall_rect = enemy_wall_rect_1;
		break;
	case 2:
		enemy_wall_rect = enemy_wall_rect_2;
		max_health = 750;
		current_health = max_health;
		break;
	default:
		enemy_wall_rect = enemy_wall_rect_2;
		break;
	}
}

void EnemyWall::InitEntity()
{
	// POSITION & SIZE
	iPoint world_position = App->map->MapToWorld(tile_position.x, tile_position.y);

	pixel_position.x = (float)world_position.x;
	pixel_position.y = (float)world_position.y;

	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height);

	tiles_occupied.x = 1;
	tiles_occupied.y = 1;

	// TEXTURE & SECTIONS
	entity_sprite = App->entity_manager->GetEnemyWallTexture();
	enemy_wall_rect_1 = { 0, 0, 106, 95 };										// NEEDS REVISION
	enemy_wall_rect_2 = { 108, 0, 106, 95 };
	enemy_wall_rect = enemy_wall_rect_1;

	// FLAGS
	is_selected = false;

	// STATS
	max_health = 500;
	current_health = max_health;

	// HEALTHBAR & CREATION BAR
	if (App->entity_manager->CheckTileAvailability(tile_position, this))
	{
		AttachHealthbarToEntity();
	}

	// FOG OF WAR
	is_visible = false;
	provides_visibility = false;

	fow_entity = App->fow_manager->CreateFowEntity(tile_position, provides_visibility);
}

void EnemyWall::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -6;
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 618, 1, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };
	healthbar_rect = { 618, 12, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in GuiHealthbar.

	healthbar = (GuiHealthbar*)App->gui_manager->CreateHealthbar(GUI_ELEMENT_TYPE::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}
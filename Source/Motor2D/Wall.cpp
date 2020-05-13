#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Map.h"
#include "Pathfinding.h"
#include "GuiManager.h"
#include "UI.h"
#include "UI_Healthbar.h"
#include "UI_CreationBar.h"
#include "FowManager.h"
#include "EntityManager.h"

#include "Wall.h"

Wall::Wall(int x, int y, ENTITY_TYPE type, int level) : Building(x, y, type, level)
{
	InitEntity();
}

Wall::~Wall()
{

}

bool Wall::Awake(pugi::xml_node& config)
{
	return true;
}

bool Wall::Start()
{
	App->pathfinding->ChangeWalkability(tile_position, this, NON_WALKABLE);

	return true;
}

bool Wall::PreUpdate()
{
	return true;
}

bool Wall::Update(float dt, bool do_logic)
{
	// FOG OF WAR
	is_visible = fow_entity->is_visible;
	
	return true;
}

bool Wall::PostUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->DeleteEntity(this);
	}
	
	return true;
}

bool Wall::CleanUp()
{
	App->pathfinding->ChangeWalkability(tile_position, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.

	entity_sprite = nullptr;

	App->gui_manager->DeleteGuiElement(healthbar);
	App->gui_manager->DeleteGuiElement(creation_bar);

	App->fow_manager->DeleteFowEntity(fow_entity);
	
	return true;
}

void Wall::Draw()
{
	//App->render->Blit(entity_sprite, (int)pixel_position.x, (int)pixel_position.y - 20, &wall_rect); //Magic 
	
	App->render->Blit(entity_sprite, (int)pixel_position.x + 2, (int)pixel_position.y + 2, NULL); //Magic 
}

void Wall::LevelChanges()
{
	switch (level)
	{
	case 1:
		wall_rect = wall_rect_1;
		break;
	case 2:
		wall_rect = wall_rect_2;
		max_health = 750;
		current_health = max_health;
		//infantry_level++;									// TALK ABOUT THIS
		break;
	default:
		wall_rect = wall_rect_2;
		break;
	}
}

void Wall::InitEntity()
{
	// POSITION & SIZE
	iPoint world_position = App->map->MapToWorld(tile_position.x, tile_position.y);

	pixel_position.x = (float)world_position.x;
	pixel_position.y = (float)world_position.y;

	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height);

	tiles_occupied.x = 1;
	tiles_occupied.y = 1;

	// TEXTURE & SECTIONS
	entity_sprite = App->entity_manager->GetWallTexture();
	wall_rect_1 = { 0, 0, 106, 95 };									// NEED REVISION
	wall_rect_2 = { 108, 0, 106, 95 };
	wall_rect = wall_rect_1;

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
	is_visible = true;
	provides_visibility = true;
	range_of_vision = 4;

	fow_entity = App->fow_manager->CreateFowEntity(tile_position, provides_visibility);

	//fow_entity->frontier = App->fow_manager->CreateRectangularFrontier(range_of_vision, range_of_vision, tile_position);
	fow_entity->frontier = App->fow_manager->CreateCircularFrontier(range_of_vision, tile_position);

	fow_entity->line_of_sight = App->fow_manager->GetLineOfSight(fow_entity->frontier);
}

void Wall::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -6;																//Magic
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 618, 1, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };
	healthbar_rect = { 618, 23, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in UI_Healthbar.

	healthbar = (UI_Healthbar*)App->gui_manager->CreateHealthbar(UI_ELEMENT::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}
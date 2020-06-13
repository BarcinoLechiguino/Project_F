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
#include "FowManager.h"
#include "EntityManager.h"
#include "ParticleManager.h"
#include "Emitter.h"

#include "Rock.h"


Rock::Rock(int x, int y, ENTITY_TYPE type, int level) : Resource(x, y, type, level)
{
	InitEntity();
}

Rock::~Rock()
{

}

bool Rock::Awake(pugi::xml_node&)
{
	return true;
}

bool Rock::Start()
{
	App->pathfinding->ChangeWalkability(tile_position, this, NON_WALKABLE);
	ShineEmitter = App->particle_manager->SpawnEmitter({ pixel_position.x, pixel_position.y - 0 }, EMITTER_ROCK);
	return true;
}

bool Rock::PreUpdate()
{
	return true;
}

bool Rock::Update(float dt, bool do_logic)
{
	// FOG OF WAR
	is_visible = fow_entity->is_visible;									// No fow_entity->SetPos(tile_position) as, obviously, a StaticObject entity will never move.
	
	return true;
}

bool Rock::PostUpdate()
{
	if (current_health <= 0)
	{
		App->entity_manager->DeleteEntity(this);
		App->audio->PlayFx(App->entity_manager->gatherer_gathering_finished_fx);
	}

	return true;
}

bool Rock::CleanUp()
{
	App->pathfinding->ChangeWalkability(tile_position, this, WALKABLE);		//The entity is cleared from the walkability map.
	App->entity_manager->ChangeEntityMap(tile_position, this, true);		//The entity is cleared from the entity_map.
	App->particle_manager->DeleteEmitter(ShineEmitter);

	entity_sprite = nullptr;
	
	if (is_selected)
	{
		App->player->DeleteEntityFromBuffers(this);
	}

	App->gui_manager->DeleteGuiElement(healthbar);

	App->fow_manager->DeleteFowEntity(fow_entity);
	
	delete blit_section;

	return true;
}

void Rock::Draw()
{
	App->render->Blit(entity_sprite, (int)pixel_position.x, (int)pixel_position.y, blit_section);
}

void Rock::InitEntity()
{
	// POSITION & SIZE
	iPoint world_position = App->map->MapToWorld(tile_position.x, tile_position.y);

	pixel_position.x = (float)world_position.x;
	pixel_position.y = (float)world_position.y;

	center_point = fPoint(pixel_position.x, pixel_position.y + App->map->data.tile_height * 0.5f);

	tiles_occupied.x = 1;
	tiles_occupied.y = 1;
	
	// TEXTURE & SECTIONS
	entity_sprite = App->entity_manager->GetRockTexture();
	int rock_version = (rand() % 4) * 54;
	blit_section = new SDL_Rect{ rock_version, 0, 54, 35 };

	// FLAGS
	is_selected = false;

	// STATS
	max_health = 40;
	current_health = max_health;

	// HEALTHBAR
	if (App->entity_manager->CheckTileAvailability(iPoint(tile_position), this))
	{
		AttachHealthbarToEntity();
	}

	// FOG OF WAR
	is_visible = false;
	is_neutral = true;
	provides_visibility = false;

	fow_entity = App->fow_manager->CreateFowEntity(tile_position, is_neutral, provides_visibility);
}

void Rock::AttachHealthbarToEntity()
{
	healthbar_position_offset.x = -30;
	healthbar_position_offset.y = -6;

	healthbar_background_rect = { 618, 1, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };
	healthbar_rect = { 618, 34, MAX_BUILDING_HEALTHBAR_WIDTH, 9 };

	int healthbar_position_x = (int)pixel_position.x + healthbar_position_offset.x;					// X and Y position of the healthbar's hitbox.
	int healthbar_position_y = (int)pixel_position.y + healthbar_position_offset.y;					// The healthbar's position is already calculated in GuiHealthbar.

	healthbar = (GuiHealthbar*)App->gui_manager->CreateHealthbar(GUI_ELEMENT_TYPE::HEALTHBAR, healthbar_position_x, healthbar_position_y, true, &healthbar_rect, &healthbar_background_rect, this);
}
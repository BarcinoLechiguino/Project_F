#include "Application.h"
#include "Player.h"
#include "EntityManager.h"
#include "Entity.h"
#include "GuiManager.h"

#include "UI_Healthbar.h"


UI_Healthbar::UI_Healthbar(UI_ELEMENT element, int x, int y, bool is_visible, SDL_Rect* healthbar, SDL_Rect* background, Entity* attached_entity)
	: UI(element, x, y, *healthbar, nullptr, nullptr)
{
	tex = App->gui_manager->GetAtlas();
	
	this->is_visible = is_visible;
	this->attached_entity = attached_entity;

	if (healthbar != nullptr)
	{
		this->healthbar = *healthbar;
	}

	if (background != nullptr)
	{
		this->background = *background;
	}
}

UI_Healthbar::~UI_Healthbar()
{

}

bool UI_Healthbar::Draw()
{
	CheckInput();
	
	UpdateHealthbarPosition();

	if (attached_entity->current_health != attached_entity->max_health || attached_entity->is_selected || App->player->god_mode)
	{
		BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &background, 1.0f, 1.0f);
		BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &healthbar, 1.0f, 1.0f);
	}
	
	return true;
}

void UI_Healthbar::CheckInput()
{

}

void UI_Healthbar::CleanUp()
{	
	tex = nullptr;
}

void UI_Healthbar::UpdateHealthbarPosition()
{
	int position_x = (int)attached_entity->pixel_position.x + attached_entity->healthbar_position_offset.x;
	int position_y = (int)attached_entity->pixel_position.y + attached_entity->healthbar_position_offset.y;

	SetScreenPos({ position_x, position_y });

	SetHitbox({ position_x, position_y, GetHitbox().w, GetHitbox().h });
}

void UI_Healthbar::UpdateHealthbarValue()
{
	int new_width = 0;

	if (App->entity_manager->IsUnit(attached_entity))
	{
		new_width = (MAX_UNIT_HEALTHBAR_WIDTH * attached_entity->current_health) / attached_entity->max_health;
	}

	if (App->entity_manager->IsBuilding(attached_entity))
	{
		new_width = (MAX_BUILDING_HEALTHBAR_WIDTH * attached_entity->current_health) / attached_entity->max_health;
	}

	if (App->entity_manager->IsResource(attached_entity))
	{
		new_width = (MAX_RESOURCE_HEALTHBAR_WIDTH * attached_entity->current_health) / attached_entity->max_health;
	}

	healthbar.w = new_width;
}
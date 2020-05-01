#include "Application.h"
#include "Window.h"
#include "Player.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Static_Object.h"
#include "TownHall.h"
#include "EnemyTownHall.h"
#include "Barracks.h"
#include "EnemyBarracks.h"
#include "GuiManager.h"

#include "UI_Healthbar.h"

#include "Textures.h"


UI_Healthbar::UI_Healthbar(UI_ELEMENT element, int x, int y, bool is_visible, SDL_Rect* healthbar, SDL_Rect* background, Entity* attached_unit, bool is_creation_bar, Module* listener, UI* parent)
	: UI(element, x, y, *healthbar, listener, parent)
{
	creation_timer = 0.f;
	creation_complete = false;

	tex = App->gui_manager->GetAtlas();
	
	this->is_visible = is_visible;
	this->attached_unit = attached_unit;

	if (this->is_interactible)													//If the Healthbar element is interactible.
	{
		this->listener = listener;												//This Healthbar's listener is set to the App->gui module (For OnCallEvent()).
	}

	if (parent != nullptr)														//If a parent is passed as argument.
	{
		int localPosX = x - parent->GetScreenPos().x;							//Gets the local position of the Healthbar element in the X Axis.
		int localPosY = y - parent->GetScreenPos().y;							//Gets the local position of the Healthbar element in the Y Axis.

		iPoint localPos = { localPosX, localPosY };								//Buffer iPoint to pass it as argument to SetLocalPos().

		SetLocalPos(localPos);													//Sets the local poisition of this Healthbar Element to the given localPos.
	}

	if (healthbar != nullptr)
	{
		this->healthbar = *healthbar;
	}

	if (background != nullptr)
	{
		this->background = *background;
	}

	this->is_creation_bar = is_creation_bar;

	if (is_creation_bar)
	{
		creation_complete = false;
		creation_timer = 0.0f;
		this->healthbar.w = 0;
	}
}

UI_Healthbar::~UI_Healthbar()
{

}

bool UI_Healthbar::Draw()
{
	CheckInput();
	
	UpdateHealthbarPosition();

	if (!is_creation_bar)
	{
		if (attached_unit->current_health != attached_unit->max_health || attached_unit->is_selected || App->player->god_mode)
		{
			BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &background, 1.0f, 1.0f);
			BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &healthbar, 1.0f, 1.0f);
		}
	}
	else
	{
		if (!creation_complete)
		{
			BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &background, 1.0f, 1.0f);
			BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &healthbar, 1.0f, 1.0f);
		}
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
	int position_x = (int)attached_unit->pixel_position.x + attached_unit->healthbar_position_offset.x;
	int position_y = (int)attached_unit->pixel_position.y + attached_unit->healthbar_position_offset.y;

	SetScreenPos({ position_x, position_y });

	SetHitbox({ position_x, position_y, GetHitbox().w, GetHitbox().h });
}

void UI_Healthbar::UpdateBarValue()
{
	if (!is_creation_bar)
	{
		UpdateHealthBarValue();
	}
	else
	{
		UpdateCreationBarValue();
	}
}

void UI_Healthbar::UpdateHealthBarValue()
{
	int new_width = 0;

	if (App->entity_manager->IsUnit(attached_unit))
	{
		new_width = (MAX_UNIT_HEALTHBAR_WIDTH * attached_unit->current_health) / attached_unit->max_health;
	}

	if (App->entity_manager->IsBuilding(attached_unit))
	{
		new_width = (MAX_BUILDING_HEALTHBAR_WIDTH * attached_unit->current_health) / attached_unit->max_health;
	}

	if (App->entity_manager->IsResource(attached_unit))
	{
		new_width = (MAX_RESOURCE_HEALTHBAR_WIDTH * attached_unit->current_health) / attached_unit->max_health;
	}

	healthbar.w = new_width;
}

void UI_Healthbar::UpdateCreationBarValue()
{
	int new_width = 0;
	
	/*if (!creation_complete)
	{
		if (App->entity_manager->IsBuilding(attached_unit))
		{
			creation_timer += App->GetDt();

			if (attached_unit->type == ENTITY_TYPE::TOWNHALL)
			{
				TownHall* townhall = (TownHall*)attached_unit;

				new_width = (int)((MAX_BUILDING_HEALTHBAR_WIDTH * creation_timer) / townhall->gatherer_creation_time);
			}

			if (attached_unit->type == ENTITY_TYPE::ENEMY_TOWNHALL)
			{
				EnemyTownHall* enemy_townhall = (EnemyTownHall*)attached_unit;

				new_width = (int)((MAX_BUILDING_HEALTHBAR_WIDTH * creation_timer) / enemy_townhall->enemy_gatherer_creation_time);
			}

			/*if (creation_timer >= building->unit_creation_time)
			{
				creation_complete = true;
				creation_timer = 0.0f;
			}
		}

		healthbar.w = new_width;
	}
	else
	{
		healthbar.w = 0;

		//ResetProgressBar();
	}*/
}

void UI_Healthbar::ResetProgressBar()
{
	healthbar.w = 0;
	creation_complete = false;
}
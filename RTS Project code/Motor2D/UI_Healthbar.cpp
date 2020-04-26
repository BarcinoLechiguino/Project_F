#include "Application.h"
#include "Window.h"
#include "Player.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Static_Object.h"
#include "Gui.h"

#include "UI_Healthbar.h"

#include "Textures.h"


UI_Healthbar::UI_Healthbar(UI_ELEMENT element, int x, int y, bool isVisible, SDL_Rect* healthbar, SDL_Rect* background, Entity* attached_unit, bool is_progress_bar, Module* listener, UI* parent)
	: UI(element, x, y, *healthbar, listener, parent)
{
	progress_timer = 0.f;
	progress_complete = false;

	tex = App->gui->GetAtlas();
	
	this->isVisible = isVisible;
	this->attached_unit = attached_unit;

	if (this->isInteractible)													//If the Healthbar element is interactible.
	{
		this->listener = listener;												//This Healthbar's listener is set to the App->gui module (For OnCallEvent()).
	}

	if (parent != nullptr)															//If a parent is passed as argument.
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

	this->is_progress_bar = is_progress_bar;

	if (is_progress_bar)
	{
		progress_complete = false;
		progress_timer = 0.0f;
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

	if (!is_progress_bar)
	{
		if (attached_unit->current_health != attached_unit->max_health || attached_unit->is_selected || App->player->god_mode)
		{
			BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &background, 1.0f, 1.0f);
			BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &healthbar, 1.0f, 1.0f);
		}
	}
	else
	{
		if (!progress_complete)
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

void UI_Healthbar::UpdateHealthbarValue()
{
	int new_width = 0;
	
	if (!is_progress_bar)
	{
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
	}
	else
	{
		if (!progress_complete)
		{
			if (App->entity_manager->IsBuilding(attached_unit))
			{
				progress_timer += App->GetDt();

				Static_Object* building = (Static_Object*)attached_unit;

				new_width = (MAX_BUILDING_HEALTHBAR_WIDTH * progress_timer) / building->unit_creation_time;

				if (progress_timer >= building->unit_creation_time)
				{
					progress_complete = true;
					progress_timer = 0.0f;
				}
			}
		}
		else
		{
			healthbar.w = 0;
			
			//ResetProgressBar();
		}
	}

	healthbar.w = new_width;
}

void UI_Healthbar::ResetProgressBar()
{
	healthbar.w = 0;
	progress_complete = false;
}
#include "Application.h"
#include "Window.h"
#include "Entity.h"
#include "Gui.h"

#include "UI_Healthbar.h"

#include "Textures.h"


UI_Healthbar::UI_Healthbar(UI_ELEMENT element, int x, int y, bool isVisible, SDL_Rect* healthbar, SDL_Rect* background, Entity* attached_unit, Module* listener, UI* parent) 
	: UI(element, x, y, *healthbar, listener, parent)
{
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
}

UI_Healthbar::~UI_Healthbar()
{

}

bool UI_Healthbar::Draw()
{
	CheckInput();
	
	UpdateHealthbarPosition();

	BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &background, 1.0f, 1.0f);
	BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &healthbar, 1.0f, 1.0f);
	
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
	int position_x = attached_unit->pixel_position.x + attached_unit->healthbar_position_offset.x;
	int position_y = attached_unit->pixel_position.y + attached_unit->healthbar_position_offset.y;

	SetScreenPos({ position_x, position_y });

	SetHitbox({ position_x, position_y, GetHitbox().w, GetHitbox().h });
}

void UI_Healthbar::UpdateHealthbarValue()
{
	int new_width = (MAX_UNIT_HEALTHBAR_WIDTH * attached_unit->current_health) / attached_unit->max_health;

	//attached_unit->healthbar_rect.w = new_width;

	healthbar.w = new_width;
}
#include "UI_Healthbar.h"
#include "Gui.h"

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
	
	BlitElement(tex, attached_unit->pixel_position.x, attached_unit->pixel_position.y - 6, &background, 1.0f, 1.0f);	//Magic Number
	BlitElement(tex, attached_unit->pixel_position.x, attached_unit->pixel_position.y - 6, &healthbar, 1.0f, 1.0f);		//Magic Number
	
	return true;
}

void UI_Healthbar::CheckInput()
{

}

void UI_Healthbar::CleanUp()
{

}

void UI_Healthbar::ModifyHealthbar()
{
	int new_width = (MAX_UNIT_HEALTHBAR_WIDTH * attached_unit->current_health) / attached_unit->max_health;

	//attached_unit->healthbar_rect.w = new_width;

	healthbar.w = new_width;
}
#include "UI_Lifebar.h"

UI_Lifebar::UI_Lifebar(UI_Element element, int x, int y, SDL_Rect hitbox, bool isVisible, Module* listener, Entity* attached_unit, UI* parent) : UI(element, x, y, hitbox, listener, parent)
{

}

UI_Lifebar::~UI_Lifebar()
{

}

bool UI_Lifebar::Draw()
{
	return true;
}

void UI_Lifebar::CheckInput()
{

}

void UI_Lifebar::CleanUp()
{

}
#include "Application.h"
#include "Player.h"
#include "EntityManager.h"
//#include "Entity.h"
#include "StaticObject.h"
#include "GuiManager.h"

#include "UI_CreationBar.h"

UI_CreationBar::UI_CreationBar(UI_ELEMENT element, int x, int y, bool is_visible, SDL_Rect* creation_bar, SDL_Rect* background, Entity* attached_entity) 
	: UI(element, x, y, *creation_bar, nullptr, nullptr)
{
	tex = App->gui_manager->GetAtlas();

	this->is_visible = is_visible;
	this->attached_entity = attached_entity;

	if (creation_bar != nullptr)
	{
		this->creation_bar = *creation_bar;
	}

	if (background != nullptr)
	{
		this->background = *background;
	}

	this->creation_bar.w = 0;

	creation_time = 0.0f;
	accumulated_time = 0.0f;

	creation_finished = false;
}

UI_CreationBar::UI_CreationBar() : tex(nullptr), creation_bar({ 0, 0, 0, 0 }), background({ 0, 0, 0, 0 }), attached_entity(nullptr)
									, creation_time(0.0f), accumulated_time(0.0f), creation_finished(false)
{

}

bool UI_CreationBar::Draw()
{
	CheckInput();

	BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &background, 1.0f, 1.0f);
	BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &creation_bar, 1.0f, 1.0f);
	
	return true;
}

void UI_CreationBar::CheckInput()
{

}

void UI_CreationBar::CleanUp()
{
	tex = nullptr;
}

void UI_CreationBar::UpdateCreationBarValue()
{
	accumulated_time += App->GetDt();

	if (!creation_finished)
	{
		if (accumulated_time <= creation_time)
		{
			creation_bar.w = (int)((MAX_CREATION_BAR_WIDTH * accumulated_time) / creation_time);
		}
		else
		{
			creation_finished	= true;
			accumulated_time	= 0.0f;
		}
	}
	else
	{
		ResetCreationBar();
	}

	//creation_bar.w = new_width;
}

void UI_CreationBar::SetNewCreationTime(float new_creation_time)
{
	creation_time = new_creation_time;
	creation_finished = false;
}

void UI_CreationBar::ResetCreationBar()
{
	creation_bar.w = 0;
	creation_finished = false;
}
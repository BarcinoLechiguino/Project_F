#include "Application.h"
#include "Input.h"
#include "Scene.h"
#include "Main_Menu.h"

Main_Menu::Main_Menu()
{
	name = ("mainmenu");
}

// Destructor
Main_Menu::~Main_Menu()
{

}

// Called before render is available
bool Main_Menu::Awake(pugi::xml_node&)
{
	return true;
}

// Called before the first frame
bool Main_Menu::Start()
{


	return true;
}

// Called before all Updates
bool Main_Menu::PreUpdate()
{
	//Load Scene unload Main Menu
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
	{
		App->scene->Enable();
		Disable();
	}

	//play_button = App->gui->CreateButton(UI_Element::BUTTON,100,100,true,true,false,nullptr,)

	return true;
}

// Called each loop iteration
bool Main_Menu::Update(float dt)
{
	return true;
}

// Called before all Updates
bool Main_Menu::PostUpdate()
{
	return true;
}

// Called before quitting
bool Main_Menu::CleanUp()
{
	return true;
}
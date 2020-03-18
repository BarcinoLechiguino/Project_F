#include "Main_Menu.h"
#include "j1Input.h"
#include "j1App.h"
#include "j1Scene.h"

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
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
	{
		App->scene->Enable();
		Disable();
	}

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
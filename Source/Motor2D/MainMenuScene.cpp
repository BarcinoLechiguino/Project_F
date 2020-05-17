#include "Dependencies/SDL/include/SDL_rect.h"

#include "Log.h"

#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Fonts.h"
#include "Input.h"
#include "GuiManager.h"
#include "GuiElement.h"
#include "GuiImage.h"
#include "GuiText.h"
#include "GuiButton.h"
#include "GuiScrollbar.h"
#include "SceneManager.h"
#include "TransitionManager.h"

#include "MainMenuScene.h"


MainMenuScene::MainMenuScene() : Scene(SCENES::MAIN_MENU_SCENE)
{
	name_tag = ("mainmenu");
}

MainMenuScene::~MainMenuScene()
{

}

bool MainMenuScene::Awake(pugi::xml_node&)
{
	return true;
}

bool MainMenuScene::Start()
{	
	InitScene();

	LOG("Main menu scene start");

	App->gui_manager->CreateSlideAnimation(new_game_button, 0.6f, false, iPoint(new_game_button->GetScreenPos().x, 1000), iPoint(new_game_button->GetScreenPos().x, 274));

	App->gui_manager->CreateSlideAnimation(continue_button, 0.7f, false, iPoint(continue_button->GetScreenPos().x, 1200), iPoint(continue_button->GetScreenPos().x, 306));
	
	App->gui_manager->CreateSlideAnimation(options_button, 0.8f, false, iPoint(options_button->GetScreenPos().x, 1400), iPoint(options_button->GetScreenPos().x, 336));
	
	App->gui_manager->CreateSlideAnimation(exit_button, 0.9f, false, iPoint(exit_button->GetScreenPos().x, 1600), iPoint(exit_button->GetScreenPos().x, 366));
	
	return true;
}

bool MainMenuScene::PreUpdate()
{
	return true;
}

bool MainMenuScene::Update(float dt)
{
	//LOG("BEFORE DRAW UPDATE");

	App->render->Blit(background_texture, 0, 0, &background_rect, false, 0.0f);
	
	//LOG("AFTER DRAW UPDATE");

	return true;
}

bool MainMenuScene::PostUpdate()
{
	DebugKeys();
	
	//Load Scene / Unload MainMenuScene
	ExecuteTransition();

	ExecuteDebugTransition();
	
	return true;
}

bool MainMenuScene::CleanUp()
{
	App->tex->UnLoad(background_texture);
	
	App->gui_manager->DestroyGuiElements();
	
	return true;
}

void MainMenuScene::ExecuteTransition()
{
	if (transition_to_gameplay_scene)
	{
		if (App->pause)
		{
			App->pause = false;
		}
		
		App->transition_manager->CreateAlternatingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 30, true, false, Magenta, Black);
	}

	if (transition_to_options_scene)
	{
		if (App->pause)
		{
			App->pause = false;
		}
		
		App->transition_manager->CreateSlide(SCENES::OPTIONS_SCENE, 0.5f, true);
	}
}

void MainMenuScene::OnEventCall(GuiElement* element, GUI_EVENT ui_event)
{

	if (element == new_game_button && ui_event == GUI_EVENT::UNCLICKED)
	{
		/*iPoint mousepos;
		App->input->GetMousePosition(mousepos.x, mousepos.y);*/

		App->audio->PlayFx(App->gui_manager->new_game_button_clicked_fx,0);

		transition_to_gameplay_scene = true;
	}

	if (element == exit_button && ui_event == GUI_EVENT::UNCLICKED)
	{
		escape = false;
	}
	
	if (element == options_button && ui_event == GUI_EVENT::UNCLICKED)
	{
		App->audio->PlayFx(App->gui_manager->options_button_clicked_fx, 0);

		transition_to_options_scene = true;
	}	
}

void MainMenuScene::LoadGuiElements()
{
	//LOG("STARTED GUI LOAD");

	// Main Screen
	main_parent = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	// New Game Button
	SDL_Rect new_game_button_size = { 0, 0, 175, 28 };
	SDL_Rect new_game_button_idle = { 2, 24, 175, 28 };
	SDL_Rect new_game_button_hover = { 179, 24, 175, 28 };
	SDL_Rect new_game_button_clicked = { 357, 24, 175, 28 };

	new_game_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 420, 274, true, true, false, this, main_parent
		, &new_game_button_idle, &new_game_button_hover, &new_game_button_clicked);

	// Continue Button
	SDL_Rect continue_button_size = { 0, 0, 158, 23 };
	SDL_Rect continue_button_idle = { 1, 0, 158, 23 };
	SDL_Rect continue_button_hover = { 178, 0, 158, 23 };
	SDL_Rect continue_button_clicked = { 356, 0, 158, 23 };

	continue_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 425, 306, true, true, false, this, main_parent
		, &continue_button_idle, &continue_button_hover, &continue_button_clicked);

	// Options Button
	SDL_Rect options_button_size = { 0, 0, 133, 24 };
	SDL_Rect options_button_idle = { 1, 52, 133, 24 };
	SDL_Rect options_button_hover = { 178, 52, 133, 24 };
	SDL_Rect options_button_clicked = { 356, 52, 133, 24 };

	options_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 439, 336, true, true, false, this, main_parent
		, &options_button_idle, &options_button_hover, &options_button_clicked);

	// Exit Button
	SDL_Rect exit_button_size = { 0, 0, 74, 23 };
	SDL_Rect exit_button_idle = { 1, 77, 74, 23 };
	SDL_Rect exit_button_hover = { 178, 77, 74, 23 };
	SDL_Rect exit_button_clicked = { 356, 77, 74, 23 };

	exit_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 465, 366, true, true, false, this, main_parent
		, &exit_button_idle, &exit_button_hover, &exit_button_clicked);

	LOG("FINISHED GUI LOAD");
}

void MainMenuScene::InitScene()
{
	//LOG("INITSCENE START");
	transition_to_gameplay_scene = false;
	transition_to_options_scene = false;

	menu_song = App->audio->LoadMusic(App->config_file.child("config").child("scenes").child("music").child("main_menu_scene").attribute("path").as_string());
	App->audio->PlayMusic(menu_song, 0.0f);
	
	// BACKGROUND
	background_rect = { 0, 0, 1280, 720 };
	background_texture = App->tex->Load(App->config_file.child("config").child("gui").child("backgrounds").child("main_menu_scene").attribute("path").as_string());
	
	LoadGuiElements();

	//LOG("INITSCENE END");
}
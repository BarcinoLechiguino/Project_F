#include "SDL/include/SDL_rect.h"

#include "p2Log.h"

#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Fonts.h"
#include "Input.h"
#include "GuiManager.h"
#include "UI.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_Scrollbar.h"
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
	
	return true;
}

bool MainMenuScene::CleanUp()
{
	App->tex->UnLoad(background_texture);
	
	App->gui_manager->DestroyGuiElements();
	
	return true;
}

void MainMenuScene::LoadGuiElements()
{	
	//LOG("STARTED GUI LOAD");

	// BACKGROUND
	background_rect = { 0,0,1280,720 };
	background_texture = App->tex->Load("maps/MainMenu_background.png");
	
	// Main Screen
		main_parent = (UI_Image*)App->gui_manager->CreateImage(UI_ELEMENT::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

		// New Game Button
		SDL_Rect new_game_button_size = { 0, 0, 175, 28 };
		SDL_Rect new_game_button_idle = { 2, 24, 175, 28 };
		SDL_Rect new_game_button_hover = { 179, 24, 175, 28 };
		SDL_Rect new_game_button_clicked = { 357, 24, 175, 28 };
	
		new_game_button = (UI_Button*)App->gui_manager->CreateButton(UI_ELEMENT::BUTTON, 420, 274, true, true, false, this, main_parent
			, &new_game_button_idle, &new_game_button_hover, &new_game_button_clicked);
	
		// Continue Button
		SDL_Rect continue_button_size = { 0, 0, 158, 23 };
		SDL_Rect continue_button_idle = { 1, 0, 158, 23 };
		SDL_Rect continue_button_hover = { 178, 0, 158, 23 };
		SDL_Rect continue_button_clicked = { 356, 0, 158, 23 };
	
		continue_button = (UI_Button*)App->gui_manager->CreateButton(UI_ELEMENT::BUTTON, 425, 306, true, true, false, this, main_parent
			, &continue_button_idle, &continue_button_hover, &continue_button_clicked);
	
		// Options Button
		SDL_Rect options_button_size = { 0, 0, 133, 24 };
		SDL_Rect options_button_idle = { 1, 52, 133, 24 };
		SDL_Rect options_button_hover = { 178, 52, 133, 24 };
		SDL_Rect options_button_clicked = { 356, 52, 133, 24 };
	
		options_button = (UI_Button*)App->gui_manager->CreateButton(UI_ELEMENT::BUTTON, 439, 336, true, true, false, this, main_parent
			, &options_button_idle, &options_button_hover, &options_button_clicked);
	
		// Exit Button
		SDL_Rect exit_button_size = { 0, 0, 74, 23 };
		SDL_Rect exit_button_idle = { 1, 77, 74, 23 };
		SDL_Rect exit_button_hover = { 178, 77, 74, 23 };
		SDL_Rect exit_button_clicked = { 356, 77, 74, 23 };
	
		exit_button = (UI_Button*)App->gui_manager->CreateButton(UI_ELEMENT::BUTTON, 465, 366, true, true, false, this, main_parent
			, &exit_button_idle, &exit_button_hover, &exit_button_clicked);

		LOG("FINISHED GUI LOAD");
}

void MainMenuScene::OnEventCall(UI* element, UI_EVENT ui_event)
{

	if (element == new_game_button && ui_event == UI_EVENT::UNCLICKED)
	{
		/*App->transition_manager->CreateExpandingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 5, true, true);*/
		iPoint mousepos;
		App->input->GetMousePosition(mousepos.x, mousepos.y);
		App->transition_manager->CreateAlternatingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 30, true, false, Magenta, Black);

		App->audio->PlayFx(App->gui_manager->new_game_fx,0);
	}

	if (element == exit_button && ui_event == UI_EVENT::UNCLICKED)
	{
		escape = false;
	}
	
	if (element == options_button && ui_event == UI_EVENT::UNCLICKED)
	{
		App->audio->PlayFx(App->gui_manager->options_fx, 0);
	
		//App->gui_manager->SetElementsVisibility(main_parent, false);							// Deactivate Main menu
	
		//App->gui_manager->SetElementsVisibility(options_parent, true);							//Activate Options Menu
	
		//background_texture = App->tex->Load("maps/Options_background.png");

		App->transition_manager->CreateSlide(SCENES::OPTIONS_SCENE, 0.5f, true);
	}	
}

void MainMenuScene::ExecuteTransition()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KeyState::KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::LOGO_SCENE, 0.5f, true, 3, true, true);
	}

	// No KP_2 because we are in the 2nd scene.
	
	if (App->input->GetKey(SDL_SCANCODE_3) == KeyState::KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::OPTIONS_SCENE, 0.5f, true, 5, true, true);
	}
	
	if (App->input->GetKey(SDL_SCANCODE_4) == KeyState::KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 7, true, true);
	}
	
	if (App->input->GetKey(SDL_SCANCODE_5) == KeyState::KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::WIN_SCENE, 0.5f, true, 7, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_6) == KeyState::KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::LOSE_SCENE, 0.5f, true, 7, false, true);
	}
}

void MainMenuScene::InitScene()
{
	//LOG("INITSCENE START");

	menu_song = App->audio->LoadMusic("audio/music/Music_Menu.ogg");
	App->audio->PlayMusic(menu_song, 0.0f);
	LoadGuiElements();

	//LOG("INITSCENE END");
}
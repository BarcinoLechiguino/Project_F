#include "Application.h"
#include "Window.h"
#include "MainScene.h"
#include "Render.h"

MainScene::MainScene() : Scene(SCENES::MAIN_SCENE)
{
	name_tag = ("mainmenu");
}

MainScene::~MainScene()
{

}

bool MainScene::Awake(pugi::xml_node&)
{
	return true;
}

bool MainScene::Start()
{	
	App->gui->Start();
	
	LoadGuiElements();

	return true;
}

bool MainScene::PreUpdate()
{


	return true;
}

bool MainScene::Update(float dt)
{
	App->render->Blit(background_texture, 0, 0, &background_rect, false, 0.0f);

	return true;
}

bool MainScene::PostUpdate()
{
	//Load Scene / Unload MainScene
	ExecuteTransition();

	
	return true;
}

bool MainScene::CleanUp()
{
	App->gui->CleanUp();
	
	return true;
}

void MainScene::LoadGuiElements()
{
	// Main Screen

		// New Game Button
		SDL_Rect new_game_button_size = { 0, 0, 175, 28 };
		SDL_Rect new_game_button_idle = { 2, 24, 175, 28 };
		SDL_Rect new_game_button_hover = { 179, 24, 175, 28 };
		SDL_Rect new_game_button_clicked = { 357, 24, 175, 28 };

		new_game_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 420, 274, true, true, false, this, nullptr
			, &new_game_button_idle, &new_game_button_hover, &new_game_button_clicked);

		// Continue Button
		SDL_Rect continue_button_size = { 0, 0, 158, 23 };
		SDL_Rect continue_button_idle = { 1, 0, 158, 23 };
		SDL_Rect continue_button_hover = { 178, 0, 158, 23 };
		SDL_Rect continue_button_clicked = { 356, 0, 158, 23 };

		continue_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 425, 306, true, true, false, this, nullptr
			, &continue_button_idle, &continue_button_hover, &continue_button_clicked);

		// Options Button
		SDL_Rect options_button_size = { 0, 0, 133, 24 };
		SDL_Rect options_button_idle = { 1, 52, 133, 24 };
		SDL_Rect options_button_hover = { 178, 52, 133, 24 };
		SDL_Rect options_button_clicked = { 356, 52, 133, 24 };

		options_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 439, 336, true, true, false, this, nullptr
			, &options_button_idle, &options_button_hover, &options_button_clicked);

		// Exit Button
		SDL_Rect exit_button_size = { 0, 0, 74, 23 };
		SDL_Rect exit_button_idle = { 1, 77, 74, 23 };
		SDL_Rect exit_button_hover = { 178, 77, 74, 23 };
		SDL_Rect exit_button_clicked = { 356, 77, 74, 23 };

		exit_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 465, 366, true, true, false, this, nullptr
			, &exit_button_idle, &exit_button_hover, &exit_button_clicked);


		background_rect = { 0,0,1280,720 };
		background_texture = App->tex->Load("maps/MainMenu_background.png");

}

void MainScene::LoadOptionsMenu()
{
	// Deactivate Main menu
	new_game_button->isVisible = false;	
	exit_button->isVisible = false;
	continue_button->isVisible = false;
	options_button->isVisible = false;

	//Options Menu
	SDL_Rect text_rect = { 0, 0, 100, 20 };
	_TTF_Font* font = App->font->Load("fonts/borgsquadcond.ttf", 40);
	_TTF_Font* font2 = App->font->Load("fonts/borgsquadcond.ttf", 30);

	//Options
	std::string title_string = "Options";
	UI_Text* options_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 370, 150, text_rect, font, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, nullptr, &title_string);

	//Music
	std::string music_string = "Music";
	UI_Text* music_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 457, 255, text_rect, font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, nullptr, &music_string);

	//SFX
	std::string sfx_string = "SFX";
	UI_Text* sfx_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 486, 289, text_rect, font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, nullptr, &sfx_string);

	//Resolution
	std::string resolution_string = "RESOLUTION";
	UI_Text* resolution_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 418, 326, text_rect, font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, nullptr, &resolution_string);

	//Remapping

	//Back button
	SDL_Rect back_button_size = { 0, 0, 45, 33 };
	SDL_Rect back_button_idle = { 0, 103, 45, 33 };
	SDL_Rect back_button_hover = { 57, 103, 45, 33 };
	SDL_Rect back_button_clicked = { 114, 103, 45, 33 };

	exit_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 465, 366, true, true, false, this, nullptr
		, &back_button_idle, &back_button_hover, &back_button_clicked);

	background_texture = App->tex->Load("maps/Options_background.png");
}

void MainScene::OnEventCall(UI* element, UI_EVENT ui_event)
{

	if (element == new_game_button && ui_event == UI_EVENT::UNCLICKED)
	{
		App->transition_manager->CreateExpandingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 5, true, true);
	}

	if (element == exit_button && ui_event == UI_EVENT::UNCLICKED)
	{
		escape = false;
	}

	if (element == options_button && ui_event == UI_EVENT::UNCLICKED)
	{
		LoadOptionsMenu();
	}	
	
	if (element == back_button && ui_event == UI_EVENT::UNCLICKED)
	{
		
	}
}


void MainScene::ExecuteTransition()
{
	if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::LOGO_SCENE, 0.5f, true, 3, true, true);
	}

	// No KP_2 because we are in the 2nd scene.

	if (App->input->GetKey(SDL_SCANCODE_KP_3) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 5, true, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::WIN_SCENE, 0.5f, true, 7, true, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_KP_5) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::LOSE_SCENE, 0.5f, true, 7, false, true);
	}
}


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
	SDL_Rect new_game_button_size		= { 0, 0, 175, 28 };
	SDL_Rect new_game_button_idle		= { 2, 24, 175, 28 };
	SDL_Rect new_game_button_hover		= { 179, 24, 175, 28 };
	SDL_Rect new_game_button_clicked	= { 357, 24, 175, 28 };

	new_game_button = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON,420, 274, true, true, false, this, nullptr
													, &new_game_button_idle, &new_game_button_hover, &new_game_button_clicked);

	// Continue Button
	SDL_Rect continue_button_size = { 0, 0, 158, 23 };
	SDL_Rect continue_button_idle = { 1, 0, 158, 23 };
	SDL_Rect continue_button_hover = { 178, 0, 158, 23 };
	SDL_Rect continue_button_clicked = { 356, 0, 158, 23 };

	continue_button = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 425, 306, true, true, false, this, nullptr
		, &continue_button_idle, &continue_button_hover, &continue_button_clicked);

	// Options Button
	SDL_Rect options_button_size = { 0, 0, 133, 24 };
	SDL_Rect options_button_idle = { 1, 52, 133, 24 };
	SDL_Rect options_button_hover = { 178, 52, 133, 24 };
	SDL_Rect options_button_clicked = { 356, 52, 133, 24 };

	options_button = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 439, 336, true, true, false, this, nullptr
		, &options_button_idle, &options_button_hover, &options_button_clicked);

	// Exit Button
	SDL_Rect exit_button_size = { 0, 0, 74, 23 };
	SDL_Rect exit_button_idle = { 1, 77, 74, 23 };
	SDL_Rect exit_button_hover = { 178, 77, 74, 23 };
	SDL_Rect exit_button_clicked = { 356, 77, 74, 23 };

	exit_button = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 465, 366, true, true, false, this, nullptr
		, &exit_button_idle, &exit_button_hover, &exit_button_clicked);

	//SDL_Rect text_rect = { 0, 0, 40, 9 };
	//_TTF_Font* font = App->font->Load("fonts/Minecraftia-Regular.ttf", 12);
	//std::string button_string = "To Gameplay Scene";

	//button_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, center_button.x + text_rect.w, center_button.y + 3, text_rect, font
	//												, SDL_Color{ 255, 255, 255, 255 }, true, false, false, nullptr, transition_button, &button_string);

	background_rect = { 0,0,1280,720 };
	background_texture = App->tex->Load("maps/MainMenu_background.png");


}

void MainScene::OnEventCall(UI* element, UI_Event ui_event)
{

	if (element == new_game_button && ui_event == UI_Event::UNCLICKED)
	{
		App->transition_manager->CreateExpandingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 5, true, true);
	}

	if (element == exit_button && ui_event == UI_Event::UNCLICKED)
	{
		escape = false;
	}

	if (element == options_button && ui_event == UI_Event::UNCLICKED)
	{
		LoadOptionsMenu();
	}
}

void MainScene::LoadOptionsMenu()
{
	// Options Menu
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


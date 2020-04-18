#include "Application.h"
#include "Window.h"
#include "Fonts.h"
#include "Input.h"
#include "Gui.h"
#include "UI.h"
#include "UI_Button.h"
#include "TransitionManager.h"

#include "LoseScene.h"

LoseScene::LoseScene() : Scene(SCENES::LOSE_SCENE)
{

}

LoseScene::~LoseScene()
{

}

bool LoseScene::Awake(pugi::xml_node&)
{
	return true;
}

bool LoseScene::Start()
{
	App->gui->Start();
	
	LoadGuiElements();
	
	return true;
}

bool LoseScene::PreUpdate()
{
	return true;
}

bool LoseScene::Update(float dt)
{
	return true;
}

bool LoseScene::PostUpdate()
{
	//Load Scene / Unload LoseScene
	ExecuteTransition();
	
	return true;
}

bool LoseScene::CleanUp()
{
	App->gui->CleanUp();
	
	return true;
}

void LoseScene::LoadGuiElements()
{
	// Temporal transition button
	int win_width;
	int win_height;

	iPoint window_center;

	App->win->GetWindowSize(win_width, win_height);

	window_center = iPoint(win_width * 0.5f, win_height * 0.5f);

	SDL_Rect button_size = { 0, 0, 115, 9 };
	SDL_Rect transition_button_idle = { 618, 34, 115, 9 };
	SDL_Rect transition_button_hover = { 618, 23, 115, 9 };
	SDL_Rect transition_button_clicked = { 618, 1, 115, 9 };

	iPoint center_button = iPoint(window_center.x - button_size.w * 0.5, window_center.y - button_size.h * 0.5f);

	transition_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, center_button.x, center_button.y, true, true, false, this, nullptr
		, &transition_button_idle, &transition_button_hover, &transition_button_clicked);

	SDL_Rect text_rect = { 0, 0, 15, 8 };
	_TTF_Font* font = App->font->Load("fonts/Minecraftia-Regular.ttf", 9);
	std::string button_string = "To Main Scene";

	button_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, center_button.x + text_rect.w, center_button.y - 2, text_rect, font
		, SDL_Color{ 255, 255, 255, 255 }, true, false, false, nullptr, transition_button, &button_string);
}

void LoseScene::OnEventCall(UI* element, UI_EVENT ui_event)
{
	if (element == transition_button && ui_event == UI_EVENT::UNCLICKED)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_SCENE, 0.5f, true, 10, true, true);
	}
}

void LoseScene::ExecuteTransition()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::LOGO_SCENE, 0.5f, true, 8, true, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_SCENE, 0.5f, true, 10, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 12, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::WIN_SCENE, 0.5f, true, 12, true, true);
	}

	// No KP_5 because we are in the 5th scene.
}
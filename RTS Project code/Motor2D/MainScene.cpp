#include "Application.h"
#include "Window.h"
#include "MainScene.h"

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
	// Temporal transition button
	int win_width;
	int win_height;
	
	iPoint window_center;

	App->win->GetWindowSize(win_width, win_height);

	window_center = iPoint(win_width * 0.5f, win_height * 0.5f);

	SDL_Rect button_size				= { 0, 0, 230, 18 };
	SDL_Rect transition_button_idle		= { 0, 202, 230, 18 };
	SDL_Rect transition_button_hover	= { 0, 224, 230, 18 };
	SDL_Rect transition_button_clicked	= { 0, 158, 230, 18 };
	
	iPoint center_button = iPoint(window_center.x - button_size.w * 0.5, window_center.y - button_size.h * 0.5f);

	transition_button = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, center_button.x, center_button.y, true, true, false, this, nullptr
													, &transition_button_idle, &transition_button_hover, &transition_button_clicked);

	SDL_Rect text_rect = { 0, 0, 40, 9 };
	_TTF_Font* font = App->font->Load("fonts/Minecraftia-Regular.ttf", 12);
	std::string button_string = "To Gameplay Scene";

	button_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, center_button.x + text_rect.w, center_button.y + 3, text_rect, font
													, SDL_Color{ 255, 255, 255, 255 }, true, false, false, nullptr, transition_button, &button_string);
}

void MainScene::OnEventCall(UI* element, UI_Event ui_event)
{
	if (element == transition_button && ui_event == UI_Event::UNCLICKED)
	{
		App->transition_manager->CreateExpandingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 5, true, true);
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

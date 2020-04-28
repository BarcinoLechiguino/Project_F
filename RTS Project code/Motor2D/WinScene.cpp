#include "Application.h"
#include "Window.h"
#include "Fonts.h"
#include "Input.h"
#include "GuiManager.h"
#include "UI.h"
#include "Audio.h"
#include "UI_Button.h"
#include "SceneManager.h"
#include "TransitionManager.h"
#include "Textures.h"

#include "WinScene.h"

WinScene::WinScene() :  Scene(SCENES::WIN_SCENE)
{
	win_main_menu = nullptr;
	win_song = 0;
	background_texture = 0;
	background_rect = {0,0,0,0};
	name_tag = ("Win");
}

WinScene::~WinScene()
{

}

bool WinScene::Awake(pugi::xml_node&)
{
	return true;
}

bool WinScene::Start()
{
	LoadGuiElements();
	InitScene();
	
	return true;
}

bool WinScene::PreUpdate()
{
	return true;
}

bool WinScene::Update(float dt)
{
	App->render->Blit(background_texture, 0, 0, &background_rect, false, 0.0f);
	return true;
}

bool WinScene::PostUpdate()
{
	//Load Scene / Unload WinScene
	ExecuteTransition();
	
	return true;
}

bool WinScene::CleanUp()
{
	App->tex->UnLoad(background_texture);
	
	App->gui_manager->DestroyGuiElements();
	
	return true;
}

void WinScene::LoadGuiElements()
{
	
	background_rect = { 0,0,1280,720 };
	background_texture = App->tex->Load("maps/WinScreen.png");

	// Back to menu Button
	SDL_Rect win_back_to_menu_size = { 0, 0, 189, 23 };
	SDL_Rect win_back_to_menu_idle = { 0, 137, 189, 23 };
	SDL_Rect win_back_to_menu_hover = { 204, 137, 189, 23 };
	SDL_Rect win_back_to_menu_clicked = { 408, 137, 189, 23 };

	win_main_menu = (UI_Button*)App->gui_manager->CreateButton(UI_ELEMENT::BUTTON, 555, 621, true, true, false, this, nullptr
		, &win_back_to_menu_idle, &win_back_to_menu_hover, &win_back_to_menu_clicked);
}

void WinScene::OnEventCall(UI* element, UI_EVENT ui_event)
{
	if (element == win_main_menu && ui_event == UI_EVENT::UNCLICKED)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_MENU_SCENE, 0.5f, true, 10, true, true);
		App->audio->PlayFx(App->gui_manager->exit_fx, 0);
	}
}

void WinScene::ExecuteTransition()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_STATE::KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::LOGO_SCENE, 0.5f, true, 8, true, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_STATE::KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_MENU_SCENE, 0.5f, true, 10, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_STATE::KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::OPTIONS_SCENE, 0.5f, true, 10, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_STATE::KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 12, false, true);
	}

	// No 5 because we are in the 5th scene.

	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_STATE::KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::WIN_SCENE, 0.5f, true, 12, true, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_6) == KEY_STATE::KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::LOSE_SCENE, 0.5f, true, 12, true, true);
	}
}

void WinScene::InitScene()
{
	win_song = App->audio->LoadMusic("audio/music/Victory_Song.ogg");
	App->audio->PlayMusic(win_song, 0);
}
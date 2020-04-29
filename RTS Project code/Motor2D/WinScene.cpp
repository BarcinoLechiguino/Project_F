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

	ExecuteDebugTransition();
	
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
	// BACKGROUND
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
		App->audio->PlayFx(App->gui_manager->exit_fx, 0);

		transition_to_main_menu_scene = true;
	}
}

void WinScene::ExecuteTransition()
{
	if (transition_to_main_menu_scene)
	{
		if (App->pause)
		{
			App->pause = false;
		}

		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_MENU_SCENE, 0.5f, true, 10, true, true);
	}
}

void WinScene::InitScene()
{
	transition_to_main_menu_scene = false;
	
	win_song = App->audio->LoadMusic("audio/music/Victory_Song.ogg");
	App->audio->PlayMusic(win_song, 0);
}
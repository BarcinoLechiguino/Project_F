#include "Application.h"
#include "Window.h"
#include "Fonts.h"
#include "Input.h"
#include "GuiManager.h"
#include "UI.h"
#include "UI_Button.h"
#include "SceneManager.h"
#include "TransitionManager.h"
#include "Textures.h"
#include "Audio.h"
#include "p2Log.h"
#include "LogoScene.h"

LogoScene::LogoScene() : Scene(SCENES::LOGO_SCENE), accumulated_time(0.f), background_rect({0,0,0,0}),background_texture(nullptr),button_text(nullptr),logo_channel(0),logo_fx(0),logo_scene_duration(0.f),transition_button(nullptr)
{
	
}

LogoScene::~LogoScene()
{

}

bool LogoScene::Awake(pugi::xml_node&)
{
	return true;
}

bool LogoScene::Start()
{
	InitScene();	

	LOG("Logo scene start");

	return true;	//crash
}

bool LogoScene::PreUpdate()
{
	return true;
}

bool LogoScene::Update(float dt)
{
	App->render->Blit(background_texture, 0, 0, &background_rect, false, 0.0f);
	return true;
}

bool LogoScene::PostUpdate()
{
	//Load Scene / Unload LogoScene
	ExecuteTransition();


	return true;
}

bool LogoScene::CleanUp()
{
	App->tex->UnLoad(background_texture);

	Mix_FadeOutChannel(logo_channel, 500); //crash

	App->gui_manager->DestroyGuiElements();

	return true;
}

void LogoScene::LoadGuiElements()
{
	
}

void LogoScene::OnEventCall(UI* element, UI_EVENT ui_event)
{

}

void LogoScene::ExecuteTransition()
{
	accumulated_time += App->GetDt();

	if (accumulated_time >= logo_scene_duration)
	{
		App->transition_manager->CreateFadeToColour(SCENES::MAIN_MENU_SCENE);
		accumulated_time = 0.0f;
	}
	
	// No KP_1 because we are in the 1rst scene.

	if (App->input->GetKey(SDL_SCANCODE_2) == KeyState::KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::MAIN_MENU_SCENE, 0.5f, true, 3, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KeyState::KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::OPTIONS_SCENE, 0.5f, true, 5, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KeyState::KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 7, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_5) == KeyState::KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::WIN_SCENE, 0.5f, true, 7, true, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_6) == KeyState::KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::LOSE_SCENE, 0.5f, true, 7, true, true);
	}
}

void LogoScene::InitScene()
{
	//logo_fx = App->audio->LoadFx("audio/fx/LogoScreen_sfx.wav");
	//logo_channel = App->audio->PlayFx(logo_fx, 0);

	logo_scene_duration = 5.0f;
	accumulated_time	= 0.0f;

	App->gui_manager->Start();

	background_rect = { 0,0,1280,720 };
	background_texture = App->tex->Load("maps/MissingmdScreen.png");

	//LoadGuiElements();
}
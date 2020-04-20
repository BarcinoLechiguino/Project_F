#include "Application.h"
#include "Window.h"
#include "Fonts.h"
#include "Input.h"
#include "Gui.h"
#include "UI.h"
#include "UI_Button.h"
#include "SceneManager.h"
#include "TransitionManager.h"
#include "Textures.h"
#include "Audio.h"

#include "LogoScene.h"

LogoScene::LogoScene() : Scene(SCENES::LOGO_SCENE)
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

	return true;
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
	Mix_FadeOutChannel(logo_channel, 500);
	App->gui->CleanUp();

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
		App->transition_manager->CreateFadeToColour(SCENES::MAIN_SCENE);
		accumulated_time = 0.0f;
	}
	
	// No KP_1 because we are in the 1rst scene.

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::MAIN_SCENE, 0.5f, true, 3, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 5, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::WIN_SCENE, 0.5f, true, 7, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::LOSE_SCENE, 0.5f, true, 7, true, true);
	}
}

void LogoScene::InitScene()
{
	logo_fx = App->audio->LoadFx("audio/fx/LogoScreen_sfx.wav");
	logo_channel = App->audio->PlayFx(logo_fx, 0);

	logo_scene_duration = 5.0f;
	accumulated_time	= 0.0f;

	App->gui->Start();

	background_rect = { 0,0,1280,720 };
	background_texture = App->tex->Load("maps/MissingmdScreen.png");

	//LoadGuiElements();
}
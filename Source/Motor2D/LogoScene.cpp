#include "Log.h"

#include "Application.h"
#include "Window.h"
#include "Fonts.h"
#include "Input.h"
#include "GuiManager.h"
#include "GuiElement.h"
#include "GuiButton.h"
#include "SceneManager.h"
#include "TransitionManager.h"
#include "Textures.h"
#include "Audio.h"
#include "LogoScene.h"

LogoScene::LogoScene() : Scene(SCENES::LOGO_SCENE)
, accumulated_time(0.f)
, background_rect({ 0, 0, 0, 0 })
, background_texture(nullptr)
, logo_texture(nullptr)
, logo_channel(0)
, logo_fx(0)
, logo_scene_duration(0.f)
{
	name_tag = ("Logo");
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
	SDL_SetTextureAlphaMod(logo_texture, accumulated_time * 300.0f);

	App->render->Blit(background_texture, 0, 0, &background_rect, false, 0.0f);
	App->render->Blit(logo_texture, 513, 166, &logo_rect, false, 0.0f);
	return true;
}

bool LogoScene::PostUpdate()
{
	//Load Scene / Unload LogoScene
	ExecuteTransition();

	ExecuteDebugTransition();

	return true;
}

bool LogoScene::CleanUp()
{
	App->tex->UnLoad(background_texture);

	Mix_FadeOutChannel(logo_channel, 500); //crash

	App->gui_manager->DestroyGuiElements();

	return true;
}

void LogoScene::ExecuteTransition()
{
	if (App->pause)
	{
		App->pause = false;
	}
	
	accumulated_time += App->GetDt();

	if (accumulated_time >= logo_scene_duration)
	{
		App->transition_manager->CreateFadeToColour(SCENES::MAIN_MENU_SCENE);
		accumulated_time = 0.0f;
	}
}

void LogoScene::OnEventCall(GuiElement* element, GUI_EVENT ui_event)
{

}

void LogoScene::LoadGuiElements()
{

}

void LogoScene::InitScene()
{
	logo_fx = App->audio->LoadFx(App->config_file.child("config").child("scenes").child("fx").child("logo_scene_background_noise").attribute("path").as_string());
	logo_channel = App->audio->PlayFx(logo_fx, 0);

	logo_scene_duration = 5.0f;
	accumulated_time	= 0.0f;

	App->gui_manager->Start();

	// BACKGROUND
	background_rect = { 0, 0, 1280, 720 };
	background_texture = App->tex->Load(App->config_file.child("config").child("gui").child("backgrounds").child("logo_scene").attribute("path").as_string());
	
	// LOGO
	logo_rect = { 0, 0, 256, 337 };
	logo_texture = App->tex->Load(App->config_file.child("config").child("gui").child("backgrounds").child("logo_scene_team_logo").attribute("path").as_string());

	//LoadGuiElements();
}
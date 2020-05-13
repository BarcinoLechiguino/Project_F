#include "Application.h"
#include "Window.h"
#include "Fonts.h"
#include "Input.h"
#include "GuiManager.h"
#include "Audio.h"
#include "UI.h"
#include "UI_Button.h"
#include "SceneManager.h"
#include "TransitionManager.h"
#include "Textures.h"

#include "LoseScene.h"

LoseScene::LoseScene() : Scene(SCENES::LOSE_SCENE),background_rect({0,0,0,0}),background_texture(nullptr),lose_main_menu(nullptr),lose_song(0)
{
	name_tag = ("Lose");
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
	LoadGuiElements();
	
	InitScene();

	return true;
}

bool LoseScene::PreUpdate()
{
	return true;
}

bool LoseScene::Update(float dt)
{
	App->render->Blit(background_texture, 0, 0, &background_rect, false, 0.0f);
	return true;
}

bool LoseScene::PostUpdate()
{
	//Load Scene / Unload LoseScene
	ExecuteTransition();

	ExecuteDebugTransition();
	
	return true;
}

bool LoseScene::CleanUp()
{
	App->tex->UnLoad(background_texture);
	
	App->gui_manager->DestroyGuiElements();
	
	return true;
}

void LoseScene::LoadGuiElements()
{
	// BACKGROUND
	background_rect = { 0,0,1280,720 };
	background_texture = App->tex->Load("maps/DefeatScreen.png");

	// Back to menu Button
	SDL_Rect lose_back_to_menu_size = { 0, 0, 189, 23 };
	SDL_Rect lose_back_to_menu_idle = { 0, 137, 189, 23 };
	SDL_Rect lose_back_to_menu_hover = { 204, 137, 189, 23 };
	SDL_Rect lose_back_to_menu_clicked = { 408, 137, 189, 23 };

	lose_main_menu = (UI_Button*)App->gui_manager->CreateButton(UI_ELEMENT::BUTTON, 555, 621, true, true, false, this, nullptr
		, &lose_back_to_menu_idle, &lose_back_to_menu_hover, &lose_back_to_menu_clicked);
}

void LoseScene::OnEventCall(UI* element, UI_EVENT ui_event)
{
	if (element == lose_main_menu && ui_event == UI_EVENT::UNCLICKED)
	{
		App->audio->PlayFx(App->gui_manager->exit_fx, 0);

		transition_to_main_menu_scene = true;
	}
}

void LoseScene::ExecuteTransition()
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

void LoseScene::InitScene()
{
	transition_to_main_menu_scene = false;
	
	lose_song = App->audio->LoadMusic("audio/music/Lose_Song.ogg");
	App->audio->PlayMusic(lose_song, 0);
}
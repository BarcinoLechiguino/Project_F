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
		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_MENU_SCENE, 0.5f, true, 10, true, true);
		App->audio->PlayFx(App->gui_manager->exit_fx, 0);
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
		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_MENU_SCENE, 0.5f, true, 10, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::OPTIONS_SCENE, 0.5f, true, 10, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 12, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::WIN_SCENE, 0.5f, true, 12, true, true);
	}

	// No 6 because we are in the 6th scene.
}

void LoseScene::InitScene()
{
	lose_song = App->audio->LoadMusic("audio/music/Lose_Song.ogg");
	App->audio->PlayMusic(lose_song, 0);
}
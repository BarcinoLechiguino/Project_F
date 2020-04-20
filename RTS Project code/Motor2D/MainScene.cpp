#include "SDL/include/SDL_rect.h"

#include "p2Log.h"

#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Fonts.h"
#include "Input.h"
#include "Gui.h"
#include "UI.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_Scrollbar.h"
#include "SceneManager.h"
#include "TransitionManager.h"

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
	InitScene();

	return true;
}

bool MainScene::PreUpdate()
{


	return true;
}

bool MainScene::Update(float dt)
{
	App->render->Blit(background_texture, 0, 0, &background_rect, false, 0.0f);

	AdjustVolumeWithScrollbar();

	return true;
}

bool MainScene::PostUpdate()
{
	DebugKeys();
	
	//Load Scene / Unload MainScene
	ExecuteTransition();
	
	return true;
}

bool MainScene::CleanUp()
{
	App->gui->CleanUp();
	
	App->tex->UnLoad(background_texture);

	return true;
}

void MainScene::LoadGuiElements()
{	
	// Main Screen
		main_parent = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

		// New Game Button
		SDL_Rect new_game_button_size = { 0, 0, 175, 28 };
		SDL_Rect new_game_button_idle = { 2, 24, 175, 28 };
		SDL_Rect new_game_button_hover = { 179, 24, 175, 28 };
		SDL_Rect new_game_button_clicked = { 357, 24, 175, 28 };

		new_game_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 420, 274, true, true, false, this, main_parent
			, &new_game_button_idle, &new_game_button_hover, &new_game_button_clicked);

		// Continue Button
		SDL_Rect continue_button_size = { 0, 0, 158, 23 };
		SDL_Rect continue_button_idle = { 1, 0, 158, 23 };
		SDL_Rect continue_button_hover = { 178, 0, 158, 23 };
		SDL_Rect continue_button_clicked = { 356, 0, 158, 23 };

		continue_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 425, 306, true, true, false, this, main_parent
			, &continue_button_idle, &continue_button_hover, &continue_button_clicked);

		// Options Button
		SDL_Rect options_button_size = { 0, 0, 133, 24 };
		SDL_Rect options_button_idle = { 1, 52, 133, 24 };
		SDL_Rect options_button_hover = { 178, 52, 133, 24 };
		SDL_Rect options_button_clicked = { 356, 52, 133, 24 };

		options_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 439, 336, true, true, false, this, main_parent
			, &options_button_idle, &options_button_hover, &options_button_clicked);

		// Exit Button
		SDL_Rect exit_button_size = { 0, 0, 74, 23 };
		SDL_Rect exit_button_idle = { 1, 77, 74, 23 };
		SDL_Rect exit_button_hover = { 178, 77, 74, 23 };
		SDL_Rect exit_button_clicked = { 356, 77, 74, 23 };

		exit_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 465, 366, true, true, false, this, main_parent
			, &exit_button_idle, &exit_button_hover, &exit_button_clicked);


		background_rect = { 0,0,1280,720 };
		background_texture = App->tex->Load("maps/MainMenu_background.png");

	// Options Menu
		LoadOptionsMenu();
		App->gui->SetElementsVisibility(options_parent, false);

}

void MainScene::LoadOptionsMenu()
{

	//Options Menu
	SDL_Rect text_rect = { 0, 0, 100, 20 };
	_TTF_Font* font = App->font->Load("fonts/borgsquadcond.ttf", 40);
	_TTF_Font* font2 = App->font->Load("fonts/borgsquadcond.ttf", 30);
	options_parent = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::EMPTY, 0, 0, SDL_Rect{0,0,1,1});

	//Options
	std::string title_string = "Options";
	options_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 370, 150, text_rect, font, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, options_parent, &title_string);

	//Music
	std::string music_string = "Music";
	music_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 457, 255, text_rect, font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, options_parent, &music_string);
	
	SDL_Rect scrollbar_rect = {743,3,180,15};
	SDL_Rect thumb_rect = { 930,2,18,31 };
	iPoint thumb_offset = { 20, -7 };
	SDL_Rect drag_area = { 0, 0, 180, 15 };
	float drag_factor = 0.2f;

	//MUSIC
	music_scrollbar = (UI_Scrollbar*)App->gui->CreateScrollbar(UI_ELEMENT::SCROLLBAR, 570, 260, scrollbar_rect, thumb_rect, thumb_offset
																, drag_area, drag_factor, true, false, false, false, false, false);
	music_scrollbar->parent = options_parent;

	//SFX
	std::string sfx_string = "SFX";
	sfx_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 486, 289, text_rect, font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, options_parent, &sfx_string);
	sfx_scrollbar = (UI_Scrollbar*)App->gui->CreateScrollbar(UI_ELEMENT::SCROLLBAR, 570, 300, scrollbar_rect, thumb_rect, thumb_offset
																, drag_area, drag_factor, true, false, false, false, false, false);
	sfx_scrollbar->parent = options_parent;

	//screen size
	std::string resolution_string = "screen";
	resolution_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 418, 326, text_rect, font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, options_parent, &resolution_string);

	//Remapping

	//Back button
	SDL_Rect back_button_size = { 0, 0, 45, 33 };
	SDL_Rect back_button_idle = { 0, 103, 45, 33 };
	SDL_Rect back_button_hover = { 57, 103, 45, 33 };
	SDL_Rect back_button_clicked = { 114, 103, 45, 33 };

	back_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 400, 470, true, true, false, this, options_parent
		, &back_button_idle, &back_button_hover, &back_button_clicked);
}

void MainScene::OnEventCall(UI* element, UI_EVENT ui_event)
{

	if (element == new_game_button && ui_event == UI_EVENT::UNCLICKED)
	{
		App->transition_manager->CreateExpandingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 5, true, true);
		App->audio->PlayFx(App->gui->new_game_fx,0);
	}

	if (element == exit_button && ui_event == UI_EVENT::UNCLICKED)
	{
		escape = false;
	}

	if (element == options_button && ui_event == UI_EVENT::UNCLICKED)
	{
		App->audio->PlayFx(App->gui->options_fx, 0);

		App->gui->SetElementsVisibility(main_parent, false);							// Deactivate Main menu

		App->gui->SetElementsVisibility(options_parent, true);							//Activate Options Menu

		background_texture = App->tex->Load("maps/Options_background.png");
	}	
	
	if (element == back_button && ui_event == UI_EVENT::UNCLICKED)
	{
		App->audio->PlayFx(App->gui->back_fx, 0);

		App->gui->SetElementsVisibility(main_parent, true);							// Activate Main menu

		App->gui->SetElementsVisibility(options_parent, false);						//Deactivate Options Menu

		background_texture = App->tex->Load("maps/MainMenu_background.png");
	}
}

void MainScene::AdjustVolumeWithScrollbar()
{	
	//UpdateVolumeThumbPosition();
	//UpdateFXVolumeThumbPosition();
	
	// --- Audio Scrollbars
	if (music_scrollbar->isVisible)
	{	
		float local_thumb_pos = music_scrollbar->GetThumbHitbox().x - music_scrollbar->GetHitbox().x;

		float offset = local_thumb_pos / music_scrollbar->GetHitbox().w;										// Value from 0.0f to 1.0f

		App->audio->volume = offset * 100;																		// Will make the offset a valid value to modify the volume.
	}

	if (sfx_scrollbar->isVisible)
	{
		float local_thumb_pos = sfx_scrollbar->GetThumbHitbox().x - sfx_scrollbar->GetHitbox().x;

		float start_offset = local_thumb_pos / sfx_scrollbar->GetHitbox().w;									// Value from 0.0f to 1.0f

		uint offset = floor(start_offset * 100);																// Will make the offset a valid value to modify the volume.					

		App->audio->volume_fx = offset;
	}
}

void MainScene::UpdateVolumeThumbPosition()
{
	float current = App->audio->volume * 0.01f;

	float local_thumb_pos = current * music_scrollbar->GetHitbox().w;

	int world_thumb_pos = floor(local_thumb_pos + music_scrollbar->GetHitbox().x);

	music_scrollbar->SetHitbox({ world_thumb_pos, music_scrollbar->GetHitbox().y, music_scrollbar->GetHitbox().w, music_scrollbar->GetHitbox().h });
}

void MainScene::UpdateFXVolumeThumbPosition()
{
	float current = App->audio->volume_fx * 0.01f;

	float local_thumb_pos = current * sfx_scrollbar->GetHitbox().w;

	int world_thumb_pos = floor(local_thumb_pos + sfx_scrollbar->GetHitbox().x);

	sfx_scrollbar->SetThumbHitbox({ world_thumb_pos, sfx_scrollbar->GetHitbox().y, sfx_scrollbar->GetHitbox().w, sfx_scrollbar->GetHitbox().h });

	const uint tmp = 10;
	
	std::string casual = { "My House %d", tmp };

	LOG("%s", casual.c_str());
}

void MainScene::ExecuteTransition()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::LOGO_SCENE, 0.5f, true, 3, true, true);
	}

	// No KP_2 because we are in the 2nd scene.

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 5, true, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::WIN_SCENE, 0.5f, true, 7, true, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::LOSE_SCENE, 0.5f, true, 7, false, true);
	}
}

void MainScene::InitScene()
{
	App->gui->Start();

	LoadGuiElements();
}
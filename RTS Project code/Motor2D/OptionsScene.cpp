#include "SDL/include/SDL_rect.h"

#include "p2Log.h"

#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Fonts.h"
#include "Input.h"
#include "GuiManager.h"
#include "UI.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_Scrollbar.h"
#include "SceneManager.h"
#include "TransitionManager.h"

#include "OptionsScene.h"


OptionsScene::OptionsScene() : Scene(SCENES::OPTIONS_SCENE)
{
	name_tag = ("optionsmenu");
}

OptionsScene::~OptionsScene()
{

}

bool OptionsScene::Awake(pugi::xml_node&)
{
	return true;
}

bool OptionsScene::Start()
{
	InitScene();

	return true;
}

bool OptionsScene::PreUpdate()
{


	return true;
}

bool OptionsScene::Update(float dt)
{
	//LOG("BEFORE DRAW UPDATE");

	App->render->Blit(background_texture, 0, 0, &background_rect, false, 0.0f);

	//LOG("AFTER DRAW UPDATE");

	AdjustVolumeWithScrollbar();

	return true;
}

bool OptionsScene::PostUpdate()
{
	DebugKeys();

	//Load Scene / Unload MainMenuScene
	ExecuteTransition();

	return true;
}

bool OptionsScene::CleanUp()
{
	App->tex->UnLoad(background_texture);

	App->gui_manager->DestroyGuiElements();

	return true;
}

void OptionsScene::LoadGuiElements()
{
	LOG("STARTED GUI LOAD");
	// BACKGROUND
	background_rect = { 0, 0, 1280, 720 };
	background_texture = App->tex->Load("maps/Options_Background.png");
	
	//Options Menu
	SDL_Rect text_rect = { 0, 0, 100, 20 };
	_TTF_Font* font = App->font->Load("fonts/borgsquadcond.ttf", 40);
	_TTF_Font* font2 = App->font->Load("fonts/borgsquadcond.ttf", 30);
	options_parent = (UI_Image*)App->gui_manager->CreateImage(UI_ELEMENT::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	//Options
	std::string title_string = "Options";
	options_text = (UI_Text*)App->gui_manager->CreateText(UI_ELEMENT::TEXT, 370, 150, text_rect, font, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, options_parent, &title_string);

	//Music
	std::string music_string = "Music";
	music_text = (UI_Text*)App->gui_manager->CreateText(UI_ELEMENT::TEXT, 457, 255, text_rect, font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, options_parent, &music_string);

	SDL_Rect scrollbar_rect = { 743,3,180,15 };
	SDL_Rect thumb_rect = { 930,2,18,31 };
	iPoint thumb_offset = { 20, -7 };
	SDL_Rect drag_area = { 0, 0, 180, 15 };
	float drag_factor = 0.2f;

	//MUSIC
	music_scrollbar = (UI_Scrollbar*)App->gui_manager->CreateScrollbar(UI_ELEMENT::SCROLLBAR, 570, 260, scrollbar_rect, thumb_rect, thumb_offset
		, drag_area, drag_factor, true, false, false, true, false, false);
	music_scrollbar->parent = options_parent;

	//SFX
	std::string sfx_string = "SFX";
	sfx_text = (UI_Text*)App->gui_manager->CreateText(UI_ELEMENT::TEXT, 486, 289, text_rect, font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, options_parent, &sfx_string);
	sfx_scrollbar = (UI_Scrollbar*)App->gui_manager->CreateScrollbar(UI_ELEMENT::SCROLLBAR, 570, 300, scrollbar_rect, thumb_rect, thumb_offset
		, drag_area, drag_factor, true, false, false, true, false, false);
	sfx_scrollbar->parent = options_parent;

	//screen size
	std::string resolution_string = "fullscreen";
	resolution_text = (UI_Text*)App->gui_manager->CreateText(UI_ELEMENT::TEXT, 380, 326, text_rect, font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, options_parent, &resolution_string);

	// Fullscreen options

	SDL_Rect opt_fullscreen_off_size = { 0, 0, 29, 24 };
	SDL_Rect opt_fullscreen_off_idle = { 744, 18, 29, 24 };
	SDL_Rect opt_fullscreen_off_hover = { 744, 18, 29, 24 };
	SDL_Rect opt_fullscreen_off_clicked = { 744, 18, 29, 24 };

	fullscreen_off = (UI_Button*)App->gui_manager->CreateButton(UI_ELEMENT::BUTTON, 580, 334, true, true, false, this, options_parent
		, &opt_fullscreen_off_idle, &opt_fullscreen_off_hover, &opt_fullscreen_off_clicked);

	SDL_Rect opt_fullscreen_on_size = { 0, 0, 29, 24 };
	SDL_Rect opt_fullscreen_on_idle = { 788, 18, 29, 24 };
	SDL_Rect opt_fullscreen_on_hover = { 788, 18, 29, 24 };
	SDL_Rect opt_fullscreen_on_clicked = { 788, 18, 29, 24 };

	fullscreen_on = (UI_Button*)App->gui_manager->CreateButton(UI_ELEMENT::BUTTON, 580, 334, false, true, false, this, nullptr
		, &opt_fullscreen_on_idle, &opt_fullscreen_on_hover, &opt_fullscreen_on_clicked);

	//Remapping

	//Back button
	SDL_Rect back_button_size = { 0, 0, 45, 33 };
	SDL_Rect back_button_idle = { 0, 103, 45, 33 };
	SDL_Rect back_button_hover = { 57, 103, 45, 33 };
	SDL_Rect back_button_clicked = { 114, 103, 45, 33 };

	back_button = (UI_Button*)App->gui_manager->CreateButton(UI_ELEMENT::BUTTON, 400, 470, true, true, false, this, options_parent
		, &back_button_idle, &back_button_hover, &back_button_clicked);
	LOG("FINISHED GUI LOAD");
}

void OptionsScene::OnEventCall(UI* element, UI_EVENT ui_event)
{
	if (element == back_button && ui_event == UI_EVENT::UNCLICKED)
	{
		App->audio->PlayFx(App->gui_manager->back_fx, 0);

		App->transition_manager->CreateSlide(SCENES::MAIN_MENU_SCENE, 0.5f, true, false, true);
	}

	if (element == fullscreen_off && ui_event == UI_EVENT::UNCLICKED)
	{
		App->gui_manager->SetElementsVisibility(fullscreen_on, true);
		App->gui_manager->SetElementsVisibility(fullscreen_off, false);
	}

	if (element == fullscreen_on && ui_event == UI_EVENT::UNCLICKED)
	{
		App->win->ToggleFullscreen();
		App->gui_manager->SetElementsVisibility(fullscreen_on, false);
		App->gui_manager->SetElementsVisibility(fullscreen_off, true);
	}

}

void OptionsScene::AdjustVolumeWithScrollbar()
{
	//UpdateVolumeThumbPosition();
	//UpdateFXVolumeThumbPosition();

	if (music_scrollbar != nullptr)
	{
		float local_thumb_pos = music_scrollbar->GetThumbHitbox().x - music_scrollbar->GetHitbox().x;

		float offset = local_thumb_pos / music_scrollbar->GetHitbox().w;										// Value from 0.0f to 1.0f

		App->audio->volume = offset * 100;																		// Will make the offset a valid value to modify the volume.
	}

	if (sfx_scrollbar != nullptr)
	{
		float local_thumb_pos = sfx_scrollbar->GetThumbHitbox().x - sfx_scrollbar->GetHitbox().x;

		float start_offset = local_thumb_pos / sfx_scrollbar->GetHitbox().w;									// Value from 0.0f to 1.0f

		uint offset = floor(start_offset * 100);																// Will make the offset a valid value to modify the volume.					

		App->audio->volume_fx = offset;
	}
}

void OptionsScene::UpdateVolumeThumbPosition()
{
	float current = App->audio->volume * 0.01f;

	float local_thumb_pos = current * music_scrollbar->GetHitbox().w;

	int world_thumb_pos = floor(local_thumb_pos + music_scrollbar->GetHitbox().x);

	music_scrollbar->SetHitbox({ world_thumb_pos, music_scrollbar->GetHitbox().y, music_scrollbar->GetHitbox().w, music_scrollbar->GetHitbox().h });
}

void OptionsScene::UpdateFXVolumeThumbPosition()
{
	float current = App->audio->volume_fx * 0.01f;

	float local_thumb_pos = current * sfx_scrollbar->GetHitbox().w;

	int world_thumb_pos = floor(local_thumb_pos + sfx_scrollbar->GetHitbox().x);

	sfx_scrollbar->SetThumbHitbox({ world_thumb_pos, sfx_scrollbar->GetHitbox().y, sfx_scrollbar->GetHitbox().w, sfx_scrollbar->GetHitbox().h });

	const uint tmp = 10;

	std::string casual = { "My House %d", tmp };

	LOG("%s", casual.c_str());
}

void OptionsScene::ExecuteTransition()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::LOGO_SCENE, 0.5f, true, 3, true, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::MAIN_MENU_SCENE, 0.5f, true, 5, true, true);
	}

	// No KP_3 because we are in the 3rd scene.

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 7, true, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::WIN_SCENE, 0.5f, true, 7, false, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
	{
		App->transition_manager->CreateExpandingBars(SCENES::LOSE_SCENE, 0.5f, true, 7, false, true);
	}
}

void OptionsScene::InitScene()
{
	LOG("INITSCENE START");

	menu_song = App->audio->LoadMusic("audio/music/Music_Menu.ogg");
	App->audio->PlayMusic(menu_song, 0.0f);
	LoadGuiElements();

	LOG("INITSCENE END");
}
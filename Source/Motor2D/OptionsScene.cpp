#include "Dependencies/SDL/include/SDL_rect.h"

#include "Log.h"

#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Fonts.h"
#include "Input.h"
#include "GuiManager.h"
#include "GuiElement.h"
#include "GuiImage.h"
#include "GuiText.h"
#include "GuiButton.h"
#include "GuiScrollbar.h"
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

	ExecuteDebugTransition();

	return true;
}

bool OptionsScene::CleanUp()
{
	App->tex->UnLoad(background_texture);

	App->gui_manager->DestroyGuiElements();

	return true;
}

void OptionsScene::ExecuteTransition()
{
	if (transition_to_main_menu_scene)
	{
		if (App->pause)
		{
			App->pause = false;
		}
		
		App->transition_manager->CreateSlide(SCENES::MAIN_MENU_SCENE, 0.5f, true, false, true);
	}
}

void OptionsScene::OnEventCall(GuiElement* element, GUI_EVENT ui_event)
{
	if (element == back_button && ui_event == GUI_EVENT::UNCLICKED)
	{
		App->audio->PlayFx(App->gui_manager->back_button_clicked_fx, 0);

		transition_to_main_menu_scene = true;
	}

	if (element == fullscreen_off && ui_event == GUI_EVENT::UNCLICKED)
	{
		App->gui_manager->SetElementsVisibility(fullscreen_on, true);
		App->gui_manager->SetElementsVisibility(fullscreen_off, false);
	}

	if (element == fullscreen_on && ui_event == GUI_EVENT::UNCLICKED)
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
		float local_thumb_pos = (float)music_scrollbar->GetThumbHitbox().x - (float)music_scrollbar->GetHitbox().x;

		float offset = local_thumb_pos / music_scrollbar->GetHitbox().w;										// Value from 0.0f to 1.0f

		App->audio->volume = offset * 100;																		// Will make the offset a valid value to modify the volume.
	}

	if (sfx_scrollbar != nullptr)
	{
		float local_thumb_pos = (float)sfx_scrollbar->GetThumbHitbox().x - (float)sfx_scrollbar->GetHitbox().x;

		float start_offset = local_thumb_pos / sfx_scrollbar->GetHitbox().w;									// Value from 0.0f to 1.0f

		uint offset = (int)floor(start_offset * 100.f);																// Will make the offset a valid value to modify the volume.					

		App->audio->volume_fx = offset;
	}
}

void OptionsScene::UpdateVolumeThumbPosition()
{
	float current = App->audio->volume * 0.01f;

	float local_thumb_pos = current * music_scrollbar->GetHitbox().w;

	int world_thumb_pos = (int)floor(local_thumb_pos + (float)music_scrollbar->GetHitbox().x);

	music_scrollbar->SetHitbox({ world_thumb_pos, music_scrollbar->GetHitbox().y, music_scrollbar->GetHitbox().w, music_scrollbar->GetHitbox().h });
}

void OptionsScene::UpdateFXVolumeThumbPosition()
{
	float current = App->audio->volume_fx * 0.01f;

	float local_thumb_pos = current * sfx_scrollbar->GetHitbox().w;

	int world_thumb_pos = (int)floor(local_thumb_pos + (float)sfx_scrollbar->GetHitbox().x);

	sfx_scrollbar->SetThumbHitbox({ world_thumb_pos, sfx_scrollbar->GetHitbox().y, sfx_scrollbar->GetHitbox().w, sfx_scrollbar->GetHitbox().h });

	const uint tmp = 10;

	std::string casual = { "My House %d", tmp };

	LOG("%s", casual.c_str());
}

void OptionsScene::LoadGuiElements()
{
	LOG("STARTED GUI LOAD");
	// BACKGROUND
	background_rect = { 0, 0, 1280, 720 };
	background_texture = App->tex->Load(App->config_file.child("config").child("gui").child("backgrounds").child("options_scene").attribute("path").as_string());

	std::string borgsquadcond = App->config_file.child("config").child("gui").child("fonts").child("borgsquadcond").attribute("path").as_string();

	//Options Menu
	SDL_Rect text_rect = { 0, 0, 100, 20 };
	_TTF_Font* font = App->font->Load(borgsquadcond.c_str(), 40);
	_TTF_Font* font2 = App->font->Load(borgsquadcond.c_str(), 30);
	options_parent = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	//Options
	std::string title_string = "Options";
	options_text = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 370, 150, text_rect, font, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, options_parent, &title_string);

	//Music
	std::string music_string = "Music";
	music_text = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 457, 255, text_rect, font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, options_parent, &music_string);

	SDL_Rect scrollbar_rect = { 743,3,180,15 };
	SDL_Rect thumb_rect = { 930,2,18,31 };
	iPoint thumb_offset = { 20, -7 };
	SDL_Rect drag_area = { 0, 0, 180, 15 };
	float drag_factor = 0.2f;

	//MUSIC
	music_scrollbar = (GuiScrollbar*)App->gui_manager->CreateScrollbar(GUI_ELEMENT_TYPE::SCROLLBAR, 570, 260, scrollbar_rect, thumb_rect, thumb_offset
		, drag_area, drag_factor, true, false, false, true, false, false);
	music_scrollbar->parent = options_parent;

	//SFX
	std::string sfx_string = "SFX";
	sfx_text = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 486, 289, text_rect, font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, options_parent, &sfx_string);
	sfx_scrollbar = (GuiScrollbar*)App->gui_manager->CreateScrollbar(GUI_ELEMENT_TYPE::SCROLLBAR, 570, 300, scrollbar_rect, thumb_rect, thumb_offset
		, drag_area, drag_factor, true, false, false, true, false, false);
	sfx_scrollbar->parent = options_parent;

	//screen size
	std::string resolution_string = "fullscreen";
	resolution_text = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 380, 326, text_rect, font2, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, options_parent, &resolution_string);

	// Fullscreen options

	SDL_Rect opt_fullscreen_off_size = { 0, 0, 29, 24 };
	SDL_Rect opt_fullscreen_off_idle = { 744, 18, 29, 24 };
	SDL_Rect opt_fullscreen_off_hover = { 744, 18, 29, 24 };
	SDL_Rect opt_fullscreen_off_clicked = { 744, 18, 29, 24 };

	fullscreen_off = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 580, 334, true, true, false, this, options_parent
		, &opt_fullscreen_off_idle, &opt_fullscreen_off_hover, &opt_fullscreen_off_clicked);

	SDL_Rect opt_fullscreen_on_size = { 0, 0, 29, 24 };
	SDL_Rect opt_fullscreen_on_idle = { 788, 18, 29, 24 };
	SDL_Rect opt_fullscreen_on_hover = { 788, 18, 29, 24 };
	SDL_Rect opt_fullscreen_on_clicked = { 788, 18, 29, 24 };

	fullscreen_on = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 580, 334, false, true, false, this, nullptr
		, &opt_fullscreen_on_idle, &opt_fullscreen_on_hover, &opt_fullscreen_on_clicked);

	//Remapping

	//Back button
	SDL_Rect back_button_size = { 0, 0, 45, 33 };
	SDL_Rect back_button_idle = { 0, 103, 45, 33 };
	SDL_Rect back_button_hover = { 57, 103, 45, 33 };
	SDL_Rect back_button_clicked = { 114, 103, 45, 33 };

	back_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 400, 470, true, true, false, this, options_parent
		, &back_button_idle, &back_button_hover, &back_button_clicked);
	LOG("FINISHED GUI LOAD");
}

void OptionsScene::InitScene()
{
	LOG("INITSCENE START");

	transition_to_main_menu_scene = false;
	
	menu_song = App->audio->LoadMusic(App->config_file.child("config").child("scenes").child("music").child("main_menu_scene").attribute("path").as_string());
	App->audio->PlayMusic(menu_song, 0.0f);
	LoadGuiElements();

	LOG("INITSCENE END");
}
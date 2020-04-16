#include "Brofiler\Brofiler.h"
//#include "mmgr/mmgr.h"

#include "p2Log.h"
#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Input.h"
#include "Audio.h"
#include "Collisions.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Console.h"
#include "Player.h"
#include "Scene.h"

#include "EntityManager.h"
#include "Entity.h"

#include "Gui.h"
#include "UI.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_Image.h"

#include "SceneManager.h"
#include "TransitionManager.h"

#include "GameplayScene.h"


GameplayScene::GameplayScene() : Scene(SCENES::GAMEPLAY_SCENE)
{
	name_tag = ("scene");
}

// Destructor
GameplayScene::~GameplayScene()
{}

// Called before render is available
bool GameplayScene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");

	bool ret = true;
	
	/*for (pugi::xml_node map = config.child("map_name"); map; map = map.next_sibling("map_name"))
	{
		std::string data;							
	
		data = (map.attribute("name").as_string());
		map_names.push_back(data);
	}*/

	music_path = (config.child("audio").attribute("path").as_string());
	music_path2=(config.child("audio2").attribute("path").as_string());
	music_path3=(config.child("audio3").attribute("path").as_string());
	
	return ret;
}

// Called before the first frame
bool GameplayScene::Start()
{
	bool ret = false;

	InitScene();
	
	return ret;
}

// Called each loop iteration
bool GameplayScene::PreUpdate()
{
	// debug pathfing ------------------
	if (App->map->pathfindingMetaDebug == true)
	{
		PathfindingDebug();
	}
	
	return true;
}

// Called each loop iteration
bool GameplayScene::Update(float dt)														//Receives dt as an argument.
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::LavenderBlush);
	
	App->render->Blit(background_texture, 0, 0, &background_rect, false, 0.0f);

	App->map->Draw();																//Map Draw
	
	DrawOccupied();

	DrawEntityMapDebug();

	if (App->map->pathfindingMetaDebug == true)
	{
		DrawPathfindingDebug();														//Pathfinding Debug. Shows a debug texture on the path's tiles.
	}

	//LOG("Rocks %d", rock_test.size());

	return true;
}


// Called each loop iteration
bool GameplayScene::PostUpdate()
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::LavenderBlush);
	bool ret = true;

	DebugKeys();																	//Debug Keys
	UnitDebugKeys();

	CameraDebugMovement(App->GetDt());

	//Transition To Any Scene. Load Scene / Unload GameplayScene
	ExecuteTransition();
	
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		App->gui->SetElementsVisibility(in_game_background, !in_game_background->isVisible);
		App->audio->PlayFx(App->gui->appear_menu_fx, 0);
	}

	return ret;
}

// Called before quitting
bool GameplayScene::CleanUp()
{
	App->tex->UnLoad(background_texture);
	App->tex->UnLoad(occupied_debug);
	App->tex->UnLoad(occupied_by_entity_debug);
	App->tex->UnLoad(path_debug_tex);

	App->collisions->CleanUp();								//Deletes all colliders that were loaded for this scene / map.
	App->entity_manager->DestroyEntities();					//Destroys all non-player entities.
	App->map->CleanUp();									//Deletes everything related with the map from memory. (Tilesets, Layers and ObjectGroups)
	App->gui->CleanUp();
	
	return true;
}

void GameplayScene::InitScene()
{
	bool ret = true;
	
	//cam_debug_speed = App->render->cam.camera_debug_speed;				//Sets the camera speed in debug mode.

	App->gui->Start();

	to_end = false;

	ret = App->map->Load("New_Tilesete_Map.tmx");

	if (ret)
	{
		SetWalkabilityMap();
		SetEntitiesMap();
	}

	path_debug_tex = App->tex->Load("maps/path2.png");

	//test background
	background_rect = { 0,0,1280,720 };
	background_texture = App->tex->Load("maps/hacker_background.png");

	LoadGuiElements();

	occupied_debug = App->tex->Load("maps/occupied_tile.png");
	occupied_by_entity_debug = App->tex->Load("maps/occupied_by_entity_tile.png");

	//App->audio->PlayMusic(App->scene->music_path2.c_str());
}

void GameplayScene::SetWalkabilityMap()
{
	int w, h;
	uchar* data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &data))
	{
		App->pathfinding->SetMap(w, h, data);						//Sets a new walkability map with the map passed by CreateWalkabilityMap().
	}

	RELEASE_ARRAY(data);											//Frees all memory allocated to the data array.
}

void GameplayScene::SetEntitiesMap()
{
	App->entity_manager->SetEntityMap(App->map->data.width, App->map->data.height);
}

void GameplayScene::LoadGuiElements()
{
	// Temporal transition buttons
	SDL_Rect button_size = { 0, 0, 115, 9 };
	SDL_Rect transition_button_idle = { 618, 34, 115, 9 };
	SDL_Rect transition_button_hover = { 618, 23, 115, 9 };
	SDL_Rect transition_button_clicked = { 618, 1, 115, 9 };

	transition_button		= (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 100, 680, true, true, false, this, nullptr
																, &transition_button_idle, &transition_button_hover, &transition_button_clicked);

	transition_button_II	= (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 350, 680, true, true, false, this, nullptr
																, &transition_button_idle, &transition_button_hover, &transition_button_clicked);

	transition_button_III	= (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 100, 50, true, true, false, this, nullptr
																, &transition_button_idle, &transition_button_hover, &transition_button_clicked);

	// Temporal transition buttons' texts
	SDL_Rect text_rect = { 0, 0, 15, 8 };
	_TTF_Font* font = App->font->Load("fonts/Minecraftia-Regular.ttf", 9);

	std::string win_button_string	= "To Win Screen";
	std::string lose_button_string	= "To Lose Screen";
	std::string main_button_string	= "To Main Screen";

	button_text			= (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 121, 678, text_rect, font, SDL_Color{ 255, 255, 255, 255 }
														, true, false, false, nullptr, transition_button, &win_button_string);

	button_text_II		= (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 366, 678, text_rect, font, SDL_Color{ 255, 255, 255, 255 }
														, true, false, false, nullptr, transition_button_II, &lose_button_string);

	button_text_III		= (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 121, 48, text_rect, font, SDL_Color{ 255, 255, 255, 255 }
														, true, false, false, nullptr, transition_button_III, &main_button_string);


	// In-game menu

	// Back
	SDL_Rect background = { 780, 451, 514, 403 };
	in_game_background = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 350, 180, background, false, false, false, this, nullptr);


	// Continue Button
	SDL_Rect in_game_continue_button_size = { 0, 0, 158, 23 };
	SDL_Rect in_game_continue_button_idle = { 1, 0, 158, 23 };
	SDL_Rect in_game_continue_button_hover = { 178, 0, 158, 23 };
	SDL_Rect in_game_continue_button_clicked = { 356, 0, 158, 23 };

	in_game_continue_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 525, 286, false, true, false, this, in_game_background
		, &in_game_continue_button_idle, &in_game_continue_button_hover, &in_game_continue_button_clicked);


	// Options Button
	SDL_Rect in_game_options_button_size = { 0, 0, 133, 24 };
	SDL_Rect in_game_options_button_idle = { 1, 52, 133, 24 };
	SDL_Rect in_game_options_button_hover = { 178, 52, 133, 24 };
	SDL_Rect in_game_options_button_clicked = { 356, 52, 133, 24 };

	in_game_options_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 537, 326, false, true, false, this, in_game_background
		, &in_game_options_button_idle, &in_game_options_button_hover, &in_game_options_button_clicked);


	// Back to menu Button
	SDL_Rect in_game_back_to_menu_size = { 0, 0, 189, 23 };
	SDL_Rect in_game_back_to_menu_idle = { 0, 137, 189, 23 };
	SDL_Rect in_game_back_to_menu_hover = { 204, 137, 189, 23 };
	SDL_Rect in_game_back_to_menu_clicked = { 408, 137, 189, 23 };

	in_game_back_to_menu = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 511, 366, false, true, false, this, in_game_background
		, &in_game_back_to_menu_idle, &in_game_back_to_menu_hover, &in_game_back_to_menu_clicked);


	// Exit Button
	SDL_Rect in_game_exit_button_size = { 0, 0, 74, 23 };
	SDL_Rect in_game_exit_button_idle = { 1, 77, 74, 23 };
	SDL_Rect in_game_exit_button_hover = { 178, 77, 74, 23 };
	SDL_Rect in_game_exit_button_clicked = { 356, 77, 74, 23 };

	in_game_exit_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 566, 406, false, true, false, this, in_game_background
		, &in_game_exit_button_idle, &in_game_exit_button_hover, &in_game_exit_button_clicked);

	
	// Title
	SDL_Rect in_game_text_rect = { 0, 0, 100, 20 };
	_TTF_Font* in_game_font = App->font->Load("fonts/borgsquadcond.ttf", 50);
	std::string in_game_title_string = "Pause Menu";
	in_game_title_text = (UI_Text*)App->gui->CreateText(UI_ELEMENT::TEXT, 439, 210, in_game_text_rect, in_game_font, SDL_Color{ 255,255,0,0 }, false, false, false, this, in_game_background, &in_game_title_string);


	// HUD

	// Up Bar
	// Group Up
	SDL_Rect HUD_group_button_size = { 0, 0, 63, 38 };
	SDL_Rect HUD_group_button_idle = { 759, 48, 63, 38 };
	SDL_Rect HUD_group_button_hover = { 970, 48, 63, 38 };
	SDL_Rect HUD_group_button_clicked = { 1171, 48, 63, 38 };

	HUD_group_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 489, -4, true, true, false, this, nullptr
		, &HUD_group_button_idle, &HUD_group_button_hover, &HUD_group_button_clicked);

	// Pause
	SDL_Rect HUD_pause_button_size = { 0, 0, 63, 38 };
	SDL_Rect HUD_pause_button_idle = { 825, 48, 63, 38 };
	SDL_Rect HUD_pause_button_hover = { 1036, 48, 63, 38 };
	SDL_Rect HUD_pause_button_clicked = { 1237, 48, 63, 38 };

	HUD_pause_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 546, -4, true, true, false, this, nullptr
		, &HUD_pause_button_idle, &HUD_pause_button_hover, &HUD_pause_button_clicked);

	// Home 
	SDL_Rect HUD_home_button_size = { 0, 0, 63, 37 };
	SDL_Rect HUD_home_button_idle = { 890, 48, 63, 37 };
	SDL_Rect HUD_home_button_hover = { 1101, 48, 63, 37 };
	SDL_Rect HUD_home_button_clicked = { 1302, 48, 63, 37 };

	HUD_home_button = (UI_Button*)App->gui->CreateButton(UI_ELEMENT::BUTTON, 602, -4, true, true, false, this, nullptr
		, &HUD_home_button_idle, &HUD_home_button_hover, &HUD_home_button_clicked);


	//Down Bar
	//Main Bar
	SDL_Rect HUD_main_down_bar_size = { 685, 226, 707, 211 };

	HUD_main_down_bar = (UI_Image*)App->gui->CreateImage(UI_ELEMENT::IMAGE, 230, 514, HUD_main_down_bar_size, true, true, false, this, nullptr);

	//Resource Bar

}

void GameplayScene::OnEventCall(UI* element, UI_EVENT ui_event)
{
	if (element == transition_button && ui_event == UI_EVENT::UNCLICKED)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::WIN_SCENE, 0.5f, true, 5, true, true);
	}

	if (element == transition_button_II && ui_event == UI_EVENT::UNCLICKED)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::LOSE_SCENE, 0.5f, true, 5, true, true);
	}

	if (element == transition_button_III && ui_event == UI_EVENT::UNCLICKED)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_SCENE, 0.5f, true, 10, false, true);
	}

	// In_game menu

	if (element == in_game_continue_button && ui_event == UI_EVENT::UNCLICKED)
	{
		// Continue
		App->gui->SetElementsVisibility(in_game_background, false);
		App->audio->PlayFx(App->gui->new_game_fx, 0);
	}

	if (element == in_game_options_button && ui_event == UI_EVENT::UNCLICKED)
	{
		// Options
		App->audio->PlayFx(App->gui->options_fx, 0);
	}

	if (element == in_game_back_to_menu && ui_event == UI_EVENT::UNCLICKED)
	{
		// Back to menu
		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_SCENE, 0.5f, true, 10, false, true);
		App->audio->PlayFx(App->gui->exit_fx, 0);
	}

	if (element == in_game_exit_button && ui_event == UI_EVENT::UNCLICKED)
	{
		// Exit
		escape = false;
	}

	
	// HUD

	if (element == HUD_group_button && ui_event == UI_EVENT::UNCLICKED)
	{
		// Group
		App->audio->PlayFx(App->gui->standard_fx, 0);
	}

	if (element == HUD_pause_button && ui_event == UI_EVENT::UNCLICKED)
	{
		// Pause
		App->audio->PlayFx(App->gui->standard_fx, 0);
	}

	if (element == HUD_home_button && ui_event == UI_EVENT::UNCLICKED)
	{
		// Home
		App->audio->PlayFx(App->gui->standard_fx, 0);
	}

}

void GameplayScene::ExecuteTransition()
{
	if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::LOGO_SCENE, 0.5f, true, 8, true, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_SCENE, 0.5f, true, 10, false, true);
	}

	// No KP_3 because we are in the 3rd scene.

	if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::WIN_SCENE, 0.5f, true, 12, true, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_KP_5) == KEY_DOWN)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::LOSE_SCENE, 0.5f, true, 12, false, true);
	}
}

void GameplayScene::UnitDebugKeys()
{
	if (App->scene_manager->god_mode)
	{
		if (App->pathfinding->IsWalkable(iPoint(App->player->mouse_tile.x, App->player->mouse_tile.y)))
		{
			if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
			{
				(Gatherer*)App->entity_manager->CreateEntity(ENTITY_TYPE::GATHERER, App->player->mouse_tile.x, App->player->mouse_tile.y);
			}
			if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
			{
				(Barracks*)App->entity_manager->CreateEntity(ENTITY_TYPE::BARRACKS, App->player->mouse_tile.x, App->player->mouse_tile.y);
			}
			if (App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
			{
				(TownHall*)App->entity_manager->CreateEntity(ENTITY_TYPE::TOWNHALL, App->player->mouse_tile.x, App->player->mouse_tile.y);
			}
			if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			{
				(Enemy*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY, App->player->mouse_tile.x, App->player->mouse_tile.y);
			}
			if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
			{
				(Infantry*)App->entity_manager->CreateEntity(ENTITY_TYPE::INFANTRY, App->player->mouse_tile.x, App->player->mouse_tile.y);
			}
			if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			{
				(Rock*)App->entity_manager->CreateEntity(ENTITY_TYPE::ROCK, App->player->mouse_tile.x, App->player->mouse_tile.y);
			}
		}
	}
}

void GameplayScene::PathfindingDebug()
{
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			LOG("Tiles selected: (%d, %d) and (%d, %d)", origin.x, origin.y, p.x, p.y);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}
}

void GameplayScene::DrawPathfindingDebug()
{
	// Draw pathfinding debug tiles ------------------------------
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(path_debug_tex, p.x, p.y);								//Should we want it, we could make a separate texture called mouse_debug_tex so the tex at mouse pos and the tex at path tile are different.

	const std::vector<iPoint> path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path.size(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path.at(i).x, path.at(i).y);		//Both work, reach a consensus on which to use.
		//iPoint pos = App->map->MapToWorld((*path)[i].x, (*path)[i].y);

		App->render->Blit(path_debug_tex, pos.x, pos.y);
	}
}

void GameplayScene::DrawOccupied()
{
	for (int x = 0; x < App->map->data.width; ++x) //Magic
	{
		for (int y = 0; y < App->map->data.height; ++y)
		{
			if (App->pathfinding->IsOccupied(iPoint(x, y)))
			{
				iPoint draw_position = App->map->MapToWorld(x, y);
				App->render->Blit(occupied_debug, draw_position.x, draw_position.y, nullptr);
			}
		}
	}
}

void GameplayScene::DrawEntityMapDebug()
{
	for (int x = 0; x < App->map->data.width; ++x) //Magic
	{
		for (int y = 0; y < App->map->data.height; ++y)
		{
			if (App->entity_manager->entity_map[(y * App->map->data.width) + x] != nullptr)
			{
				iPoint draw_position = App->map->MapToWorld(x, y);
				App->render->Blit(occupied_by_entity_debug, draw_position.x, draw_position.y, nullptr);
			}
		}
	}
}

// --------------- REVISE IF THEY ARE NEEDED ---------------
//bool Scene1::Load(pugi::xml_node& data)
//{
//	if (currentMap != data.child("currentMap").attribute("num").as_int())
//	{
//		LOG("Calling switch maps");
//		currentMap = data.child("currentMap").attribute("num").as_int();
//
//		//std::list<std::string>::iterator map_iterator = map_names.begin();
//
//		//std::advance(map_iterator, data.child("currentMap").attribute("num").as_int() );
//
//		//App->map->SwitchMaps( (*map_iterator) );
//	}
//	return true;
//}

//bool Scene1::Save(pugi::xml_node& data) const
//{
//	data.append_child("currentMap").append_attribute("num") = currentMap;
//	return true;
//}
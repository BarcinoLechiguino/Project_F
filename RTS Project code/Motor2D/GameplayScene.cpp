#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "Console.h"
#include "Collisions.h"
#include "Pathfinding.h"
#include "Player.h"
#include "GameplayScene.h"
#include "Scene.h"

#include "Brofiler\Brofiler.h"

#include "EntityManager.h"

//#include "mmgr/mmgr.h"

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

	App->gui->Start();
	
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
		//ret = false;
	}
	
	//if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
	//{
	//	Mix_HaltMusic();
	//	App->gui->SetElementsVisibility(main_in_menu2, !main_in_menu2->isVisible);
	//	//App->audio->PlayMusic(App->scene->music_path3.c_str());
	//	if (!main_in_menu2->isVisible)
	//	{
	//		App->pause = false;
	//		
	//	}
	//}
	
	return ret;
}

// Called before quitting
bool GameplayScene::CleanUp()
{
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
	
	to_end = false;
	
	ret = App->map->Load("New_Tilesete_Map.tmx");
	
	if (ret)
	{
		int w, h;
		uchar* data = nullptr;
		if (App->map->CreateWalkabilityMap(w, h, &data))
		{
			App->pathfinding->SetMap(w, h, data);						//Sets a new walkability map with the map passed by CreateWalkabilityMap().
		}
		RELEASE_ARRAY(data);											//Frees all memory allocated to the data array.
	}
	
	path_debug_tex = App->tex->Load("maps/path2.png");
	
	//test background
	background_rect = { 0,0,1280,720 };
	background_texture = App->tex->Load("maps/hacker_background.png");
	
	LoadGuiElements();
	
	occupied_debug = App->tex->Load("maps/occupied_tile.png");
	
	//App->audio->PlayMusic(App->scene->music_path2.c_str());
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
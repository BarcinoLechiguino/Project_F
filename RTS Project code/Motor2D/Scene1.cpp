#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "Main_Menu.h"
#include "Console.h"
#include "Collisions.h"
#include "Gui.h"
#include "Pathfinding.h"
#include "Player.h"
#include "Scene1.h"
#include "Scene.h"

#include "Brofiler\Brofiler.h"

#include "EntityManager.h"

//#include "mmgr/mmgr.h"

Scene1::Scene1() : Scene(SCENES::NONE)
{
	name_tag = ("scene");
}

// Destructor
Scene1::~Scene1()
{}

// Called before render is available
bool Scene1::Awake(pugi::xml_node& config)
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
bool Scene1::Start()
{
	bool ret = false;

	InitScene();
	App->gui->Start();
	
	return ret;
}

// Called each loop iteration
bool Scene1::PreUpdate()
{
	// debug pathfing ------------------
	if (App->map->pathfindingMetaDebug == true)
	{
		PathfindingDebug();
	}
	
	return true;
}

// Called each loop iteration
bool Scene1::Update(float dt)														//Receives dt as an argument.
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::LavenderBlush);
	
	App->render->Blit(background_texture, 0, 0, &background_rect, false, 0.0f);

	App->map->Draw();																//Map Draw
	
	if (App->map->pathfindingMetaDebug == true)
	{
		DrawPathfindingDebug();														//Pathfinding Debug. Shows a debug texture on the path's tiles.
	}

	//LOG("Rocks %d", rock_test.size());

	return true;
}


// Called each loop iteration
bool Scene1::PostUpdate()
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::LavenderBlush);
	bool ret = true;

	DebugKeys();																	//Debug Keys
	UnitDebugKeys();

	CameraDebugMovement(App->GetDt());

	//Transition MainMenu
	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	{
		ExecuteTransition();
	}

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		//ret = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
	{
		Mix_HaltMusic();
		App->gui->SetElementsVisibility(main_in_menu2, !main_in_menu2->isVisible);
		//App->audio->PlayMusic(App->scene->music_path3.c_str());
		if (!main_in_menu2->isVisible)
		{
			App->pause = false;
			
		}
	}
	
	return ret;
}

// Called before quitting
bool Scene1::CleanUp()
{
	App->collisions->CleanUp();								//Deletes all colliders that were loaded for this scene / map.
	App->entityManager->DestroyEntities();					//Destroys all non-player entities.
	App->map->CleanUp();									//Deletes everything related with the map from memory. (Tilesets, Layers and ObjectGroups)
	App->gui->CleanUp();
	
	return true;
}

void Scene1::InitScene()
{
	bool ret = true;
	
	//cam_debug_speed = App->render->cam.camera_debug_speed;				//Sets the camera speed in debug mode.

	to_end = false;

	ret = App->map->Load("New_Tilesete_Map.tmx");

	if (ret)
	{
		int w, h;
		uchar* data = NULL;
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
	
	//test entity
	/*rock_test.push_back((Rock*)App->entityManager->CreateEntity(ENTITY_TYPE::ROCK,2, 2));
	enemy_test.push_back((Enemy*)App->entityManager->CreateEntity(ENTITY_TYPE::ENEMY, 10, 10));
	gatherer_test.push_back((Gatherer*)App->entityManager->CreateEntity(ENTITY_TYPE::GATHERER, 5, 5));
	gatherer_test.push_back((Gatherer*)App->entityManager->CreateEntity(ENTITY_TYPE::GATHERER, 10, 5));
	gatherer_test.push_back((Gatherer*)App->entityManager->CreateEntity(ENTITY_TYPE::GATHERER, 8, 7));
	gatherer_test.push_back((Gatherer*)App->entityManager->CreateEntity(ENTITY_TYPE::GATHERER, 7, 8));
	gatherer_test.push_back((Gatherer*)App->entityManager->CreateEntity(ENTITY_TYPE::GATHERER, 9, 7));
	gatherer_test.push_back((Gatherer*)App->entityManager->CreateEntity(ENTITY_TYPE::GATHERER, 7, 9));
	town_hall_test.push_back((TownHall*)App->entityManager->CreateEntity(ENTITY_TYPE::TOWNHALL, 0,0));
	barracks_test.push_back((Barracks*)App->entityManager->CreateEntity(ENTITY_TYPE::BARRACKS, 5, 2));*/

	//rock_test.back()->entity_sprite = App->tex->Load("maps/debug_tile.png");

	LoadGuiElements();

	//App->audio->PlayMusic(App->scene->music_path2.c_str());
}

void Scene1::LoadGuiElements()
{
	//In-game menu
	SDL_Rect main_in_rect{ 0, 388, 466, 447 };
	main_in_menu2 = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 280, 180, main_in_rect, false, false, false, nullptr, nullptr);
	
	SDL_Rect label1_rect_in{ 1078,242,382,61 };
	label_1_in = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 323, 160, label1_rect_in, false, false, false, nullptr,nullptr, main_in_menu2);
	
	_TTF_Font* font_in_esc = App->font->Load("fonts/Minecraftia-Regular.ttf", 32);
	std::string string_title_in = "PAUSE MENU";
	//in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 410, 166, textHitbox, font_in_esc, fontRgb, false, false, false, main_in_menu, &string_title_in);
	
	SDL_Rect in_rect = { 1057,30,173,45 };
	SDL_Rect idle_in = { 1057,30,173,45 };
	SDL_Rect hover_in = { 1057,76,173,45 };
	SDL_Rect clicked_in = { 1057,126,173,45 };
	in_buttons_resume = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 430, 247, false, true, false, this, main_in_menu2, &idle_in, &hover_in, &clicked_in);
	in_buttons_save = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 430, 317, false, true, false, this, main_in_menu2, &idle_in, &hover_in, &clicked_in);
	in_buttons_load = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 430, 387, false, true, false, this, main_in_menu2, &idle_in, &hover_in, &clicked_in);
	in_buttons_exit = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 430, 457, false, true, false, this, main_in_menu2, &idle_in, &hover_in, &clicked_in);
	
	SDL_Rect unmute_rect_in = { 512,147,57,57 };
	SDL_Rect idle_u_in = { 512,147,57,57 };
	SDL_Rect hover_u_in = { 512,267,57,57 };
	SDL_Rect clicked_u_in = { 1479,72,57,57 };
	unmute_in = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 670, 550, false, true, false, this, main_in_menu2, &idle_u_in, &hover_u_in, &clicked_u_in);
	SDL_Rect mute_rect_in = { 1479, 9, 57, 57 };
	SDL_Rect idle_m_in = { 1479, 9, 57, 57 };
	SDL_Rect hover_m_in = { 1544,9,57,57 };
	SDL_Rect clicked_m_in = { 440,214,57,57 };
	mute_in = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 300, 550, false, true, false, this, main_in_menu2, &idle_m_in, &hover_m_in, &clicked_m_in);
	
	_TTF_Font* font_in_button = App->font->Load("fonts/Future Now.ttf", 30);
	std::string string_play_in_button = "RESUME";
	std::string string_save_in_button = "SAVE";
	std::string string_load_in_button = "LOAD";
	std::string string_exit_in_button = "EXIT";
	/*button_in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 470, 257, textHitbox, font_in_button, fontRgb, false, false, false, main_in_menu, &string_play_in_button);
	button_in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 485, 327, textHitbox, font_in_button, fontRgb, false, false, false, main_in_menu, &string_save_in_button);
	button_in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 485, 397, textHitbox, font_in_button, fontRgb, false, false, false, main_in_menu, &string_load_in_button);
	button_in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 495, 467, textHitbox, font_in_button, fontRgb, false, false, false, main_in_menu, &string_exit_in_button);*/

	firstScrollPosCalc = false;
	secondScrollPosCalc = false;
}

void Scene1::OnEventCall(UI* element, UI_Event ui_event)
{
	if (element == in_buttons_resume && ui_event == UI_Event::UNCLICKED)
	{
		App->transition_manager->CreateAlternatingBars(SCENES::MAIN_MENU, 0.5f, true, 10, false, true);
	}
}

void Scene1::ExecuteTransition()
{
	App->transition_manager->CreateAlternatingBars(SCENES::MAIN_MENU, 0.5f, true, 10, false, true);
}

void Scene1::UnitDebugKeys()
{
	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
	{
		gatherer_test.push_back((Gatherer*)App->entityManager->CreateEntity(ENTITY_TYPE::GATHERER, App->player->mouse_tile.x, App->player->mouse_tile.y));
	}
	if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
	{
		barracks_test.push_back((Barracks*)App->entityManager->CreateEntity(ENTITY_TYPE::BARRACKS, App->player->mouse_tile.x, App->player->mouse_tile.y));
	}
	if (App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
	{
		town_hall_test.push_back((TownHall*)App->entityManager->CreateEntity(ENTITY_TYPE::TOWNHALL, App->player->mouse_tile.x, App->player->mouse_tile.y));
	}
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
	{
		enemy_test.push_back((Enemy*)App->entityManager->CreateEntity(ENTITY_TYPE::ENEMY, App->player->mouse_tile.x, App->player->mouse_tile.y));
	}
	if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
	{
		infantry_test.push_back((Infantry*)App->entityManager->CreateEntity(ENTITY_TYPE::INFANTRY, App->player->mouse_tile.x, App->player->mouse_tile.y));
	}

}

void Scene1::PathfindingDebug()
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

void Scene1::DrawPathfindingDebug()
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
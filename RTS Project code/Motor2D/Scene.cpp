#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "FadeScene.h"
#include "Scene.h"
#include "Main_Menu.h"
#include "Console.h"
#include "Collisions.h"
#include "Gui.h"
#include "Pathfinding.h"

#include "Brofiler\Brofiler.h"

#include "EntityManager.h"

//#include "mmgr/mmgr.h"

Scene::Scene() : Module()
{
	name = ("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");

	bool ret = true;
	
	fade_time = config.child("fade_time").attribute("value").as_float();
	
	for (pugi::xml_node map = config.child("map_name"); map; map = map.next_sibling("map_name"))
	{
		std::string data;							
	
		data = (map.attribute("name").as_string());
		map_names.push_back(data);
	}
	
	music_path = (config.child("audio").attribute("path").as_string());
	music_path2=(config.child("audio2").attribute("path").as_string());
	music_path3=(config.child("audio3").attribute("path").as_string());
	
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	bool ret = false;

	//cam_debug_speed = App->render->cam.camera_debug_speed;				//Sets the camera speed in debug mode.

	to_end = false;
	firstMap	= true;
	secondMap	= false;
	
	ret = LoadFirstMap();

	path_debug_tex = App->tex->Load("maps/path2.png");

	//test entity
	rock_test.push_back((Rock*)App->entityManager->CreateEntity(ENTITY_TYPE::ROCK,2, 2));
	enemy_test.push_back((Enemy*)App->entityManager->CreateEntity(ENTITY_TYPE::ENEMY, 10, 10));
	gatherer_test.push_back((Gatherer*)App->entityManager->CreateEntity(ENTITY_TYPE::GATHERER, 5, 5));
	gatherer_test.push_back((Gatherer*)App->entityManager->CreateEntity(ENTITY_TYPE::GATHERER, 10, 5));
	gatherer_test.push_back((Gatherer*)App->entityManager->CreateEntity(ENTITY_TYPE::GATHERER, 7, 7));
	town_hall_test.push_back((TownHall*)App->entityManager->CreateEntity(ENTITY_TYPE::TOWNHALL, 0,0));
	barracks_test.push_back((Barracks*)App->entityManager->CreateEntity(ENTITY_TYPE::BARRACKS, 5, 2));

	rock_test.back()->entity_sprite = App->tex->Load("maps/debug_tile.png");
	
	//LoadGuiElements();
	
	//test background
	background_rect = { 0,0,1280,720 };
	background_texture = App->tex->Load("maps/hacker_background.png");
	

	//App->audio->PlayMusic(App->scene->music_path2.c_str());
	

	return ret;

}

// Called each loop iteration
bool Scene::PreUpdate()
{
	// debug pathfing ------------------
	if (App->map->pathfindingMetaDebug == true)
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

	//Transition MainMenu
	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	{
		App->mainmenu->Enable();
		Disable();
	}
	
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)														//Receives dt as an argument.
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::LavenderBlush);
	
	DebugKeys();											//Debug Keys
	
	App->render->Blit(background_texture, 0, 0, &background_rect,false,0.0f);

	App->map->Draw();										//Map Draw
	
	if (App->map->pathfindingMetaDebug == true)
	{
		PathfindingDebug();									//Pathfinding Debug. Shows a debug texture on the path's tiles.
	}

	

	//LOG("Rocks %d", rock_test.size());

	return true;
}


// Called each loop iteration
bool Scene::PostUpdate()
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::LavenderBlush);
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		//ret = false;
	}
	
	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	App->collisions->CleanUp();								//Deletes all colliders that were loaded for this scene / map.
	App->entityManager->DestroyEntities();					//Destroys all non-player entities.
	App->map->CleanUp();									//Deletes everything related with the map from memory. (Tilesets, Layers and ObjectGroups)
	App->gui->CleanUp();
	
	return true;
}

bool Scene::Load(pugi::xml_node& data)
{
	if (currentMap != data.child("currentMap").attribute("num").as_int())
	{
		LOG("Calling switch maps");
		currentMap = data.child("currentMap").attribute("num").as_int();

		std::list<std::string>::iterator map_iterator = map_names.begin();

		std::advance(map_iterator, data.child("currentMap").attribute("num").as_int() );

		App->map->SwitchMaps( (*map_iterator) );
	}
	return true;
}

bool Scene::Save(pugi::xml_node& data) const
{
	data.append_child("currentMap").append_attribute("num") = currentMap;
	return true;
}

bool Scene::Load_lvl(int time)
{
	std::list<std::string>::iterator map_iterator = map_names.begin();

	std::advance(map_iterator, time);

	App->map->SwitchMaps( (*map_iterator) ); //map_names[time]
	
	return false;
}

bool Scene::LoadFirstMap()
{
	bool ret = false;
	
	//First walkability map created here
	ret = App->map->Load((*map_names.begin()).c_str());
	LOG("Map Name: %s", (*map_names.begin()).c_str());

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

	return ret;
}

void Scene::LoadGuiElements()
{
	//In-game menu
	SDL_Rect main_in_rect{ 0, 388, 466, 447 };
	main_in_menu = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 280, 180, main_in_rect, false, false, false, NULL);
	
	SDL_Rect label1_rect_in{ 1078,242,382,61 };
	label_1_in = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 323, 160, label1_rect_in, false, false, false, main_in_menu);
	
	_TTF_Font* font_in_esc = App->font->Load("fonts/Minecraftia-Regular.ttf", 32);
	std::string string_title_in = "PAUSE MENU";
	//in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 410, 166, textHitbox, font_in_esc, fontRgb, false, false, false, main_in_menu, &string_title_in);
	
	SDL_Rect in_rect = { 1057,30,173,45 };
	SDL_Rect idle_in = { 1057,30,173,45 };
	SDL_Rect hover_in = { 1057,76,173,45 };
	SDL_Rect clicked_in = { 1057,126,173,45 };
	in_buttons_resume = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 430, 247, false, true, false, main_in_menu, &idle_in, &hover_in, &clicked_in);
	in_buttons_save = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 430, 317, false, true, false, main_in_menu, &idle_in, &hover_in, &clicked_in);
	in_buttons_load = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 430, 387, false, true, false, main_in_menu, &idle_in, &hover_in, &clicked_in);
	in_buttons_exit = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 430, 457, false, true, false, main_in_menu, &idle_in, &hover_in, &clicked_in);
	
	SDL_Rect unmute_rect_in = { 512,147,57,57 };
	SDL_Rect idle_u_in = { 512,147,57,57 };
	SDL_Rect hover_u_in = { 512,267,57,57 };
	SDL_Rect clicked_u_in = { 1479,72,57,57 };
	unmute_in = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 670, 550, false, true, false, main_in_menu, &idle_u_in, &hover_u_in, &clicked_u_in);
	SDL_Rect mute_rect_in = { 1479, 9, 57, 57 };
	SDL_Rect idle_m_in = { 1479, 9, 57, 57 };
	SDL_Rect hover_m_in = { 1544,9,57,57 };
	SDL_Rect clicked_m_in = { 440,214,57,57 };
	mute_in = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 300, 550, false, true, false, main_in_menu, &idle_m_in, &hover_m_in, &clicked_m_in);
	
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

void Scene::DebugKeys()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)			//Load First Level Key
	{
		App->fadescene->FadeToBlack("Test_map.tmx");
	}

	//if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)			//Load Second Level Key
	//{
	//	App->fadescene->FadeToBlack("Test_map_2.tmx");
	//}

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		App->pause = !App->pause;
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)			//Save Game Key
	{
		App->SaveGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)			//Load Game Key
	{
		App->LoadGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)			//Enable / Diable free camera movement Key
	{
		//App->render->cam.camera_debug = !App->render->cam.camera_debug;
	}

	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)			//PathfindingCollisions meta layer Debug Key
	{
		App->gui->ui_debug = !App->gui->ui_debug;
	}

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)			//Collider Debug Key
	{
		App->collisions->collider_debug = !App->collisions->collider_debug;
		App->map->pathfindingMetaDebug = !App->map->pathfindingMetaDebug;
	}

	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)		//Enabling / Disabling frame cap
	{
		//App->framesAreCapped = !App->framesAreCapped;
		if (App->frame_cap == CAP_AT_60)
		{
			App->frame_cap = CAP_AT_30;
		}
		else
		{
			App->frame_cap = CAP_AT_60;
		}
	}
}

void Scene::PathfindingDebug()
{
	// Debug pathfinding ------------------------------
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(path_debug_tex, p.x, p.y);								//Should we want it, we could make a separate texture called mouse_debug_tex so the tex at mouse pos and the tex at path tile are different.

	const std::vector<iPoint>* path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path->size(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->at(i).x, path->at(i).y);		//Both work, reach a consensus on which to use.
		//iPoint pos = App->map->MapToWorld((*path)[i].x, (*path)[i].y);

		App->render->Blit(path_debug_tex, pos.x, pos.y);
	}
}
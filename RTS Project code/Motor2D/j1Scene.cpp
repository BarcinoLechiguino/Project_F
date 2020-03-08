#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1FadeScene.h"
#include "j1Scene.h"

#include "j1Console.h"
#include "j1Collisions.h"
#include "j1Gui.h"
#include "j1EntityManager.h"

#include "j1Pathfinding.h"

#include "Brofiler\Brofiler.h"

//#include "mmgr/mmgr.h"

j1Scene::j1Scene() : j1Module()
{
	name = ("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
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
bool j1Scene::Start()
{
	bool ret = false;

	to_end = false;
	
	firstMap	= true;
	secondMap	= false;
	
	ret = App->map->Load((*map_names.begin()).c_str());
	LOG("Map Name: %s", (*map_names.begin()).c_str());

	rock_test.push_back( (j1Rock*)App->entityManager->CreateEntity(ENTITY_TYPE::ROCK, 0, 0) );

	rock_test.back->entity_sprite = App->tex->Load("maps/debug_tile.png");


	cam_debug_speed = App->render->cam.camera_debug_speed;				//Sets the camera speed in debug mode.
	
	if (ret == true)													//If the first map is loaded then create the walkability map for it.
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))				//If CreatewalkabilityMap() returns true. It means that the walkability map could be created.
		{
			App->pathfinding->SetMap(w, h, data);						//Sets a new walkability map with the map passed by CreateWalkabilityMap().
		}
	
		RELEASE_ARRAY(data);											//Frees all memory allocated to the data array.
	}
	
	//path_debug_tex = App->tex->Load("maps/path2_centered.png");					//Sets the path_debug_tex as path2.png.
	path_debug_tex = App->tex->Load("maps/path2.png");					//Sets the path_debug_tex as path2.png.
	
	LoadGuiElements();
	
	//App->audio->PlayMusic(App->scene->music_path2.c_str());
	
	return ret;

}

// Called each loop iteration
bool j1Scene::PreUpdate()
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
	
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)														//Receives dt as an argument.
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::LavenderBlush);
	//Camera Movement With Arrow Keys
	//if (App->render->cam.camera_debug == true)
	//{
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			App->render->camera.x += ceil(cam_debug_speed * dt);	//As the value is multiplied by dt, camera movement will be adjusted to the framerate.  (100 * 0.033s (30fps), 100 * 0.066s (60fps)...)
		}
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			App->render->camera.x -= ceil(cam_debug_speed * dt);	//Ceil rounds up all the decimal values, returning the smallest integral value not less than the given value. 
		}
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			App->render->camera.y += ceil(cam_debug_speed * dt);
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			App->render->camera.y -= ceil(cam_debug_speed * dt);
		}
	//}

	// ---------------------------------------- DEBUG KEYS ----------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)			//Load First Level Key
	{
		App->fadescene->FadeToBlack("Test_map.tmx");
		
	}
	
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)			//Load Second Level Key
	{
		App->fadescene->FadeToBlack("Test_map_2.tmx");
	}
	
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
		App->render->cam.camera_debug = !App->render->cam.camera_debug;
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
	// --------------------------------------------------------------------------------------------
	
	App->map->Draw(); //Map Draw
	
	if (App->map->pathfindingMetaDebug == true)
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

	// --- Audio Scrollbars
	if (scrollbar_settings->isVisible)
	{
		iPoint currentThumbPos = { scrollbar_settings->GetThumbHitbox().x, scrollbar_settings->GetThumbHitbox().y };

		int offset = currentThumbPos.x - scrollbar_settings->GetHitbox().x;

		App->audio->volume = offset;
	}

	LOG("Rocks %d", rock_test.size());

	return true;
}


// Called each loop iteration
bool j1Scene::PostUpdate()
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
bool j1Scene::CleanUp()
{
	App->collisions->CleanUp();								//Deletes all colliders that were loaded for this scene / map.
	App->entityManager->DestroyEntities();					//Destroys all non-player entities.
	App->map->CleanUp();									//Deletes everything related with the map from memory. (Tilesets, Layers and ObjectGroups)
	//App->audio->CleanUp();
	
	App->gui->CleanUp();
	//App->gui->Disable();
	
	return true;
}

bool j1Scene::Load(pugi::xml_node& data)
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

bool j1Scene::Save(pugi::xml_node& data) const
{
	data.append_child("currentMap").append_attribute("num") = currentMap;
	return true;
}

bool j1Scene::Load_lvl(int time)
{
	std::list<std::string>::iterator map_iterator = map_names.begin();

	std::advance(map_iterator, time);

	App->map->SwitchMaps( (*map_iterator) ); //map_names[time]
	
	return false;
}

void j1Scene::LoadGuiElements()
{
	// Main Menu
	//--------------------------------------------------------------------------------------------


	SDL_Rect image_rect{ 1654,56,915,768 };
	background_image = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 45, 0, image_rect, true, false, false, NULL);

	SDL_Rect bars_rect{ 1601, 347, 45, 486 };
	side_bars = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 0, 0, bars_rect, true, false, false, background_image);
	side_bars = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 0, 486, bars_rect, true, false, false, background_image);
	side_bars = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 960, 0, bars_rect, true, false, false, background_image);
	side_bars = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 1005, 0, bars_rect, true, false, false, background_image);
	side_bars = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 960, 486, bars_rect, true, false, false, background_image);
	side_bars = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 1005, 486, bars_rect, true, false, false, background_image);

	SDL_Rect main_rect{ 0, 388, 466, 447 };
	main_window = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 280, 180, main_rect, true, false, false, NULL);

	SDL_Rect label1_rect{ 1078,242,382,61 };
	label_1 = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 323, 100, label1_rect, true, false, false, main_window);

	SDL_Rect label2_rect{ 973,342,237,39 };
	label_2 = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 393, 145, label2_rect, true, false, false, main_window);

	SDL_Rect button_main_menu = { 1166,418,423,107 };
	SDL_Rect idle = { 1166,418,423,107 };
	SDL_Rect hover = { 1166,556,423,107 };
	SDL_Rect clicked = { 1166,702,423,107 };
	main_button_play = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 305, 189, true, true, false, main_window, &idle, &hover, &clicked);
	main_button_continue = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 305, 294, true, true, false, main_window, &idle, &hover, &clicked);
	main_button_settings = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 305, 399, true, true, false, main_window, &idle, &hover, &clicked);
	main_button_exit = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 305, 506, true, true, false, main_window, &idle, &hover, &clicked);

	SDL_Rect textHitbox{ 432, 75, 65, 20 };
	_TTF_Font* font = App->font->Load("fonts/Minecraftia-Regular.ttf", 20);
	_TTF_Font* font_sub = App->font->Load("fonts/Minecraftia-Regular.ttf");
	SDL_Color fontRgb = { 0, 0, 0, 255 };
	std::string string_title = "MUTUAL COOPERATION";
	std::string string_subtitle	= "Can you trust your friend?";
	title_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 388, 114, textHitbox, font, fontRgb, true, false, false, main_window, &string_title);
	title_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 415, 155, textHitbox, font_sub, fontRgb, true, false, false, main_window, &string_subtitle);

	_TTF_Font* font_button = App->font->Load("fonts/Future Now.ttf", 40);
	std::string string_play			= "PLAY";
	std::string string_continue		= "CONTINUE";
	std::string string_settings		= "SETTINGS";
	std::string string_exit			= "EXIT";
	title_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 480, 219, textHitbox, font_button, fontRgb, true, false, false, main_window, &string_play);
	title_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 440, 320, textHitbox, font_button, fontRgb, true, false, false, main_window, &string_continue);
	title_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 440, 429, textHitbox, font_button, fontRgb, true, false, false, main_window, &string_settings);
	title_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 480, 535, textHitbox, font_button, fontRgb, true, false, false, main_window, &string_exit);

	//-----------------------------------------------------------------------------------------------------------------------
	
	// Settings menu
	SDL_Rect main_settings_rect{ 0, 388, 466, 447 };
	main_settings_menu = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 280, 180, main_settings_rect, false, false, false, NULL);
	
	SDL_Rect label1_rect_settings{ 1078,242,382,61 };
	label_1_settings = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 323, 160, label1_rect_settings, false, false, false, main_settings_menu);
	
	_TTF_Font* font_settings = App->font->Load("fonts/Minecraftia-Regular.ttf", 22);
	std::string string_title_settings = "SETTINGS MENU";
	settings_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 420, 176, textHitbox, font_settings, fontRgb, false, false, false, main_settings_menu, &string_title_settings);
	
	SDL_Rect unmute_rect = { 512,147,57,57 };
	SDL_Rect idle_u = { 512,147,57,57 };
	SDL_Rect hover_u = { 512,267,57,57 };
	SDL_Rect clicked_u = { 1479,72,57,57 };
	unmute = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 670, 300, false, true, false, main_settings_menu, &idle_u, &hover_u, &clicked_u);
	SDL_Rect mute_rect = { 1479, 9, 57, 57 };
	SDL_Rect idle_m = { 1479, 9, 57, 57 };
	SDL_Rect hover_m = { 1544,9,57,57 };
	SDL_Rect clicked_m = { 440,214,57,57 };
	mute = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 300, 300, false, true, false, main_settings_menu, &idle_m, &hover_m, &clicked_m);
	
	std::string string_volume = "Sound Settings";
	_TTF_Font* font_settings_sub = App->font->Load("fonts/Future Now.ttf", 25);
	settings_button_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 428, 253, textHitbox, font_settings_sub, fontRgb, false, false, false, main_settings_menu, &string_volume);
	
	std::string string_cap = "Cap To 30 FPS";
	settings_button_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 300, 433, textHitbox, font_settings_sub, fontRgb, false, false, false, main_settings_menu, &string_cap);
	
	SDL_Rect check_rect = { 633,323,53,53 };
	SDL_Rect idle_ch = { 633,323,53,53 };
	SDL_Rect hover_ch = { 687,323,53,53 };
	SDL_Rect clicked_ch = { 571,54,53,53 };
	check = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 560, 419, false, true, false, main_settings_menu, &idle_ch, &hover_ch, &clicked_ch);
	SDL_Rect back_rect = { 0,74,284,66 };
	SDL_Rect idle_b = { 0,74,284,66 };
	SDL_Rect hover_b = { 285,74,284,66 };
	SDL_Rect clicked_b = { 0,142,284,66 };
	back = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 370, 550, false, true, false, main_settings_menu, &idle_b, &hover_b, &clicked_b);
	SDL_Rect credits_rect = { 744,320,58,58 };
	SDL_Rect idle_c = { 744,320,58,58 };
	SDL_Rect hover_c = { 880,319,58,58 };
	SDL_Rect clicked_c = { 810, 319, 58, 58 };
	credits = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 300, 555, false, true, false, main_settings_menu, &idle_c, &hover_c, &clicked_c);
	SDL_Rect git_rect = { 1066,178,58,58 };
	SDL_Rect idle_g = { 1066,178,58,58 };
	SDL_Rect hover_g = { 1202,178,58,58 };
	SDL_Rect clicked_g = { 1132, 178, 58, 58 };
	github = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 670, 555, false, true, false, main_settings_menu, &idle_g, &hover_g, &clicked_g);
	
	_TTF_Font* font_settings_sub_back = App->font->Load("fonts/Future Now.ttf", 40);
	std::string string_back = "BACK";
	settings_button_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 470, 570, textHitbox, font_settings_sub_back, fontRgb, false, false, false, main_settings_menu, &string_back);
	
	SDL_Rect scrollbarBar = { 674,273,200,38 };
	SDL_Rect scrollbarThumb = { 928,272,37,34 };
	iPoint thumbOffset = { -2, 0 };
	SDL_Rect scrollMask = { 0, 0, 350, 158 };
	iPoint maskOffset = { -360, 0 };
	SDL_Rect dragArea = { 0, 0, 11, 158 };
	float dragFactor = 0.2f;
	scrollbar_settings = (UI_Scrollbar*)App->gui->CreateScrollbar(UI_Element::SCROLLBAR, 420, 309, scrollbarBar, scrollbarThumb, thumbOffset, dragArea, dragFactor, true, false, true,
		false, false, false, main_settings_menu, &scrollMask, maskOffset);
	//-----------------------------------------------------------------------------------------------------------------------
	
	// Credits menu
	SDL_Rect main_credits_rect{ 0, 388, 466, 447 };
	main_credits_menu = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 280, 180, main_credits_rect, false, false, false, NULL);
	
	SDL_Rect label1_rect_credits{ 1078,242,382,61 };
	label_1_credits = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 323, 160, label1_rect_credits, false, false, false, main_credits_menu);
	
	_TTF_Font* font_credits = App->font->Load("fonts/Minecraftia-Regular.ttf", 22);
	std::string string_title_credits = "CREDITS";
	credits_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 460, 173, textHitbox, font_credits, fontRgb, false, false, false, main_credits_menu, &string_title_credits);
	
	SDL_Rect back_c_rect = { 1057,30,173,45 };
	SDL_Rect idle_b_c = { 1057,30,173,45 };
	SDL_Rect hover_b_c = { 1057,76,173,45 };
	SDL_Rect clicked_b_c = { 1057,126,173,45 };
	back_credits = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 430, 570, false, true, false, main_credits_menu, &idle_b_c, &hover_b_c, &clicked_b_c);
	
	_TTF_Font* font_credits_sub_back = App->font->Load("fonts/Future Now.ttf", 31);
	credits_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 485, 580, textHitbox, font_credits_sub_back, fontRgb, false, false, false, main_credits_menu, &string_back);
	
	_TTF_Font* default = App->font->Load("fonts/arial.ttf", 12);
	_TTF_Font* default_bigger = App->font->Load("fonts/arial.ttf", 15);
	std::string one			= "MIT License Copyright(c)[2019][Angel Gonzalez, Gerard Romeu]";
	std::string two			= "Permission is hereby granted, free of charge, to any person obtaining a copy";
	std::string three		= "of this software and associated documentation files(the Software), to deal in";
	std::string four		= "the Software without restriction, including without limitation the rights to use,";
	std::string five		= "copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the ";
	std::string six			= "Software, and to permit persons to whom the Software is furnished to do so, ";
	std::string seven		= "subject to the following conditions : ";
	std::string eight		= "The above copyright notice and this permission notice shall be included in all  ";
	std::string nine		= "copies or substantial portions of the Software. ";
	std::string ten			= "THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, ";
	std::string eleven		= "EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES ";
	std::string twelve		= "OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON";
	std::string thirteen	= "INFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT  ";
	std::string fourteen	= "HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, ";
	std::string fifteen		= "WHETHER IN AN  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING ";
	std::string sixteen		= "FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR ";
	std::string seventeen	= "OTHER DEALINGS IN THE SOFTWARE. ";
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 300, 250, textHitbox, default_bigger, fontRgb, false, false, false, main_credits_menu, &one);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 300, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &two);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 315, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &three);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 330, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &four);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 345, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &five);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 360, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &six);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 375, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &seven);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 390, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &eight);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 405, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &nine);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 430, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &ten);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 445, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &eleven);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 460, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &twelve);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 475, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &thirteen);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 490, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &fourteen);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 505, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &fifteen);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 520, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &sixteen);
	text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 298, 535, textHitbox, default, fontRgb, false, false, false, main_credits_menu, &seventeen);
	//-----------------------------------------------------------------------------------------------------------------------
	
	// HUD
	/*SDL_Rect hud_bars_rect{ 973,305,677,36 };
	upper_bar = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 0, 0, hud_bars_rect, false, false, false, NULL);
	upper_bar1 = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 0, 36, hud_bars_rect, false, false, false, upper_bar);
	upper_bar1 = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 676, 0, hud_bars_rect, false, false, false, upper_bar);
	upper_bar1 = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 676, 36, hud_bars_rect, false, false, false, upper_bar);
	upper_bar1 = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 1352, 0, hud_bars_rect, false, false, false, upper_bar);
	upper_bar1 = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 1352, 36, hud_bars_rect, false, false, false, upper_bar);
	
	SDL_Rect p1_rect{ 1270,210,25,21 };
	p1 = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 80, 30, p1_rect, false, false, false, upper_bar);
	
	SDL_Rect p2_rect{ 1301,210,26,21 };
	p2 = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 920, 30, p2_rect, false, false, false, upper_bar);
	
	_TTF_Font* font_in = App->font->Load("fonts/Future Now.ttf", 25);
	_TTF_Font* font_in_title = App->font->Load("fonts/Minecraftia-Regular.ttf", 21);
	p2SString string_p1 = "JOE";
	p2SString string_p2 = "TIMM";
	p2SString string_score = "SCORE";
	p2SString string_timer = "TIMER";
	p2SString string_title_hud = "MUTUAL COOPERATION";
	names = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 17, 30, textHitbox, font_in, fontRgb, false, false, false, upper_bar, &string_p1);
	names = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 970, 30, textHitbox, font_in, fontRgb, false, false, false, upper_bar, &string_p2);
	names = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 210, 30, textHitbox, font_in, fontRgb, false, false, false, upper_bar, &string_score);
	names = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 670, 30, textHitbox, font_in, fontRgb, false, false, false, upper_bar, &string_timer);
	title = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 375, 25, textHitbox, font_in_title, fontRgb, false, false, false, upper_bar, &string_title_hud);
	
	SDL_Rect hearts_rect{ 1058,8,24,21 };
	hearts[0] = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 113, 30, hearts_rect, false, false, false, upper_bar);
	hearts[1] = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 138, 30, hearts_rect, false, false, false, upper_bar);
	hearts[2] = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 163, 30, hearts_rect, false, false, false, upper_bar);
	hearts[3] = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 880, 30, hearts_rect, false, false, false, upper_bar);
	hearts[4] = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 855, 30, hearts_rect, false, false, false, upper_bar);
	hearts[5] = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 830, 30, hearts_rect, false, false, false, upper_bar);
	
	p2SString string_score_player = "0";
	score_player = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 310, 30, textHitbox, font_in, fontRgb, false, false, false, upper_bar, &string_score_player);
	p2SString string_timer_player = "0";
	timer = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 755, 30, textHitbox, font_in, fontRgb, false, false, false, upper_bar, &string_timer_player);
	
	SDL_Rect coin_rect{ 1267,139,36,36 };
	coin = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 50, 730, coin_rect, false, false, false, upper_bar);
	
	SDL_Color fontRgbWhite = { 255, 255, 255, 255 };
	_TTF_Font* font_count = App->font->Load("fonts/Future Now.ttf", 40);
	p2SString string_coin_count = "0";
	count_coins = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 20, 735, textHitbox, font_count, fontRgbWhite, false, false, false, upper_bar, &string_coin_count);*/


	//In-game menu
	SDL_Rect main_in_rect{ 0, 388, 466, 447 };
	main_in_menu = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 280, 180, main_in_rect, false, false, false, NULL);
	
	SDL_Rect label1_rect_in{ 1078,242,382,61 };
	label_1_in = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 323, 160, label1_rect_in, false, false, false, main_in_menu);
	
	_TTF_Font* font_in_esc = App->font->Load("fonts/Minecraftia-Regular.ttf", 32);
	std::string string_title_in = "PAUSE MENU";
	in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 410, 166, textHitbox, font_in_esc, fontRgb, false, false, false, main_in_menu, &string_title_in);
	
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
	button_in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 470, 257, textHitbox, font_in_button, fontRgb, false, false, false, main_in_menu, &string_play_in_button);
	button_in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 485, 327, textHitbox, font_in_button, fontRgb, false, false, false, main_in_menu, &string_save_in_button);
	button_in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 485, 397, textHitbox, font_in_button, fontRgb, false, false, false, main_in_menu, &string_load_in_button);
	button_in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 495, 467, textHitbox, font_in_button, fontRgb, false, false, false, main_in_menu, &string_exit_in_button);
	
	scrollbar_in = (UI_Scrollbar*)App->gui->CreateScrollbar(UI_Element::SCROLLBAR, 430, 559, scrollbarBar, scrollbarThumb, thumbOffset, dragArea, dragFactor, true, false, true,
		false, false, false, main_in_menu, &scrollMask, maskOffset);

	firstScrollPosCalc = false;
	secondScrollPosCalc = false;
}
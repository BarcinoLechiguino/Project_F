#include "Dependencies\Brofiler\Brofiler.h"
//#include "mmgr/mmgr.h"

#include "Log.h"
#include "EasingFunctions.h"

#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Input.h"
#include "Audio.h"
#include "Map.h"
#include "Minimap.h"
#include "Pathfinding.h"
#include "Player.h"
#include "Scene.h"
#include "FowManager.h"
#include "DialogManager.h"
#include "ParticleManager.h"
#include "Emitter.h"
#include "QuestManager.h"
#include "ProjectileManager.h"

#include "EntityManager.h"
#include "Entity.h"

#include "Gatherer.h"
#include "Scout.h"
#include "Infantry.h"
#include "Heavy.h"
#include "EnemyGatherer.h"
#include "EnemyScout.h"
#include "EnemyInfantry.h"
#include "EnemyHeavy.h"

#include "StaticObject.h"
#include "TownHall.h"
#include "Barracks.h"
#include "Wall.h"
#include "EnemyTownHall.h"
#include "EnemyBarracks.h"
#include "EnemyWall.h"
#include "Rock.h"
#include "Tree.h"
#include "Obelisk.h"
#include "Boulder.h"

#include "GuiManager.h"
#include "GuiElement.h"
#include "GuiText.h"
#include "GuiButton.h"
#include "GuiScrollbar.h"
#include "GuiImage.h"

#include "SceneManager.h"
#include "TransitionManager.h"

#include "GameplayScene.h"

GameplayScene::GameplayScene() : Scene(SCENES::GAMEPLAY_SCENE), background_rect({ 0, 0, 0, 0 }), background_texture(nullptr)
{
	name_tag = ("Gameplay");
}

// Destructor
GameplayScene::~GameplayScene()
{}

// Called before render is available
bool GameplayScene::Awake(pugi::xml_node& config)
{
	return true;
}

// Called before the first frame
bool GameplayScene::Start()
{
	bool ret = true;

	InitScene();

	App->particle_manager->SpawnEmitter({ 0,0 }, EMITTER_BACKGROUND);

	return ret;
}

// Called each loop iteration
bool GameplayScene::PreUpdate()
{
	// debug pathfing ------------------
	if (App->map->pathfinding_meta_debug)
	{
		PathfindingDebug();
	}
	
	// Text Save
	if (saved_text_active)
	{
		time_on_scene += App->GetUnpausableDt();
		
		if (time_on_scene > 7.0f)
		{
			App->gui_manager->SetElementsVisibility(Saving, false);
			time_on_scene = 0.0f;
			saved_text_active = false;
		}
	}
	

	return true;
}

// Called each loop iteration
bool GameplayScene::Update(float dt)														//Receives dt as an argument.
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::LavenderBlush);

	//App->render->Blit(background_texture, 0, 0, &background_rect, false, 0.0f);

	App->map->Draw();																		//Map Draw

	App->minimap->BlitMinimap();

	if (App->map->walkability_debug || App->map->entity_map_debug)
	{
		App->map->DataMapDebug();																// Will print on screen the debug tiles of the walkability map and the entity map.
	}

	if (App->map->pathfinding_meta_debug)
	{
		DrawPathfindingDebug();																//Pathfinding Debug. Shows a debug texture on the path's tiles.
	}

	AdjustVolumeWithScrollbar();

	DebugHUDSpawn();

	// Refresh resources on screen
	std::string HUD_data_resource_string = std::to_string(App->entity_manager->resource_data);
	HUD_data_resource_text->RefreshTextInput(HUD_data_resource_string.c_str());

	std::string HUD_electricity_resource_string = std::to_string(App->entity_manager->resource_electricity);
	HUD_electricity_resource_text->RefreshTextInput(HUD_electricity_resource_string.c_str());

	std::string HUD_bits_resource_string = std::to_string(App->entity_manager->resource_bits);
	HUD_bytes_resource_text->RefreshTextInput(HUD_bits_resource_string.c_str());

	CheckCompletedQuests();

	HandleTutorial();

	return true;
}


// Called each loop iteration
bool GameplayScene::PostUpdate()
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::LavenderBlush);
	bool ret = true;

	DebugKeys();																	//Debug Keys

	if (App->player->god_mode)
	{
		CameraDebugMovement(App->GetUnpausableDt());
		UnitDebugKeys();
	}

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_STATE::KEY_DOWN)
	{
		App->render->camera.x = App->player->original_camera_position.x;
		App->render->camera.y = App->player->original_camera_position.y;
	}

	CheckForWinLose();

	//Transition To Any Scene. Load Scene / Unload GameplayScene
	ExecuteTransition();

	ExecuteDebugTransition();

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_STATE::KEY_DOWN
		|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_B) == BUTTON_STATE::BUTTON_DOWN)
	{	
		SwitchPauseMenuMode();
	}

	if (App->player->god_mode)
	{
		if (!God_Mode_Activated->is_visible)
		{
			App->gui_manager->SetElementsVisibility(God_Mode_Activated, true);
		}
	}
	else
	{
		if (God_Mode_Activated->is_visible)
		{
			App->gui_manager->SetElementsVisibility(God_Mode_Activated, false);
		}
	}

	if (App->pause)
	{
		App->audio->volume *= 0.5;
	}

	App->minimap->BlitMinimap();

	return ret;
}

// Called before quitting
bool GameplayScene::CleanUp()
{
	App->tex->UnLoad(background_texture);
	App->tex->UnLoad(occupied_debug);
	App->tex->UnLoad(occupied_by_entity_debug);
	App->tex->UnLoad(path_debug_tex);

	App->player->ClearEntityBuffers();						//Clears the entity list
	App->entity_manager->DestroyEntities();					//Destroys all non-player entities.
	App->map->CleanUp();									//Deletes everything related with the map from memory. (Tilesets, Layers and ObjectGroups)
	App->gui_manager->DestroyGuiElements();					//Deletes all the Gui Elements of the Gameplay Scene.
	App->dialog_manager->CleanUp();							//Deletes everything related with dialog in the Gameplay Scene.
	App->particle_manager->RemoveEverything();
	App->projectile_manager->CleanUp();

	App->player->god_mode = false;							//Will disable the God Mode upon exiting the Gameplay Scene.
	App->fow_manager->fow_debug = false;					//Will disable the FOW Debug Mode upon exiting the Gameplay Scene.

	return true;
}

bool GameplayScene::Load(pugi::xml_node& data)
{


	return true;
}

bool GameplayScene::Save(pugi::xml_node& data) const
{


	return true;
}

// --------------- INITIALIZING THE SCENE ---------------
void GameplayScene::InitScene()
{
	bool ret = true;

	transition_to_main_menu_scene = false;
	transition_to_win_scene = false;
	transition_to_lose_scene = false;

	App->entity_manager->resource_data = 0;
	App->entity_manager->resource_electricity = 0;
	App->entity_manager->resource_bits = 0;

	App->render->camera.x = App->player->original_camera_position.x;
	App->render->camera.y = App->player->original_camera_position.y;

	ret = App->map->Load(App->config_file.child("config").child("scenes").child("map_name").attribute("name").as_string());

	//test background
	//background_rect = { 0, 0, 1280, 720 };
	//background_texture = App->tex->Load("maps/hacker_background.png");

	LoadGuiElements();

	path_debug_tex = App->tex->Load(App->config_file.child("config").child("debug").child("textures").child("path_tile_debug").attribute("path").as_string());
	occupied_debug = App->tex->Load(App->config_file.child("config").child("debug").child("textures").child("occupied_tile_debug").attribute("path").as_string());
	occupied_by_entity_debug = App->tex->Load(App->config_file.child("config").child("debug").child("textures").child("entity_map_tile_debug").attribute("path").as_string());

	//App->audio->PlayMusic(App->scene->music_path2.c_str());
	inGame_song = App->audio->LoadMusic(App->config_file.child("config").child("scenes").child("music").child("gameplay_scene").attribute("path").as_string());
	App->audio->PlayMusic(inGame_song, 0.0f);

	//App->fow_manager->ResetVisibilityMap();

	if (!App->player->load_game_from_main_menu)
	{
		App->dialog_manager->StartDialog(0);
		tutorial.tutorial_state = TutorialState::SELECT_UNIT;

		tutorial.boulders_active = true;
		tutorial.lock_camera = false;
	}
	else
	{
		tutorial.tutorial_state = TutorialState::NOT_ACTIVE;
		App->LoadGame("save_game.xml");
	}

	App->projectile_manager->Start();

	time_on_scene = 0.0f;
}

// --- SCENE TRANSITIONS
void GameplayScene::ExecuteTransition()
{
	if (transition_to_main_menu_scene)
	{
		if (App->pause)
		{
			App->pause = false;
		}

		//App->transition_manager->CreateAlternatingBars(SCENES::MAIN_MENU_SCENE, 0.5f, true, 10, false, true);
		App->transition_manager->CreateOldMonitor(SCENES::MAIN_MENU_SCENE, 1.0f, true, Black);
	}

	if (transition_to_win_scene)
	{
		if (App->pause)
		{
			App->pause = false;
		}

		App->transition_manager->CreateSlide(SCENES::WIN_SCENE, 0.5f, true, true, false, false, Black);
	}

	if (transition_to_lose_scene)
	{
		if (App->pause)
		{
			App->pause = false;
		}

		App->transition_manager->CreateSlide(SCENES::LOSE_SCENE, 0.5f, true, true, false, false, Black);
	}
}

void GameplayScene::HandleTutorial()
{
	switch (tutorial.tutorial_state)
	{
	case TutorialState::NOT_ACTIVE:

		HUD_dialogs_skip_tutorial->is_visible = false;

		if (tutorial.boulders_active)
		{
			for (std::vector<Entity*>::iterator boulder = App->map->tutorial_boulders.begin(); boulder != App->map->tutorial_boulders.end(); ++boulder)
			{
				App->entity_manager->DeleteEntity((*boulder));
			}
			App->map->tutorial_boulders.clear();

			tutorial.boulders_active = false;
		}

		break;
	case TutorialState::SELECT_UNIT:

		break;
	case TutorialState::MOVE_UNIT:

		break;
	case TutorialState::SELECT_GATHERER:

		break;
	case TutorialState::GATHER_RESOURCE:

		break;
	case TutorialState::GATHER_MORE_RESOURCES:
		if (App->entity_manager->resource_electricity >= 5 && App->entity_manager->resource_data >= 40) //Tutorial 4
		{
			tutorial.tutorial_state = TutorialState::RECRUIT_INFANTRY;
			App->dialog_manager->StartDialog(4);
		}

		break;
	case TutorialState::RECRUIT_INFANTRY:

		break;
	}
}

void Tutorial::NextStep( TutorialState state)
{
	App->scene_manager->gameplay_scene->tutorial.tutorial_state = state;
}

// --------------- WIN/LOSE CONDITION METHODS ---------------
void GameplayScene::CheckForWinLose()
{
	if (!App->player->god_mode)
	{
		//Check for an enemy townhall alive. If none is found the player has won, thus we call the transition to win scene
		bool exists_enemytownhall = false;
		for (int i = 0; i < (int)App->entity_manager->entities.size(); ++i)
		{
			if (App->entity_manager->entities[i]->type == ENTITY_TYPE::ENEMY_TOWNHALL)
			{
				exists_enemytownhall = true;
				break;
			}
		}
		if (exists_enemytownhall == false)
		{
			transition_to_win_scene = true;
			return;
		}

		//Same but for allied town halls. We put it second so in case they break at the same frame (not gonna happen) the player wins.
		bool exists_townhall = false;
		for (int i = 0; i < (int)App->entity_manager->entities.size(); ++i)
		{
			if (App->entity_manager->entities[i]->type == ENTITY_TYPE::TOWNHALL)
			{
				exists_townhall = true;
				break;
			}
		}
		if (exists_townhall == false)
		{
			transition_to_lose_scene = true;
			return;
		}

		bool exists_allyunits = false;
		for (int i = 0; i < (int)App->entity_manager->entities.size(); ++i)
		{
			if (App->entity_manager->entities[i]->type == ENTITY_TYPE::BARRACKS
				|| App->entity_manager->entities[i]->type == ENTITY_TYPE::INFANTRY
				|| App->entity_manager->entities[i]->type == ENTITY_TYPE::GATHERER)
			{
				exists_allyunits = true;
				break;
			}
		}
		if (exists_allyunits == false)
		{
			transition_to_lose_scene = true;
			return;
		}
	}
}

bool GameplayScene::CheckForTownHall()
{

	//Check for an enemy townhall alive
	bool exists_enemytownhall = false;
	for (int i = 0; i < (int)App->entity_manager->entities.size(); ++i)
	{
		if (App->entity_manager->entities[i]->type == ENTITY_TYPE::ENEMY_TOWNHALL)
		{
			exists_enemytownhall = true;
			break;
		}
	}

	return exists_enemytownhall;
}

// ------------------- ENTITY SPAWN METHODS -------------------
void GameplayScene::SpawnAllyUnit(ENTITY_TYPE type)
{
	if (App->player->building_selected != nullptr)														// Safety check
	{
		TownHall* townhall = nullptr;
		Barracks* barrack = nullptr;

		switch (type)
		{
		case ENTITY_TYPE::GATHERER:

			if (CheckResources(40, 0, 0))
			{
				townhall = (TownHall*)App->player->building_selected;
				App->audio->PlayFx(App->gui_manager->recruit_unit_button_clicked_fx, 0);
				townhall->creation_queue.push_back(ENTITY_TYPE::GATHERER);
			}
			break;

		case ENTITY_TYPE::SCOUT:
			if (CheckResources(20, 0, 0))
			{
				townhall = (TownHall*)App->player->building_selected;
				App->audio->PlayFx(App->gui_manager->recruit_unit_button_clicked_fx, 0);
				townhall->creation_queue.push_back(ENTITY_TYPE::SCOUT);
			}
			break;

		case ENTITY_TYPE::INFANTRY:
			if (CheckResources(0, 5, 0))
			{
				barrack = (Barracks*)App->player->building_selected;
				
				App->audio->PlayFx(App->gui_manager->recruit_unit_button_clicked_fx, 0);
				barrack->creation_queue.push_back(ENTITY_TYPE::INFANTRY);

				if (tutorial.tutorial_state == TutorialState::RECRUIT_INFANTRY)//Tutorial 5
				{
					tutorial.tutorial_state = TutorialState::NOT_ACTIVE;
					App->dialog_manager->StartDialog(5);
				}
			}
			break;

		case ENTITY_TYPE::HEAVY:
			if (CheckResources(0, 15, 0))
			{
				barrack = (Barracks*)App->player->building_selected;
				App->audio->PlayFx(App->gui_manager->recruit_unit_button_clicked_fx, 0);
				barrack->creation_queue.push_back(ENTITY_TYPE::HEAVY);

			}
			break;
		}
	}
}

void GameplayScene::SpawnEnemyUnit(ENTITY_TYPE type)
{
	if (App->player->building_selected != nullptr)														// Safety check
	{
		EnemyTownHall* enemy_townhall = nullptr;
		EnemyBarracks* enemy_barrack = nullptr;

		switch (type)
		{
		case ENTITY_TYPE::ENEMY_GATHERER:


			enemy_townhall = (EnemyTownHall*)App->player->building_selected;

			enemy_townhall->creation_queue.push_back(ENTITY_TYPE::ENEMY_GATHERER);

			break;

		case ENTITY_TYPE::ENEMY_SCOUT:

			enemy_barrack = (EnemyBarracks*)App->player->building_selected;

			enemy_barrack->creation_queue.push_back(ENTITY_TYPE::ENEMY_SCOUT);

			break;

		case ENTITY_TYPE::ENEMY_INFANTRY:

			enemy_barrack = (EnemyBarracks*)App->player->building_selected;

			enemy_barrack->creation_queue.push_back(ENTITY_TYPE::ENEMY_INFANTRY);

			break;

		case ENTITY_TYPE::ENEMY_HEAVY:

			enemy_barrack = (EnemyBarracks*)App->player->building_selected;

			enemy_barrack->creation_queue.push_back(ENTITY_TYPE::ENEMY_HEAVY);


			break;
		}
	}
}

void GameplayScene::SpawnEnemyWave(int gatherer_amount, int scout_amount, int infantry_amount, int heavy_amount)
{
	EnemyTownHall* enemy_townhall = nullptr;
	EnemyBarracks* enemy_barracks = nullptr;

	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item < App->entity_manager->entities.end(); ++item)									// Finding Both the enemy townhall and the enemy barracks.
	{
		if (enemy_townhall == nullptr || enemy_barracks == nullptr)
		{
			if ((*item)->type == ENTITY_TYPE::ENEMY_TOWNHALL)
			{
				enemy_townhall = (EnemyTownHall*)(*item);
			}

			if ((*item)->type == ENTITY_TYPE::ENEMY_BARRACKS && enemy_barracks == nullptr)			// COULD THERE BE MULTIPLE ENEMY BARRACKS?
			{
				enemy_barracks = (EnemyBarracks*)(*item);
			}
		}
		else
		{
			break;
		}
	}

	if (enemy_townhall != nullptr)
	{
		for (int i = 0; i < gatherer_amount; ++i)
		{
			enemy_townhall->creation_queue.push_back(ENTITY_TYPE::ENEMY_GATHERER);
		}
	}

	if (enemy_barracks != nullptr)
	{
		for (int i = 0; i < scout_amount; ++i)
		{
			enemy_barracks->creation_queue.push_back(ENTITY_TYPE::ENEMY_SCOUT);
		}

		for (int i = 0; i < infantry_amount; ++i)
		{
			enemy_barracks->creation_queue.push_back(ENTITY_TYPE::ENEMY_INFANTRY);
		}

		for (int i = 0; i < heavy_amount; ++i)
		{
			enemy_barracks->creation_queue.push_back(ENTITY_TYPE::ENEMY_HEAVY);
		}
	}
}

void GameplayScene::BuildingUpgrade()
{
	if (App->player->building_selected != nullptr)
	{
		TownHall* townhall = nullptr;
		Barracks* barrack = nullptr;

		switch (App->player->building_selected->type)
		{
		case ENTITY_TYPE::TOWNHALL:

			townhall = (TownHall*)App->player->building_selected;

			if (townhall->level < townhall->max_level)
			{
				if (CheckResources(240, 20, 3))
				{
					App->audio->PlayFx(App->gui_manager->upgrade_button_clicked_fx, 0);
					townhall->level++;
					townhall->LevelChanges();
				}
			}
			break;

		case ENTITY_TYPE::BARRACKS:

			barrack = (Barracks*)App->player->building_selected;

			if (barrack->level < barrack->max_level)
			{
				if (CheckResources(180, 25, 2))
				{
					App->audio->PlayFx(App->gui_manager->upgrade_button_clicked_fx, 0);
					barrack->level++;
					barrack->LevelChanges();
				}
			}
			break;
		}
	}
}

void GameplayScene::UnitUpgrade(int unit)
{
	if (App->player->building_selected != nullptr)
	{
		Barracks* barrack = (Barracks*)App->player->building_selected;
		TownHall* townhall = (TownHall*)App->player->building_selected;

		switch (unit)
		{
		case 0:

			if (townhall->gatherer_level < townhall->max_gatherer_level)
			{
				if (CheckResources(100, 5, 1))
				{
					App->audio->PlayFx(App->gui_manager->upgrade_button_clicked_fx, 0);
					townhall->gatherer_level++;
				}
			}
			break;

		case 1:

			if (barrack->infantry_level < barrack->max_infantry_level)
			{
				if (CheckResources(100, 5, 1))
				{
					App->audio->PlayFx(App->gui_manager->upgrade_button_clicked_fx, 0);
					barrack->infantry_level++;
				}
			}
			break;

		case 2:

			if (townhall->scout_level < townhall->max_scout_level)
			{
				if (CheckResources(180, 30, 1))
				{
					App->audio->PlayFx(App->gui_manager->upgrade_button_clicked_fx, 0);
					townhall->scout_level++;
				}
			}

			break;

		case 3:

			if (barrack->heavy_level < barrack->max_heavy_level)
			{
				if (CheckResources(180, 30, 1))
				{
					App->audio->PlayFx(App->gui_manager->upgrade_button_clicked_fx, 0);
					barrack->heavy_level++;
				}
			}

			break;
		}
	}
}

//--------------- RESOURCE MANAGEMENT ---------------
bool GameplayScene::CheckResources(uint required_data, uint required_electricity, uint required_bits)
{
	if ((required_data <= App->entity_manager->resource_data) && (required_electricity <= App->entity_manager->resource_electricity) && (required_bits <= App->entity_manager->resource_bits))
	{
		App->entity_manager->resource_data -= required_data;
		App->entity_manager->resource_electricity -= required_electricity;
		App->entity_manager->resource_bits -= required_bits;
		return true;
	}
	else
	{
		App->audio->PlayFx(App->gui_manager->no_resources_fx, 0);
		return false;
	}
}

// --------------- UI & HUD ---------------
void GameplayScene::LoadGuiElements()
{
	// *****_____In-game menu_____*****
	// Back
	SDL_Rect background = { 780, 451, 514, 403 };
	in_game_background = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 380, 135, background, false, false, false, this, nullptr);

	// Continue Button
	SDL_Rect in_game_continue_button_size = { 0, 0, 158, 23 };
	SDL_Rect in_game_continue_button_idle = { 1, 0, 158, 23 };
	SDL_Rect in_game_continue_button_hover = { 178, 0, 158, 23 };
	SDL_Rect in_game_continue_button_clicked = { 356, 0, 158, 23 };

	in_game_continue_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 555, 241, false, true, false, this, in_game_background
		, &in_game_continue_button_idle, &in_game_continue_button_hover, &in_game_continue_button_clicked);

	// Options Button
	SDL_Rect in_game_options_button_size = { 0, 0, 133, 24 };
	SDL_Rect in_game_options_button_idle = { 1, 52, 133, 24 };
	SDL_Rect in_game_options_button_hover = { 178, 52, 133, 24 };
	SDL_Rect in_game_options_button_clicked = { 356, 52, 133, 24 };

	in_game_options_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 567, 281, false, true, false, this, in_game_background
		, &in_game_options_button_idle, &in_game_options_button_hover, &in_game_options_button_clicked);

	// Back to menu Button
	SDL_Rect in_game_back_to_menu_size = { 0, 0, 189, 23 };
	SDL_Rect in_game_back_to_menu_idle = { 0, 137, 189, 23 };
	SDL_Rect in_game_back_to_menu_hover = { 204, 137, 189, 23 };
	SDL_Rect in_game_back_to_menu_clicked = { 408, 137, 189, 23 };

	in_game_back_to_menu = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 541, 321, false, true, false, this, in_game_background
		, &in_game_back_to_menu_idle, &in_game_back_to_menu_hover, &in_game_back_to_menu_clicked);

	// Save button

	SDL_Rect in_game_save_size = { 0, 0, 189, 23 };
	SDL_Rect in_game_save_idle = { 0, 137, 189, 23 };
	SDL_Rect in_game_save_hover = { 204, 137, 189, 23 };
	SDL_Rect in_game_save_clicked = { 408, 137, 189, 23 };

	in_game_save_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 596, 361, false, true, false, this, in_game_background
		, &in_game_save_idle, &in_game_save_hover, &in_game_save_clicked);

	// Exit Button
	SDL_Rect in_game_exit_button_size = { 0, 0, 74, 23 };
	SDL_Rect in_game_exit_button_idle = { 1, 77, 74, 23 };
	SDL_Rect in_game_exit_button_hover = { 178, 77, 74, 23 };
	SDL_Rect in_game_exit_button_clicked = { 356, 77, 74, 23 };

	in_game_exit_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 596, 401, false, true, false, this, in_game_background
		, &in_game_exit_button_idle, &in_game_exit_button_hover, &in_game_exit_button_clicked);

	// Title
	SDL_Rect in_game_text_rect = { 0, 0, 100, 20 };
	std::string in_game_title_string = "Pause Menu";
	in_game_title_text = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 469, 165, in_game_text_rect, App->gui_manager->borgsquadcond_50, SDL_Color{ 255,255,0,0 }, false, false, false, this, in_game_background, &in_game_title_string);

	// Options Menu
	LoadInGameOptionsMenu();
	App->gui_manager->SetElementsVisibility(in_game_options_parent, false);



	// ****____HUD____*****

	// ****____Up Bar_____******
	// Group Up
	SDL_Rect HUD_group_button_size = { 0, 0, 63, 38 };
	SDL_Rect HUD_group_button_idle = { 759, 48, 63, 38 };
	SDL_Rect HUD_group_button_hover = { 970, 48, 63, 38 };
	SDL_Rect HUD_group_button_clicked = { 1171, 48, 63, 38 };

	HUD_group_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 544, -4, true, true, false, this, nullptr
		, &HUD_group_button_idle, &HUD_group_button_hover, &HUD_group_button_clicked);

	// Pause
	SDL_Rect HUD_pause_button_size = { 0, 0, 63, 38 };
	SDL_Rect HUD_pause_button_idle = { 825, 48, 63, 38 };
	SDL_Rect HUD_pause_button_hover = { 1036, 48, 63, 38 };
	SDL_Rect HUD_pause_button_clicked = { 1237, 48, 63, 38 };

	HUD_pause_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 601, -4, true, true, false, this, nullptr
		, &HUD_pause_button_idle, &HUD_pause_button_hover, &HUD_pause_button_clicked);

	// Play
	SDL_Rect HUD_play_button_size = { 0, 0, 63, 38 };
	SDL_Rect HUD_play_button_idle = { 1037, 3, 63, 38 };
	SDL_Rect HUD_play_button_hover = { 1104, 3, 63, 38 };
	SDL_Rect HUD_play_button_clicked = { 1171, 3, 63, 38 };

	HUD_play_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 601, -4, false, true, false, this, nullptr
		, &HUD_play_button_idle, &HUD_play_button_hover, &HUD_play_button_clicked);

	// Home 
	SDL_Rect HUD_home_button_size = { 0, 0, 63, 37 };
	SDL_Rect HUD_home_button_idle = { 890, 48, 63, 37 };
	SDL_Rect HUD_home_button_hover = { 1101, 48, 63, 37 };
	SDL_Rect HUD_home_button_clicked = { 1302, 48, 63, 37 };

	HUD_home_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 657, -4, true, true, false, this, nullptr
		, &HUD_home_button_idle, &HUD_home_button_hover, &HUD_home_button_clicked);



	//*****______Down Bar_____*****

	//Resource Bar
	SDL_Rect HUD_resource_bar_size = { 884, 223, 188, 150 };

	HUD_resource_bar = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 1095, 574, HUD_resource_bar_size, true, true, false, this, nullptr);

	//Resources
	SDL_Rect HUD_electricity_resource_size = { 707, 54, 13, 25 };

	HUD_electricity_resource = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 1115, 604, HUD_electricity_resource_size, true, true, false, this, HUD_resource_bar);

	SDL_Rect HUD_data_resource_size = { 687, 54, 16, 25 };

	HUD_data_resource = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 1115, 634, HUD_data_resource_size, true, true, false, this, HUD_resource_bar);

	SDL_Rect HUD_byte_resource_size = { 667, 55, 15, 24 };

	HUD_bytes_resources = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 1115, 664, HUD_byte_resource_size, true, true, false, this, HUD_resource_bar);

	//Data Store
	SDL_Rect HUD_text_data_resource_rect = { 737, 54, 13, 25 };
	HUD_data_resource_string = "0";
	HUD_data_resource_text = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1145, 604, HUD_text_data_resource_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, true, false, false, this, HUD_resource_bar, &HUD_data_resource_string);

	//Electricity Store
	SDL_Rect HUD_text_electricity_resource_rect = { 737, 54, 13, 25 };
	HUD_electricity_resource_string = "0";
	HUD_electricity_resource_text = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1145, 634, HUD_text_electricity_resource_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, true, false, false, this, HUD_resource_bar, &HUD_electricity_resource_string);

	//Bytes Store
	SDL_Rect HUD_text_bytes_resource_rect = { 737, 54, 13, 25 };
	HUD_bytes_resource_string = "0";
	HUD_bytes_resource_text = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1145, 664, HUD_text_bytes_resource_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, true, false, false, this, HUD_resource_bar, &HUD_bytes_resource_string);



	//*****______TOWNHALL______******
	//Townhall Bar
	SDL_Rect HUD_townhall_bar_size = { 20, 209, 798, 160 };

	HUD_townhall_bar = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 309, 560, HUD_townhall_bar_size, false, true, false, this, nullptr);

	// Title Townhall
	SDL_Rect HUD_text_townhall_rect = { 0, 0, 100, 20 };
	std::string HUD_title_townhall_string = "TOWNHALL";
	HUD_title_townhall = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 360, 582, HUD_text_townhall_rect, App->gui_manager->borgsquadcond_30, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_townhall_bar, &HUD_title_townhall_string);

	// HP Townhall


	// Description Townhall
	SDL_Rect HUD_text_townhall_descp_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_descp_string = "The main building of your base.";
	std::string HUD_townhall_descp_string2 = "If It gets destroyed,";
	std::string HUD_townhall_descp_string3 = "the game ends.";
	HUD_description_townhall = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 335, 635, HUD_text_townhall_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_townhall_bar, &HUD_townhall_descp_string);
	HUD_description_townhall2 = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 362, 649, HUD_text_townhall_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_townhall_bar, &HUD_townhall_descp_string2);
	HUD_description_townhall3 = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 379, 662, HUD_text_townhall_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_townhall_bar, &HUD_townhall_descp_string3);

	// Resources Upgrade Townhall
	HUD_parent_resources_upgrade_townhall = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_townhall_res_upg_townhall_size = { 707, 54, 13, 25 };

	HUD_resources_data_upgrade_townhall = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 780, 685, HUD_townhall_res_upg_townhall_size, false, true, false, this, HUD_parent_resources_upgrade_townhall);

	SDL_Rect HUD_townhall_res2_upg_townhall_size = { 687, 54, 16, 25 };

	HUD_resources_electricity_upgrade_townhall = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 880, 685, HUD_townhall_res2_upg_townhall_size, false, true, false, this, HUD_parent_resources_upgrade_townhall);

	SDL_Rect HUD_townhall_res3_upg_townhall_size = { 667, 55, 15, 24 };

	HUD_resources_bytes_upgrade_townhall = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 980, 685, HUD_townhall_res3_upg_townhall_size, false, true, false, this, HUD_parent_resources_upgrade_townhall);

	// Price Upgrade Townhall Data
	SDL_Rect HUD_text_townhall_price_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price_upgrade_string = "240";
	HUD_prices_data_upgrade_townhall = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 800, 687, HUD_text_townhall_price_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_townhall, &HUD_townhall_price_upgrade_string);

	// Price Upgrade Townhall Electricity
	SDL_Rect HUD_text_townhall_price2_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price2_upgrade_string = "20";
	HUD_prices_electricity_upgrade_townhall = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 900, 687, HUD_text_townhall_price2_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_townhall, &HUD_townhall_price2_upgrade_string);

	// Price Upgrade Townhall Bytes
	SDL_Rect HUD_text_townhall_price3_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price3_upgrade_string = "3";
	HUD_prices_bytes_upgrade_townhall = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1000, 687, HUD_text_townhall_price3_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_townhall, &HUD_townhall_price3_upgrade_string);

	// Upgrade Building
	SDL_Rect HUD_upgrade_townhall_size = { 0, 0, 117, 24 };
	SDL_Rect HUD_upgrade_townhall_idle = { 960, 99, 117, 24 };
	SDL_Rect HUD_upgrade_townhall_hover = { 960, 125, 117, 24 };
	SDL_Rect HUD_upgrade_townhall_clicked = { 960, 154, 117, 24 };

	HUD_upgrade_townhall = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 975, 577, false, true, false, this, HUD_townhall_bar
		, &HUD_upgrade_townhall_idle, &HUD_upgrade_townhall_hover, &HUD_upgrade_townhall_clicked);


	//******______GATHERER______******
	// Title Gatherer
	SDL_Rect HUD_text_gatherer_rect = { 0, 0, 100, 20 };
	std::string HUD_title_gatherer_string = "GATHERER";
	HUD_title_gatherer = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 370, 582, HUD_text_gatherer_rect, App->gui_manager->borgsquadcond_30, SDL_Color{ 182,255,106,0 }, false, false, false, this, nullptr, &HUD_title_gatherer_string);

	// Description Gatherer
	SDL_Rect HUD_text_getherer_descp_rect = { 0, 0, 100, 20 };
	std::string HUD_gatherer_descp_string = "Primary resource unit.";
	std::string HUD_gatherer_descp_string2 = "It doesn't attack. It is useful";
	std::string HUD_gatherer_descp_string3 = "for gathering resources.";
	HUD_description_gatherer = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 360, 635, HUD_text_getherer_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_gatherer, &HUD_gatherer_descp_string);
	HUD_description_gatherer = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 337, 649, HUD_text_getherer_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_gatherer, &HUD_gatherer_descp_string2);
	HUD_description_gatherer = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 355, 662, HUD_text_getherer_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_gatherer, &HUD_gatherer_descp_string3);

	//Gatherer Recruit Button
	SDL_Rect HUD_unit_gatherer_townhall_size = { 0, 0, 53, 50 };
	SDL_Rect HUD_unit_gatherer_townhall_idle = { 1095, 140, 53, 50 };
	SDL_Rect HUD_unit_gatherer_townhall_hover = { 1152, 140, 53, 50 };
	SDL_Rect HUD_unit_gatherer_townhall_clicked = { 1207, 140, 53, 50 }; 

	HUD_unit_gatherer_townhall = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 616, 610, false, true, false, this, HUD_townhall_bar
		, &HUD_unit_gatherer_townhall_idle, &HUD_unit_gatherer_townhall_hover, &HUD_unit_gatherer_townhall_clicked);


	// Resources Recruit Gatherer
	HUD_parent_resources_unit_townhall_gatherer = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });


	SDL_Rect HUD_townhall_res_unit_size = { 707, 54, 13, 25 };

	HUD_resources_data_unit_townhall_gatherer = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 780, 685, HUD_townhall_res_unit_size, false, true, false, this, HUD_parent_resources_unit_townhall_gatherer);

	SDL_Rect HUD_townhall_res2_unit_size = { 687, 54, 16, 25 };

	HUD_resources_electricity_unit_townhall_gatherer = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 880, 685, HUD_townhall_res2_unit_size, false, true, false, this, HUD_parent_resources_unit_townhall_gatherer);

	SDL_Rect HUD_townhall_res3_unit_size = { 667, 55, 15, 24 };

	HUD_resources_bytes_unit_townhall_gatherer = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 980, 685, HUD_townhall_res3_unit_size, false, true, false, this, HUD_parent_resources_unit_townhall_gatherer);


	// Price Gatherer Data
	SDL_Rect HUD_text_townhall_price_unit_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price_unit_string = "40";
	HUD_prices_data_unit_townhall_gatherer = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 800, 687, HUD_text_townhall_price_unit_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_townhall_gatherer, &HUD_townhall_price_unit_string);

	// Price Gatherer Electricity
	SDL_Rect HUD_text_townhall_price2_unit_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price2_unit_string = "0";
	HUD_prices_electricity_unit_townhall_gatherer = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 900, 687, HUD_text_townhall_price2_unit_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_townhall_gatherer, &HUD_townhall_price2_unit_string);

	// Price Gatherer Bytes
	SDL_Rect HUD_text_townhall_price3_unit_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price3_unit_string = "0";
	HUD_prices_bytes_unit_townhall_gatherer = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1000, 687, HUD_text_townhall_price3_unit_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_townhall_gatherer, &HUD_townhall_price3_unit_string);

	// Resources Upgrade Gatherer
	HUD_parent_resources_upgrade_unit_townhall_gatherer = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_townhall_res_upg_unit_size = { 707, 54, 13, 25 };

	HUD_resources_data_upgrade_unit_townhall_gatherer = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 780, 685, HUD_townhall_res_upg_unit_size, false, true, false, this, HUD_parent_resources_upgrade_unit_townhall_gatherer);

	SDL_Rect HUD_townhall_res2_upg_unit_size = { 687, 54, 16, 25 };

	HUD_resources_electricity_upgrade_unit_townhall_gatherer = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 880, 685, HUD_townhall_res2_upg_unit_size, false, true, false, this, HUD_parent_resources_upgrade_unit_townhall_gatherer);

	SDL_Rect HUD_townhall_res3_upg_unit_size = { 667, 55, 15, 24 };

	HUD_resources_bytes_upgrade_unit_townhall_gatherer = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 980, 685, HUD_townhall_res3_upg_unit_size, false, true, false, this, HUD_parent_resources_upgrade_unit_townhall_gatherer);

	// Price Upgrade Gatherer Data
	SDL_Rect HUD_text_townhall_price_unit_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price_unit_upgrade_string = "200";
	HUD_prices_upgrade_data_unit_townhall_gatherer = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 800, 687, HUD_text_townhall_price_unit_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_townhall_gatherer, &HUD_townhall_price_unit_upgrade_string);

	// Price Upgrade Gatherer Electricity
	SDL_Rect HUD_text_townhall_price2_unit_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price2_unit_upgrade_string = "10";
	HUD_prices_electricity_upgrade_unit_townhall_gatherer = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 900, 687, HUD_text_townhall_price2_unit_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_townhall_gatherer, &HUD_townhall_price2_unit_upgrade_string);

	// Price Upgrade Gatherer Bytes
	SDL_Rect HUD_text_townhall_price3_unit_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price3_unit_upgrade_string = "1";
	HUD_prices_electricity_unit_townhall_gatherer = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1000, 687, HUD_text_townhall_price3_unit_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_townhall_gatherer, &HUD_townhall_price3_unit_upgrade_string);

	// Upgrade Unit Button
	SDL_Rect HUD_unit_upgrade_townhall_size = { 0, 0, 50, 16 };
	SDL_Rect HUD_unit_upgrade_townhall_idle = { 899, 129, 50, 16 };
	SDL_Rect HUD_unit_upgrade_townhall_hover = { 899, 148, 50, 16 };
	SDL_Rect HUD_unit_upgrade_townhall_clicked = { 899, 167, 50, 16 };

	HUD_unit_upgrade_townhall_gatherer = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 556, 594, false, true, false, this, HUD_townhall_bar
		, &HUD_unit_upgrade_townhall_idle, &HUD_unit_upgrade_townhall_hover, &HUD_unit_upgrade_townhall_clicked);



	//*****_____SCOUT_____******
	// Title Scout
	SDL_Rect HUD_text_scout_rect = { 0, 0, 100, 20 };
	std::string HUD_title_scout_string = "SCOUT";
	HUD_title_scout = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 385, 582, HUD_text_scout_rect, App->gui_manager->borgsquadcond_30, SDL_Color{ 182,255,106,0 }, false, false, false, this, nullptr, &HUD_title_scout_string);

	// Description Scout
	SDL_Rect HUD_text_scout_descp_rect = { 0, 0, 100, 20 };
	std::string HUD_scout_descp_string = "Primary explore unit.";
	std::string HUD_scout_descp_string2 = "It is very fast. It is useful";
	std::string HUD_scout_descp_string3 = "for exploring new areas.";
	HUD_description_scout = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 355, 635, HUD_text_scout_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_scout, &HUD_scout_descp_string);
	HUD_description_scout = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 337, 649, HUD_text_scout_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_scout, &HUD_scout_descp_string2);
	HUD_description_scout = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 345, 662, HUD_text_scout_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_scout, &HUD_scout_descp_string3);

	//Scout
	SDL_Rect HUD_unit_scout_townhall_size = { 0, 0, 49, 50 };
	SDL_Rect HUD_unit_scout_townhall_idle = { 1095, 239, 49, 50 };
	SDL_Rect HUD_unit_scout_townhall_hover = { 1152, 239, 49, 50 };
	SDL_Rect HUD_unit_scout_townhall_clicked = { 1207, 239, 49, 50 };

	HUD_unit_scout_townhall = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 556, 610, false, true, false, this, HUD_townhall_bar
		, &HUD_unit_scout_townhall_idle, &HUD_unit_scout_townhall_hover, &HUD_unit_scout_townhall_clicked);

	// Resources Recruit Scout
	HUD_parent_resources_unit_townhall_scout = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });


	SDL_Rect HUD_townhall_res_unit2_size = { 707, 54, 13, 25 };

	HUD_resources_data_unit_townhall_scout = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 780, 685, HUD_townhall_res_unit2_size, false, true, false, this, HUD_parent_resources_unit_townhall_scout);

	SDL_Rect HUD_townhall_res2_unit2_size = { 687, 54, 16, 25 };

	HUD_resources_electricity_unit_townhall_scout = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 880, 685, HUD_townhall_res2_unit2_size, false, true, false, this, HUD_parent_resources_unit_townhall_scout);

	SDL_Rect HUD_townhall_res3_unit2_size = { 667, 55, 15, 24 };

	HUD_resources_bytes_unit_townhall_scout = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 980, 685, HUD_townhall_res3_unit2_size, false, true, false, this, HUD_parent_resources_unit_townhall_scout);

	// Price Scout Data
	SDL_Rect HUD_text_townhall_price_unit2_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price_unit2_string = "20";
	HUD_prices_data_unit_townhall_scout = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 800, 687, HUD_text_townhall_price_unit2_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_townhall_scout, &HUD_townhall_price_unit2_string);

	// Price Scout Electricity
	SDL_Rect HUD_text_townhall_price2_unit2_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price2_unit2_string = "0";
	HUD_prices_electricity_unit_townhall_gatherer = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 900, 687, HUD_text_townhall_price2_unit2_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_townhall_scout, &HUD_townhall_price2_unit2_string);

	// Price Scout Bytes
	SDL_Rect HUD_text_townhall_price3_unit2_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price3_unit2_string = "0";
	HUD_prices_bytes_unit_townhall_gatherer = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1000, 687, HUD_text_townhall_price3_unit2_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_townhall_scout, &HUD_townhall_price3_unit2_string);

	// Resources Upgrade Gatherer
	HUD_parent_resources_upgrade_unit_townhall_scout = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_townhall_res_upg_unit2_size = { 707, 54, 13, 25 };

	HUD_resources_data_upgrade_unit_townhall_scout = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 780, 685, HUD_townhall_res_upg_unit2_size, false, true, false, this, HUD_parent_resources_upgrade_unit_townhall_scout);

	SDL_Rect HUD_townhall_res2_upg_unit2_size = { 687, 54, 16, 25 };

	HUD_resources_electricity_upgrade_unit_townhall_scout = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 880, 685, HUD_townhall_res2_upg_unit2_size, false, true, false, this, HUD_parent_resources_upgrade_unit_townhall_scout);

	SDL_Rect HUD_townhall_res3_upg_unit2_size = { 667, 55, 15, 24 };

	HUD_resources_bytes_upgrade_unit_townhall_scout = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 980, 685, HUD_townhall_res3_upg_unit2_size, false, true, false, this, HUD_parent_resources_upgrade_unit_townhall_scout);

	// Price Upgrade Scout Data
	SDL_Rect HUD_text_townhall_price_unit2_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price_unit2_upgrade_string = "100";
	HUD_prices_upgrade_data_unit_townhall_scout = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 800, 687, HUD_text_townhall_price_unit2_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_townhall_scout, &HUD_townhall_price_unit2_upgrade_string);

	// Price Upgrade Scout Electricity
	SDL_Rect HUD_text_townhall_price2_unit2_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price2_unit2_upgrade_string = "5";
	HUD_prices_electricity_upgrade_unit_townhall_scout = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 900, 687, HUD_text_townhall_price2_unit2_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_townhall_scout, &HUD_townhall_price2_unit2_upgrade_string);

	// Price Upgrade Scout Bytes
	SDL_Rect HUD_text_townhall_price3_unit2_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_townhall_price3_unit2_upgrade_string = "1";
	HUD_prices_electricity_unit_townhall_scout = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1000, 687, HUD_text_townhall_price3_unit2_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_townhall_scout, &HUD_townhall_price3_unit2_upgrade_string);

	// Upgrade Unit Button
	SDL_Rect HUD_unit_upgrade_townhall_size2 = { 0, 0, 50, 16 };
	SDL_Rect HUD_unit_upgrade_townhall_idle2 = { 899, 129, 50, 16 };
	SDL_Rect HUD_unit_upgrade_townhall_hover2 = { 899, 148, 50, 16 };
	SDL_Rect HUD_unit_upgrade_townhall_clicked2 = { 899, 167, 50, 16 };

	HUD_unit_upgrade_townhall_scout = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 616, 594, false, true, false, this, HUD_townhall_bar
		, &HUD_unit_upgrade_townhall_idle2, &HUD_unit_upgrade_townhall_hover2, &HUD_unit_upgrade_townhall_clicked2);

	//*****_____BARRACKS_____******

	//Barracks Bar
	SDL_Rect HUD_barracks_bar_size = { 20, 209, 798, 160 };

	HUD_barracks_bar = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 309, 560, HUD_barracks_bar_size, false, true, false, this, nullptr);

	// Title Barracks
	SDL_Rect HUD_text_barracks_rect = { 0, 0, 100, 20 };
	std::string HUD_title_barracks_string = "BARRACKS";
	HUD_title_barracks = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 362, 582, HUD_text_barracks_rect, App->gui_manager->borgsquadcond_30, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_barracks_bar, &HUD_title_barracks_string);

	// HP Barracks

	// Description Barracks
	SDL_Rect HUD_text_barracks_descp_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_descp_string = "Trains different military units";
	std::string HUD_barracks_descp_string2 = "depending on the number of";
	std::string HUD_barracks_descp_string3 = "resources you acquire.";
	HUD_description_barracks = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 329, 635, HUD_text_barracks_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_barracks_bar, &HUD_barracks_descp_string);
	HUD_description_barracks2 = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 341, 649, HUD_text_barracks_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_barracks_bar, &HUD_barracks_descp_string2);
	HUD_description_barracks3 = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 359, 662, HUD_text_barracks_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_barracks_bar, &HUD_barracks_descp_string3);

	// Resources Upgrade Barracks
	HUD_parent_resources_upgrade_barracks = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_res_upg_barracks_size = { 707, 54, 13, 25 };

	HUD_resources_data_upgrade_barracks = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 780, 685, HUD_res_upg_barracks_size, false, true, false, this, HUD_parent_resources_upgrade_barracks);

	SDL_Rect HUD_res2_upg_barracks_size = { 687, 54, 16, 25 };

	HUD_resources_electricity_upgrade_barracks = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 880, 685, HUD_res2_upg_barracks_size, false, true, false, this, HUD_parent_resources_upgrade_barracks);

	SDL_Rect HUD_res3_upg_barracks_size = { 667, 55, 15, 24 };

	HUD_resources_bytes_upgrade_barracks = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 980, 685, HUD_res3_upg_barracks_size, false, true, false, this, HUD_parent_resources_upgrade_barracks);

	// Price Upgrade Barracks Data
	SDL_Rect HUD_text_barracks_price_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price_upgrade_string = "180";
	HUD_prices_data_upgrade_barracks = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 800, 687, HUD_text_barracks_price_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_barracks, &HUD_barracks_price_upgrade_string);

	// Price Upgrade Barracks Electricity
	SDL_Rect HUD_text_barracks_price2_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price2_upgrade_string = "25";
	HUD_prices_electricity_upgrade_barracks = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 900, 687, HUD_text_barracks_price2_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_barracks, &HUD_barracks_price2_upgrade_string);

	// Price Upgrade Barracks Bytes
	SDL_Rect HUD_text_barracks_price3_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price3_upgrade_string = "2";
	HUD_prices_bytes_upgrade_barracks = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1000, 687, HUD_text_barracks_price3_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_barracks, &HUD_barracks_price3_upgrade_string);

	// Upgrade Building
	SDL_Rect HUD_upgrade_barracks_size = { 0, 0, 117, 24 };
	SDL_Rect HUD_upgrade_barracks_idle = { 960, 99, 117, 24 };
	SDL_Rect HUD_upgrade_barracks_hover = { 960, 125, 117, 24 };
	SDL_Rect HUD_upgrade_barracks_clicked = { 960, 154, 117, 24 };

	HUD_upgrade_barracks = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 975, 577, false, true, false, this, HUD_barracks_bar
		, &HUD_upgrade_barracks_idle, &HUD_upgrade_barracks_hover, &HUD_upgrade_barracks_clicked);


	//*****_____INFANTRY_____*****
	// Title Infantry
	SDL_Rect HUD_text_infantry_rect = { 0, 0, 100, 20 };
	std::string HUD_title_infantry_string = "INFANTRY";
	HUD_title_infantry = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 370, 582, HUD_text_infantry_rect, App->gui_manager->borgsquadcond_30, SDL_Color{ 182,255,106,0 }, false, false, false, this, nullptr, &HUD_title_infantry_string);

	// Description Infantry
	SDL_Rect HUD_text_infranty_descp_rect = { 0, 0, 100, 20 };
	std::string HUD_infantry_descp_string = "Primary combat unit.";
	std::string HUD_infantry_descp_string2 = "It has a standard attck.";
	std::string HUD_infantry_descp_string3 = "Fight your enemies with these!";
	HUD_description_infantry = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 365, 635, HUD_text_infranty_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_infantry, &HUD_infantry_descp_string);
	HUD_description_infantry = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 347, 649, HUD_text_infranty_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_infantry, &HUD_infantry_descp_string2);
	HUD_description_infantry = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 329, 662, HUD_text_infranty_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_infantry, &HUD_infantry_descp_string3);

	// Unit Barracks Infantry Recruit Button
	SDL_Rect HUD_unit_infantry_barracks_size = { 0, 0, 53, 50 };
	SDL_Rect HUD_unit_infantry_barracks_idle = { 1095, 90, 53, 50 };
	SDL_Rect HUD_unit_infantry_barracks_hover = { 1152, 90, 53, 50 };
	SDL_Rect HUD_unit_infantry_barracks_clicked = { 1207, 90, 53, 50 };

	HUD_unit_infantry_barracks = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 556, 610, false, true, false, this, HUD_barracks_bar
		, &HUD_unit_infantry_barracks_idle, &HUD_unit_infantry_barracks_hover, &HUD_unit_infantry_barracks_clicked);

	// Resources Recruit Infantry
	HUD_parent_resources_unit_barracks_infantry = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_barracks_res_unit_size = { 707, 54, 13, 25 };

	HUD_resources_data_unit_barracks_infantry = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 780, 685, HUD_barracks_res_unit_size, false, true, false, this, HUD_parent_resources_unit_barracks_infantry);

	SDL_Rect HUD_barracks_res2_unit_size = { 687, 54, 16, 25 };

	HUD_resources_electricity_unit_barracks_infantry = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 880, 685, HUD_barracks_res2_unit_size, false, true, false, this, HUD_parent_resources_unit_barracks_infantry);

	SDL_Rect HUD_barracks_res3_unit_size = { 667, 55, 15, 24 };

	HUD_resources_bytes_unit_barracks_infantry = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 980, 685, HUD_barracks_res3_unit_size, false, true, false, this, HUD_parent_resources_unit_barracks_infantry);

	// Price Infantry Data
	SDL_Rect HUD_text_barracks_price_unit_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price_unit_string = "0";
	HUD_prices_data_unit_townhall_infantry = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 800, 687, HUD_text_barracks_price_unit_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_barracks_infantry, &HUD_barracks_price_unit_string);

	// Price Infantry Electricity
	SDL_Rect HUD_text_barracks_price2_unit_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price2_unit_string = "5";
	HUD_prices_electricity_unit_townhall_infantry = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 900, 687, HUD_text_barracks_price2_unit_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_barracks_infantry, &HUD_barracks_price2_unit_string);

	// Price Infantry Bytes
	SDL_Rect HUD_text_barracks_price3_unit_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price3_unit_string = "0";
	HUD_prices_bytes_unit_townhall_infantry = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1000, 687, HUD_text_barracks_price3_unit_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_barracks_infantry, &HUD_barracks_price3_unit_string);

	// Resources Upgrade Infantry
	HUD_parent_resources_upgrade_unit_barracks_infantry = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_barracks_res_upg_unit_size = { 707, 54, 13, 25 };

	HUD_resources_data_upgrade_unit_barracks_infantry = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 780, 685, HUD_barracks_res_upg_unit_size, false, true, false, this, HUD_parent_resources_upgrade_unit_barracks_infantry);

	SDL_Rect HUD_barracks_res2_upg_unit_size = { 687, 54, 16, 25 };

	HUD_resources_electricity_upgrade_unit_barracks_infantry = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 880, 685, HUD_barracks_res2_upg_unit_size, false, true, false, this, HUD_parent_resources_upgrade_unit_barracks_infantry);

	SDL_Rect HUD_barracks_res3_upg_unit_size = { 667, 55, 15, 24 };

	HUD_resources_bytes_upgrade_unit_barracks_infantry = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 980, 685, HUD_barracks_res3_upg_unit_size, false, true, false, this, HUD_parent_resources_upgrade_unit_barracks_infantry);

	// Price Upgrade Infantry Data
	SDL_Rect HUD_text_barracks_price_unit_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price_unit_upgrade_string = "100";
	HUD_prices_data_upgrade_unit_barracks_infantry = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 800, 687, HUD_text_barracks_price_unit_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_barracks_infantry, &HUD_barracks_price_unit_upgrade_string);

	// Price Upgrade Infantry Electricity
	SDL_Rect HUD_text_barracks_price2_unit_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price2_unit_upgrade_string = "5";
	HUD_prices_electricity_upgrade_unit_barracks_infantry = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 900, 687, HUD_text_barracks_price2_unit_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_barracks_infantry, &HUD_barracks_price2_unit_upgrade_string);

	// Price Upgrade Infantry Bytes
	SDL_Rect HUD_text_barracks_price3_unit_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price3_unit_upgrade_string = "1";
	HUD_prices_bytes_upgrade_unit_barracks_infantry = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1000, 687, HUD_text_barracks_price3_unit_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_barracks_infantry, &HUD_barracks_price3_unit_upgrade_string);

	// Upgrade Unit Button
	SDL_Rect HUD_unit_upgrade_barracks_size = { 0, 0, 50, 16 };
	SDL_Rect HUD_unit_upgrade_barracks_idle = { 899, 129, 50, 16 };
	SDL_Rect HUD_unit_upgrade_barracks_hover = { 899, 148, 50, 16 };
	SDL_Rect HUD_unit_upgrade_barracks_clicked = { 899, 167, 50, 16 };

	HUD_unit_upgrade_barracks_infantry = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 556, 594, false, true, false, this, HUD_barracks_bar
		, &HUD_unit_upgrade_barracks_idle, &HUD_unit_upgrade_barracks_hover, &HUD_unit_upgrade_barracks_clicked);


	//*****____HEAVY_____*****
	// Title Heavy
	SDL_Rect HUD_text_heavy_rect = { 0, 0, 100, 20 };
	std::string HUD_title_heavy_string = "HEAVY";
	HUD_title_heavy = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 385, 582, HUD_text_heavy_rect, App->gui_manager->borgsquadcond_30, SDL_Color{ 182,255,106,0 }, false, false, false, this, nullptr, &HUD_title_heavy_string);

	// Description Heavy
	SDL_Rect HUD_text_heavy_descp_rect = { 0, 0, 100, 20 };
	std::string HUD_heavy_descp_string = "Primary combat unit.";
	std::string HUD_heavy_descp_string2 = "It is more resistant, but slower.";
	std::string HUD_heavy_descp_string3 = "Destroy your enemies with these!";
	HUD_description_heavy = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 365, 635, HUD_text_heavy_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_heavy, &HUD_heavy_descp_string);
	HUD_description_heavy = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 327, 649, HUD_text_heavy_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_heavy, &HUD_heavy_descp_string2);
	HUD_description_heavy = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 325, 662, HUD_text_heavy_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_title_heavy, &HUD_heavy_descp_string3);

	// Unit Barracks Heavy
	SDL_Rect HUD_unit_heavy_barracks_size =     { 0, 0, 49, 50 };
	SDL_Rect HUD_unit_heavy_barracks_idle =     { 1095, 190, 49, 50 };
	SDL_Rect HUD_unit_heavy_barracks_hover =    { 1152, 190, 49, 50 };
	SDL_Rect HUD_unit_heavy_barracks_clicked =  { 1207, 190, 49, 50 };

	HUD_unit_heavy_barracks = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 616, 610, false, true, false, this, HUD_barracks_bar
		, &HUD_unit_heavy_barracks_idle, &HUD_unit_heavy_barracks_hover, &HUD_unit_heavy_barracks_clicked);


	// Resources Recruit Infantry
	HUD_parent_resources_unit_barracks_heavy = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_barracks_res_unit2_size = { 707, 54, 13, 25 };

	HUD_resources_data_unit_barracks_heavy = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 780, 685, HUD_barracks_res_unit2_size, false, true, false, this, HUD_parent_resources_unit_barracks_heavy);

	SDL_Rect HUD_barracks_res2_unit2_size = { 687, 54, 16, 25 };

	HUD_resources_electricity_unit_barracks_heavy = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 880, 685, HUD_barracks_res2_unit2_size, false, true, false, this, HUD_parent_resources_unit_barracks_heavy);

	SDL_Rect HUD_barracks_res3_unit2_size = { 667, 55, 15, 24 };

	HUD_resources_bytes_unit_barracks_heavy = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 980, 685, HUD_barracks_res3_unit2_size, false, true, false, this, HUD_parent_resources_unit_barracks_heavy);

	// Price Heavy Data
	SDL_Rect HUD_text_barracks_price_unit2_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price_unit2_string = "0";
	HUD_prices_data_unit_townhall_heavy = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 800, 687, HUD_text_barracks_price_unit2_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_barracks_heavy, &HUD_barracks_price_unit2_string);

	// Price Heavy Electricity
	SDL_Rect HUD_text_barracks_price2_unit2_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price2_unit2_string = "15";
	HUD_prices_electricity_unit_townhall_heavy = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 900, 687, HUD_text_barracks_price2_unit2_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_barracks_heavy, &HUD_barracks_price2_unit2_string);

	// Price Heavy Bytes
	SDL_Rect HUD_text_barracks_price3_unit2_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price3_unit2_string = "0";
	HUD_prices_bytes_unit_townhall_heavy = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1000, 687, HUD_text_barracks_price3_unit2_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_unit_barracks_heavy, &HUD_barracks_price3_unit2_string);

	// Resources Upgrade Infantry
	HUD_parent_resources_upgrade_unit_barracks_heavy = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	SDL_Rect HUD_barracks_res_upg_unit2_size = { 707, 54, 13, 25 };

	HUD_resources_data_upgrade_unit_barracks_heavy = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 780, 685, HUD_barracks_res_upg_unit2_size, false, true, false, this, HUD_parent_resources_upgrade_unit_barracks_heavy);

	SDL_Rect HUD_barracks_res2_upg_unit2_size = { 687, 54, 16, 25 };

	HUD_resources_electricity_upgrade_unit_barracks_heavy = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 880, 685, HUD_barracks_res2_upg_unit2_size, false, true, false, this, HUD_parent_resources_upgrade_unit_barracks_heavy);

	SDL_Rect HUD_barracks_res3_upg_unit2_size = { 667, 55, 15, 24 };

	HUD_resources_bytes_upgrade_unit_barracks_heavy = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 980, 685, HUD_barracks_res3_upg_unit2_size, false, true, false, this, HUD_parent_resources_upgrade_unit_barracks_heavy);

	// Price Upgrade Heavy Data
	SDL_Rect HUD_text_barracks_price_unit2_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price_unit2_upgrade_string = "180";
	HUD_prices_data_upgrade_unit_barracks_heavy = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 800, 687, HUD_text_barracks_price_unit2_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_barracks_heavy, &HUD_barracks_price_unit2_upgrade_string);

	// Price Upgrade Heavy Electricity
	SDL_Rect HUD_text_barracks_price2_unit2_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price2_unit2_upgrade_string = "30";
	HUD_prices_electricity_upgrade_unit_barracks_heavy = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 900, 687, HUD_text_barracks_price2_unit2_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_barracks_heavy, &HUD_barracks_price2_unit2_upgrade_string);

	// Price Upgrade Heavy Bytes
	SDL_Rect HUD_text_barracks_price3_unit2_upgrade_rect = { 0, 0, 100, 20 };
	std::string HUD_barracks_price3_unit2_upgrade_string = "1";
	HUD_prices_bytes_upgrade_unit_barracks_heavy = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1000, 687, HUD_text_barracks_price3_unit2_upgrade_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, false, false, false, this, HUD_parent_resources_upgrade_unit_barracks_heavy, &HUD_barracks_price3_unit2_upgrade_string);

	// Upgrade Unit Button
	SDL_Rect HUD_unit2_upgrade_barracks_size = { 0, 0, 50, 16 };
	SDL_Rect HUD_unit2_upgrade_barracks_idle = { 899, 129, 50, 16 };
	SDL_Rect HUD_unit2_upgrade_barracks_hover = { 899, 148, 50, 16 };
	SDL_Rect HUD_unit2_upgrade_barracks_clicked = { 899, 167, 50, 16 };

	HUD_unit_upgrade_barracks_heavy = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 616, 594, false, true, false, this, HUD_barracks_bar
		, &HUD_unit2_upgrade_barracks_idle, &HUD_unit2_upgrade_barracks_hover, &HUD_unit2_upgrade_barracks_clicked);

	//*****_____ENEMY_____*****
	//Enemy Down Bar
	SDL_Rect HUD_enemy_townhall_bar_size = { 20, 209, 798, 160 };

	HUD_enemy_townhall_bar = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 309, 560, HUD_enemy_townhall_bar_size, false, true, false, this, nullptr);

	// Title Enemy Townhall
	SDL_Rect HUD_enemy_text_townhall_rect = { 0, 0, 100, 20 };
	std::string HUD_enemy_title_townhall_string = "TOWNHALL";
	HUD_enemy_title_townhall = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 360, 582, HUD_enemy_text_townhall_rect, App->gui_manager->borgsquadcond_30, SDL_Color{ 255,0,0,0 }, false, false, false, this, HUD_enemy_townhall_bar, &HUD_enemy_title_townhall_string);

	// Description Enemy Townhall
	SDL_Rect HUD_enemy_text_townhall_descp_rect = { 0, 0, 100, 20 };
	std::string HUD_enemy_townhall_descp_string = "The main building of the base.";
	std::string HUD_enemy_townhall_descp_string2 = "Destroy It to win";
	std::string HUD_enemy_townhall_descp_string3 = "the game.";
	HUD_enemy_description_townhall = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 335, 635, HUD_enemy_text_townhall_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 255,0,0,0 }, false, false, false, this, HUD_enemy_title_townhall, &HUD_enemy_townhall_descp_string);
	HUD_enemy_description_townhall = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 362, 649, HUD_enemy_text_townhall_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 255,0,0,0 }, false, false, false, this, HUD_enemy_title_townhall, &HUD_enemy_townhall_descp_string2);
	HUD_enemy_description_townhall = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 385, 662, HUD_enemy_text_townhall_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 255,0,0,0 }, false, false, false, this, HUD_enemy_title_townhall, &HUD_enemy_townhall_descp_string3);

	// Barracks
	//Enemy Barracks Bar
	SDL_Rect HUD_enemy_barracks_bar_size = { 20, 209, 798, 160 };

	HUD_enemy_barracks_bar = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 309, 560, HUD_enemy_barracks_bar_size, false, true, false, this, nullptr);

	// Title Enemy Barracks
	SDL_Rect HUD_enemy_text_barracks_rect = { 0, 0, 100, 20 };
	std::string HUD_enemy_title_barracks_string = "BARRACKS";
	HUD_enemy_title_barracks = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 362, 582, HUD_enemy_text_barracks_rect, App->gui_manager->borgsquadcond_30, SDL_Color{ 255,0,0,0 }, false, false, false, this, HUD_enemy_barracks_bar, &HUD_enemy_title_barracks_string);

	// Description Barracks
	SDL_Rect HUD_enemy_text_barracks_descp_rect = { 0, 0, 100, 20 };
	std::string HUD_enemy_barracks_descp_string = "Trains different military units";
	std::string HUD_enemy_barracks_descp_string2 = "depending on the number of";
	std::string HUD_enemy_barracks_descp_string3 = "resources the enemy acquire.";
	HUD_enemy_description_barracks = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 329, 635, HUD_enemy_text_barracks_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 255,0,0,0 }, false, false, false, this, HUD_enemy_title_barracks, &HUD_enemy_barracks_descp_string);
	HUD_enemy_description_barracks = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 341, 649, HUD_enemy_text_barracks_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 255,0,0,0 }, false, false, false, this, HUD_enemy_title_barracks, &HUD_enemy_barracks_descp_string2);
	HUD_enemy_description_barracks = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 339, 662, HUD_enemy_text_barracks_descp_rect, App->gui_manager->borgsquadcond_12, SDL_Color{ 255,0,0,0 }, false, false, false, this, HUD_enemy_title_barracks, &HUD_enemy_barracks_descp_string3);



	// *****______HUD Quests_____*****

	//Back
	SDL_Rect HUD_missions_back_size = { 25, 400, 390, 226 };

	HUD_missions_background = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 1302, 339, HUD_missions_back_size, true, true, false, this, nullptr);

	//Tab
	SDL_Rect HUD_missions_tab_size = { 0, 0, 30, 81 };
	SDL_Rect HUD_missions_tab_idle = { 780, 117, 30, 81 };
	SDL_Rect HUD_missions_tab_hover = { 813, 117, 30, 81 };
	SDL_Rect HUD_missions_tab_clicked = { 846, 117, 30, 81 };

	HUD_missions_tab = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 1252, 369, true, true, false, this, nullptr
		, &HUD_missions_tab_idle, &HUD_missions_tab_hover, &HUD_missions_tab_clicked);


	//Tab close 
	SDL_Rect HUD_missions_tab_close_size = { 0, 0, 30, 81 };
	SDL_Rect HUD_missions_tab_close_idle = { 677, 117, 30, 81 };
	SDL_Rect HUD_missions_tab_close_hover = { 710, 117, 30, 81 };
	SDL_Rect HUD_missions_tab_close_clicked = { 743, 117, 30, 81 };

	HUD_missions_tab_close = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 1252, 369, false, true, false, this, nullptr
		, &HUD_missions_tab_close_idle, &HUD_missions_tab_close_hover, &HUD_missions_tab_close_clicked);


	//Title 
	SDL_Rect HUD_missions_text_title_rect = { 0, 0, 100, 20 };
	std::string HUD_missions_title_string = "QUESTS";
	HUD_missions_title = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1345, 352, HUD_missions_text_title_rect, App->gui_manager->borgsquadcond_25, SDL_Color{ 182,255,106,0 }, true, false, false, this, HUD_missions_background, &HUD_missions_title_string);

	//Primary Title
	SDL_Rect HUD_missions_title_primary_title_rect = { 0, 0, 100, 20 };
	std::string HUD_missions_title_primary_string = "MAIN QUEST";
	HUD_missions_title_primary = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1345, 377, HUD_missions_title_primary_title_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, true, false, false, this, HUD_missions_background, &HUD_missions_title_primary_string);

	//Main Quest
	SDL_Rect HUD_missions_primary_quest_rect = { 0, 0, 100, 20 };
	std::string HUD_missions_primary_quest_string = "Destroy the enemy base";
	HUD_missions_primary_quest = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1365, 405, HUD_missions_primary_quest_rect, App->gui_manager->borgsquadcond_15, SDL_Color{ 182,255,106,0 }, true, false, false, this, HUD_missions_background, &HUD_missions_primary_quest_string);

	//Secondary Title
	SDL_Rect HUD_missions_title_secondary_title_rect = { 0, 0, 100, 20 };
	std::string HUD_missions_title_secondary_string = "SIDE QUESTS";
	HUD_missions_title_side = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1350, 430, HUD_missions_title_secondary_title_rect, App->gui_manager->borgsquadcond_20, SDL_Color{ 182,255,106,0 }, true, false, false, this, HUD_missions_background, &HUD_missions_title_secondary_string);

	//Side Quests
	SDL_Rect HUD_missions_side_quest_rect = { 0, 0, 100, 20 };
	std::string HUD_missions_side_quest_string = "Recollect 200 Data & 15 Electricty";
	HUD_missions_title_side = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1365, 455, HUD_missions_side_quest_rect, App->gui_manager->borgsquadcond_15, SDL_Color{ 182,255,106,0 }, true, false, false, this, HUD_missions_background, &HUD_missions_side_quest_string);

	SDL_Rect HUD_missions_side_quest2_rect = { 0, 0, 100, 20 };
	std::string HUD_missions_side_quest2_string = "Recollect 3 bytes";
	HUD_missions_title_side = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1365, 480, HUD_missions_side_quest2_rect, App->gui_manager->borgsquadcond_15, SDL_Color{ 182,255,106,0 }, true, false, false, this, HUD_missions_background, &HUD_missions_side_quest2_string);

	SDL_Rect HUD_missions_side_quest3_rect = { 0, 0, 100, 20 };
	std::string HUD_missions_side_quest3_string = "Destroy 10 enemies";
	HUD_missions_title_side = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 1365, 505, HUD_missions_side_quest3_rect, App->gui_manager->borgsquadcond_15, SDL_Color{ 182,255,106,0 }, true, false, false, this, HUD_missions_background, &HUD_missions_side_quest3_string);

	//Quests boxes

	SDL_Rect HUD_missions_checkbox_in_progress_bar_size = { 642, 112, 16, 16 };
	SDL_Rect HUD_missions_checkbox_in_progress_bar_size_on = { 624, 112, 16, 16 };


	HUD_missions_checkbox_in_progress_main_quest_on = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 1345, 405, HUD_missions_checkbox_in_progress_bar_size_on, false, true, false, this, HUD_missions_background);
	HUD_missions_checkbox_in_progress_main_quest_off = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 1345, 405, HUD_missions_checkbox_in_progress_bar_size, true, true, false, this, HUD_missions_background);

	HUD_missions_checkbox_in_progress_side_quest_on = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 1345, 455, HUD_missions_checkbox_in_progress_bar_size_on, false, true, false, this, HUD_missions_background);
	HUD_missions_checkbox_in_progress_side_quest_off = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 1345, 455, HUD_missions_checkbox_in_progress_bar_size, true, true, false, this, HUD_missions_background);

	HUD_missions_checkbox_in_progress_side_quest2_on = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 1345, 480, HUD_missions_checkbox_in_progress_bar_size_on, false, true, false, this, HUD_missions_background);
	HUD_missions_checkbox_in_progress_side_quest2_off = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 1345, 480, HUD_missions_checkbox_in_progress_bar_size, true, true, false, this, HUD_missions_background);

	HUD_missions_checkbox_in_progress_side_quest3_on = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 1345, 505, HUD_missions_checkbox_in_progress_bar_size_on, false, true, false, this, HUD_missions_background);
	HUD_missions_checkbox_in_progress_side_quest3_off = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::IMAGE, 1345, 505, HUD_missions_checkbox_in_progress_bar_size, true, true, false, this, HUD_missions_background);


	// *****_____HUD dialogs_____****

	App->dialog_manager->LoadDialog();

	//Skip tutorial
	SDL_Rect HUD_dialogs_skip_tutorial_size = { 0, 0, 131, 15 };
	SDL_Rect HUD_dialogs_skip_tutorial_idle = { 0, 164, 131, 15 };
	SDL_Rect HUD_dialogs_skip_tutorial_hover = { 135, 164, 131, 15 };
	SDL_Rect HUD_dialogs_skip_tutorial_clicked = { 269, 164, 131, 15 };

	HUD_dialogs_skip_tutorial = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 110, 145, true, true, false, this, nullptr
		, &HUD_dialogs_skip_tutorial_idle, &HUD_dialogs_skip_tutorial_hover, &HUD_dialogs_skip_tutorial_clicked);


	//************________EXTRAS____________*********
	// God_Mode
	SDL_Rect HUD_text_God = { 0, 0, 100, 20 };
	std::string HUD_God_string = "God Mode Activated";
	God_Mode_Activated = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 950, 2, HUD_text_God, App->gui_manager->borgsquadcond_30, SDL_Color{ 255,255,0,0 }, false, false, false, this, nullptr, &HUD_God_string);

	//Saving
	SDL_Rect HUD_text_Save = { 0, 0, 100, 20 };
	std::string HUD_Save_string = "Your Game Has Been Succesfully Saved";
	Saving = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, -500, 32, HUD_text_Save, App->gui_manager->borgsquadcond_20, SDL_Color{ 255,255,0,0 }, false, false, false, this, nullptr, &HUD_Save_string);

}

void GameplayScene::LoadInGameOptionsMenu()
{
	std::string borgsquadcond = App->config_file.child("config").child("gui").child("fonts").child("borgsquadcond").attribute("path").as_string();
	
	// Options Menu
	SDL_Rect in_game_text_rect = { 0, 0, 100, 20 };
	in_game_options_parent = (GuiImage*)App->gui_manager->CreateImage(GUI_ELEMENT_TYPE::EMPTY, 0, 0, SDL_Rect{ 0,0,1,1 });

	// Options
	std::string in_game_title_string = "Options Menu";
	in_game_options_text = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 445, 165, in_game_text_rect, App->gui_manager->borgsquadcond_50, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, in_game_options_parent, &in_game_title_string);

	//Music
	std::string in_game_music_string = "Music";
	in_game_music_text = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 487, 230, in_game_text_rect, App->gui_manager->borgsquadcond_30, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, in_game_options_parent, &in_game_music_string);

	SDL_Rect in_game_thumb_rect = { 930,2,18,31 };
	SDL_Rect in_game_scrollbar_rect = { 743,3,180,15 };

	in_game_music_scrollbar = (GuiScrollbar*)App->gui_manager->CreateScrollbar(GUI_ELEMENT_TYPE::SCROLLBAR, 600, 235, in_game_scrollbar_rect, in_game_thumb_rect
		, iPoint(20, -7), in_game_scrollbar_rect, 20.0f, true, false, true, false, false, false);
	in_game_music_scrollbar->parent = in_game_options_parent;

	//SFX
	std::string sfx_string = "SFX";
	in_game_sfx_text = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 491, 264, in_game_text_rect, App->gui_manager->borgsquadcond_30, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, in_game_options_parent, &sfx_string);
	in_game_sfx_scrollbar = (GuiScrollbar*)App->gui_manager->CreateScrollbar(GUI_ELEMENT_TYPE::SCROLLBAR, 600, 275, in_game_scrollbar_rect, in_game_thumb_rect
		, iPoint(20, -7), in_game_scrollbar_rect, 20.0f, true, false, false, false, false, false);
	in_game_sfx_scrollbar->parent = in_game_options_parent;

	//screen size
	std::string in_game_resolution_string = "screen";
	in_game_resolution_text = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 448, 301, in_game_text_rect, App->gui_manager->borgsquadcond_30, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, in_game_options_parent, &in_game_resolution_string);

	//Remapping

	//Fullscreen
	SDL_Rect in_game_text_fullscreen_rect = { 0, 0, 100, 20 };
	std::string in_game_fullscreen_string = "fullscreen";
	in_game_fullscreen_text = (GuiText*)App->gui_manager->CreateText(GUI_ELEMENT_TYPE::TEXT, 410, 336, in_game_text_fullscreen_rect, App->gui_manager->borgsquadcond_30, SDL_Color{ 255,255,0,0 }, true, false, false, nullptr, in_game_options_parent, &in_game_fullscreen_string);

	SDL_Rect in_game_opt_fullscreen_off_size = { 0, 0, 29, 24 };
	SDL_Rect in_game_opt_fullscreen_off_idle = { 744, 18, 29, 24 };
	SDL_Rect in_game_opt_fullscreen_off_hover = { 744, 18, 29, 24 };
	SDL_Rect in_game_opt_fullscreen_off_clicked = { 744, 18, 29, 24 };

	in_game_fullscreen_off = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 605, 339, true, true, false, this, in_game_options_parent
		, &in_game_opt_fullscreen_off_idle, &in_game_opt_fullscreen_off_hover, &in_game_opt_fullscreen_off_clicked);

	SDL_Rect in_game_opt_fullscreen_on_size = { 0, 0, 29, 24 };
	SDL_Rect in_game_opt_fullscreen_on_idle = { 788, 18, 29, 24 };
	SDL_Rect in_game_opt_fullscreen_on_hover = { 788, 18, 29, 24 };
	SDL_Rect in_game_opt_fullscreen_on_clicked = { 788, 18, 29, 24 };

	in_game_fullscreen_on = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 605, 339, false, true, false, this, nullptr
		, &in_game_opt_fullscreen_on_idle, &in_game_opt_fullscreen_on_clicked);
	
		//Back button
	SDL_Rect in_game_back_button_size = { 0, 0, 45, 33 };
	SDL_Rect in_game_back_button_idle = { 0, 103, 45, 33 };
	SDL_Rect in_game_back_button_hover = { 57, 103, 45, 33 };
	SDL_Rect in_game_back_button_clicked = { 114, 103, 45, 33 };

	in_game_back_button = (GuiButton*)App->gui_manager->CreateButton(GUI_ELEMENT_TYPE::BUTTON, 430, 445, true, true, false, this, in_game_options_parent
		, &in_game_back_button_idle, &in_game_back_button_hover, &in_game_back_button_clicked);
}

void GameplayScene::OnEventCall(GuiElement* element, GUI_EVENT ui_event)
{
	// In_game menu

	if (element == in_game_continue_button && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Continue
		if (App->pause)
		{
			App->pause = false;
		}

		iPoint origin		= { 380, in_game_background->GetScreenPos().y };
		iPoint destination	= { 1281, in_game_background->GetScreenPos().y };

		App->gui_manager->CreateSlideAnimation(in_game_background, 0.5f, true, origin, destination);

		App->audio->PlayFx(App->gui_manager->new_game_button_clicked_fx, 0);
	}

	if (element == in_game_options_button && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Options
		App->audio->PlayFx(App->gui_manager->options_button_clicked_fx, 0);

		App->gui_manager->SetElementsVisibility(in_game_continue_button, false);							// Deactivate Pause Menu // THIS (?)
		App->gui_manager->SetElementsVisibility(in_game_options_button, false);
		App->gui_manager->SetElementsVisibility(in_game_exit_button, false);
		App->gui_manager->SetElementsVisibility(in_game_back_to_menu, false);
		App->gui_manager->SetElementsVisibility(in_game_title_text, false);
		App->gui_manager->SetElementsVisibility(in_game_save_button, false);

		App->gui_manager->SetElementsVisibility(in_game_options_parent, true);

		App->gui_manager->CreateSlideAnimation(in_game_options_text, 0.5f, false, iPoint(2000, in_game_options_text->GetScreenPos().y), iPoint(445, in_game_options_text->GetScreenPos().y));
		App->gui_manager->CreateSlideAnimation(in_game_music_text, 0.5f, false, iPoint(2000, in_game_music_text->GetScreenPos().y), iPoint(487, in_game_music_text->GetScreenPos().y));
		App->gui_manager->CreateSlideAnimation(in_game_sfx_text, 0.5f, false, iPoint(2000, in_game_sfx_text->GetScreenPos().y), iPoint(491, in_game_sfx_text->GetScreenPos().y));
		App->gui_manager->CreateSlideAnimation(in_game_resolution_text, 0.5f, false, iPoint(2000, in_game_resolution_text->GetScreenPos().y), iPoint(448, in_game_resolution_text->GetScreenPos().y));
		App->gui_manager->CreateSlideAnimation(in_game_fullscreen_text, 0.5f, false, iPoint(2000, in_game_fullscreen_text->GetScreenPos().y), iPoint(410, in_game_fullscreen_text->GetScreenPos().y));
		App->gui_manager->CreateSlideAnimation(in_game_fullscreen_off, 0.5f, false, iPoint(2000, in_game_fullscreen_off->GetScreenPos().y), iPoint(605, in_game_fullscreen_off->GetScreenPos().y));
		App->gui_manager->CreateSlideAnimation(in_game_back_button, 0.5f, false, iPoint(2000, in_game_back_button->GetScreenPos().y), iPoint(430, in_game_back_button->GetScreenPos().y));
	}

	if (element == in_game_back_button && ui_event == GUI_EVENT::UNCLICKED)
	{
		App->audio->PlayFx(App->gui_manager->back_button_clicked_fx, 0);

		App->gui_manager->SetElementsVisibility(in_game_options_parent, false);

		App->gui_manager->SetElementsVisibility(in_game_continue_button, true);							// Activate Pause menu	// THIS (?)
		App->gui_manager->SetElementsVisibility(in_game_options_button, true);
		App->gui_manager->SetElementsVisibility(in_game_exit_button, true);
		App->gui_manager->SetElementsVisibility(in_game_back_to_menu, true);
		App->gui_manager->SetElementsVisibility(in_game_title_text, true);
		App->gui_manager->SetElementsVisibility(in_game_save_button, true);

		App->gui_manager->CreateSlideAnimation(in_game_continue_button, 0.5f, false, iPoint(-100, in_game_continue_button->GetScreenPos().y), iPoint(555, in_game_continue_button->GetScreenPos().y));
		App->gui_manager->CreateSlideAnimation(in_game_options_button, 0.5f, false, iPoint(-100, in_game_options_button->GetScreenPos().y), iPoint(567, in_game_options_button->GetScreenPos().y));
		App->gui_manager->CreateSlideAnimation(in_game_exit_button, 0.5f, false, iPoint(-100, in_game_exit_button->GetScreenPos().y), iPoint(596, in_game_exit_button->GetScreenPos().y));
		App->gui_manager->CreateSlideAnimation(in_game_title_text, 0.5f, false, iPoint(-100, in_game_title_text->GetScreenPos().y), iPoint(469, in_game_title_text->GetScreenPos().y));
		App->gui_manager->CreateSlideAnimation(in_game_back_to_menu, 0.5f, false, iPoint(-100, in_game_back_to_menu->GetScreenPos().y), iPoint(541, in_game_back_to_menu->GetScreenPos().y));
		App->gui_manager->CreateSlideAnimation(in_game_save_button, 0.5f, false, iPoint(-100, in_game_save_button->GetScreenPos().y), iPoint(596, in_game_save_button->GetScreenPos().y));
	}

	if (element == in_game_back_to_menu && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Back to menu
		App->audio->PlayFx(App->gui_manager->main_menu_button_clicked_fx, 0);

		transition_to_main_menu_scene = true;
	}

	if (element == in_game_save_button && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Save Game
		App->audio->PlayFx(App->gui_manager->standard_button_clicked_fx, 0);

		if (App->scene_manager->gameplay_scene->tutorial.tutorial_state == TutorialState::NOT_ACTIVE)
		{
			App->SaveGame("save_game.xml");
			App->player->has_saved = true;
		}

		App->gui_manager->SetElementsVisibility(Saving, true);

		App->gui_manager->CreateSlideAnimation(Saving, 8.0f, false, iPoint(-100, Saving->GetScreenPos().y), iPoint(845, Saving->GetScreenPos().y));

		saved_text_active = true;
	}

	if (element == in_game_exit_button && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Exit
		escape = false;
	}

	if (element == in_game_fullscreen_off && ui_event == GUI_EVENT::UNCLICKED)
	{
		App->gui_manager->SetElementsVisibility(in_game_fullscreen_on, true);
		App->gui_manager->SetElementsVisibility(in_game_fullscreen_off, false);
	}

	if (element == in_game_fullscreen_on && ui_event == GUI_EVENT::UNCLICKED)
	{
		App->win->ToggleFullscreen();
		App->gui_manager->SetElementsVisibility(in_game_fullscreen_on, false);
		App->gui_manager->SetElementsVisibility(in_game_fullscreen_off, true);
	}


	// HUD
	if (element == HUD_group_button && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Group
		App->player->SelectAllyUnits();
		App->audio->PlayFx(App->gui_manager->standard_button_clicked_fx, 0);
	}

	if (element == HUD_pause_button && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Pause
		SwitchPauseMenuMode();
		
		//App->gui->SetElementsVisibility(HUD_pause_button, false);			
		//App->gui->SetElementsVisibility(HUD_play_button, true);	
		//element->ui_event = GUI_EVENT::IDLE;
	}

	//if (element == HUD_play_button && ui_event == GUI_EVENT::UNCLICKED)
	//{
	//	// Play
	//	App->pause = false;
	//	App->audio->PlayFx(App->gui->standard_button_clicked_fx, 0);
	//	App->gui->SetElementsVisibility(HUD_play_button, false);
	//	App->gui->SetElementsVisibility(HUD_pause_button, true);
	//	
	//}

	if (element == HUD_home_button && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Home
		App->render->camera.x = App->player->original_camera_position.x;										// Use N_Lerp?
		App->render->camera.y = App->player->original_camera_position.y;

		App->audio->PlayFx(App->gui_manager->standard_button_clicked_fx, 0);
	}


	// *****_____Townhall bar_____*****
	//Upgrade Townhall
	if (element == HUD_upgrade_townhall && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Upgrade Townhall
		BuildingUpgrade();
	}
	//Price Upgrade Townhall
	if (element == HUD_upgrade_townhall && ui_event == GUI_EVENT::HOVER)
	{
		// Price to upgrade townhall
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_upgrade_townhall, true);
	}
	if (element == HUD_upgrade_townhall && ui_event == GUI_EVENT::UNHOVER)
	{
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_upgrade_townhall, false);
	}

	//****_____Gatherer_____*****
	//Recruit Gatherer
	if (element == HUD_unit_gatherer_townhall && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Recruit Unit
		SpawnAllyUnit(ENTITY_TYPE::GATHERER);

	}
	//Price to recruit
	if (element == HUD_unit_gatherer_townhall && ui_event == GUI_EVENT::HOVER)
	{
		// Price to recruit
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_unit_townhall_gatherer, true);
		App->gui_manager->SetElementsVisibility(HUD_title_townhall, false);
		App->gui_manager->SetElementsVisibility(HUD_description_townhall, false);
		App->gui_manager->SetElementsVisibility(HUD_description_townhall2, false);
		App->gui_manager->SetElementsVisibility(HUD_description_townhall3, false);
		App->gui_manager->SetElementsVisibility(HUD_title_gatherer, true);
	}
	if (element == HUD_unit_gatherer_townhall && ui_event == GUI_EVENT::UNHOVER)
	{
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_unit_townhall_gatherer, false);
		App->gui_manager->SetElementsVisibility(HUD_title_townhall, true);
		App->gui_manager->SetElementsVisibility(HUD_description_townhall, true);
		App->gui_manager->SetElementsVisibility(HUD_description_townhall2, true);
		App->gui_manager->SetElementsVisibility(HUD_description_townhall3, true);
		App->gui_manager->SetElementsVisibility(HUD_title_gatherer, false);
	}
	//Upgrade Gatherer
	if (element == HUD_unit_upgrade_townhall_gatherer && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Upgrade Unit
		UnitUpgrade(0);
		
	}
	//Price to upgrade Gatherer
	if (element == HUD_unit_upgrade_townhall_gatherer && ui_event == GUI_EVENT::HOVER)
	{
		// Price to upgrade unit
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_upgrade_unit_townhall_gatherer, true);
	}
	if (element == HUD_unit_upgrade_townhall_gatherer && ui_event == GUI_EVENT::UNHOVER)
	{
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_upgrade_unit_townhall_gatherer, false);
	}

	//*****_____Scout_____*****
	//Recruit Scout
	if (element == HUD_unit_scout_townhall && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Recruit Unit
		SpawnAllyUnit(ENTITY_TYPE::SCOUT);

	}
	//Price to recruit
	if (element == HUD_unit_scout_townhall && ui_event == GUI_EVENT::HOVER)
	{
		// Price to recruit
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_unit_townhall_scout, true);
		App->gui_manager->SetElementsVisibility(HUD_title_townhall, false);
		App->gui_manager->SetElementsVisibility(HUD_description_townhall, false);
		App->gui_manager->SetElementsVisibility(HUD_description_townhall2, false);
		App->gui_manager->SetElementsVisibility(HUD_description_townhall3, false);
		App->gui_manager->SetElementsVisibility(HUD_title_scout, true);
	}
	if (element == HUD_unit_scout_townhall && ui_event == GUI_EVENT::UNHOVER)
	{
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_unit_townhall_scout, false);
		App->gui_manager->SetElementsVisibility(HUD_title_townhall, true);
		App->gui_manager->SetElementsVisibility(HUD_description_townhall, true);
		App->gui_manager->SetElementsVisibility(HUD_description_townhall2,true);
		App->gui_manager->SetElementsVisibility(HUD_description_townhall3,true);
		App->gui_manager->SetElementsVisibility(HUD_title_scout, false);
	}
	//Upgrade scout
	if (element == HUD_unit_upgrade_townhall_scout && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Upgrade Unit
		UnitUpgrade(2);
		
	}
	//Price to upgrade scout
	if (element == HUD_unit_upgrade_townhall_scout && ui_event == GUI_EVENT::HOVER)
	{
		// Price to upgrade unit
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_upgrade_unit_townhall_scout, true);
	}
	if (element == HUD_unit_upgrade_townhall_scout && ui_event == GUI_EVENT::UNHOVER)
	{
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_upgrade_unit_townhall_scout, false);
	}

	// // *****_____Barracks bar_____*****
	if (element == HUD_upgrade_barracks && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Upgrade Barracks
		// Code to upgrade barracks
		BuildingUpgrade();
		
	}

	if (element == HUD_upgrade_barracks && ui_event == GUI_EVENT::HOVER)
	{
		// Price to upgrade townhall
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_upgrade_barracks, true);
	}
	if (element == HUD_upgrade_barracks && ui_event == GUI_EVENT::UNHOVER)
	{
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_upgrade_barracks, false);
	}

	//*****_____Infantry_____*****
	//Recruit Infantry
	if (element == HUD_unit_infantry_barracks && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Recruit Unit
		SpawnAllyUnit(ENTITY_TYPE::INFANTRY);
	}
	//Price Recruit Heavy
	if (element == HUD_unit_infantry_barracks && ui_event == GUI_EVENT::HOVER)
	{
		// Price to recruit
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_unit_barracks_infantry, true);
		App->gui_manager->SetElementsVisibility(HUD_title_barracks, false);
		App->gui_manager->SetElementsVisibility(HUD_description_barracks, false);
		App->gui_manager->SetElementsVisibility(HUD_description_barracks2, false);
		App->gui_manager->SetElementsVisibility(HUD_description_barracks3, false);
		App->gui_manager->SetElementsVisibility(HUD_title_infantry, true);
	}
	if (element == HUD_unit_infantry_barracks && ui_event == GUI_EVENT::UNHOVER)
	{
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_unit_barracks_infantry, false);
		App->gui_manager->SetElementsVisibility(HUD_title_barracks, true);
		App->gui_manager->SetElementsVisibility(HUD_description_barracks, true);
		App->gui_manager->SetElementsVisibility(HUD_description_barracks2, true);
		App->gui_manager->SetElementsVisibility(HUD_description_barracks3, true);
		App->gui_manager->SetElementsVisibility(HUD_title_infantry, false);
	}
	//Upgrade Infantry
	if (element == HUD_unit_upgrade_barracks_infantry && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Upgrade Unit
		UnitUpgrade(1);
		
	}
	//Price Upgrade Infantry
	if (element == HUD_unit_upgrade_barracks_infantry && ui_event == GUI_EVENT::HOVER)
	{
		// Price to upgrade unit
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_upgrade_unit_barracks_infantry, true);
	}
	if (element == HUD_unit_upgrade_barracks_infantry && ui_event == GUI_EVENT::UNHOVER)
	{
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_upgrade_unit_barracks_infantry, false);
	}

	//*****_____Heavy_____*****
	//Recruit Heavy
	if (element == HUD_unit_heavy_barracks && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Recruit Unit
		SpawnAllyUnit(ENTITY_TYPE::HEAVY);
	}
	//Price Recruit Heavy
	if (element == HUD_unit_heavy_barracks && ui_event == GUI_EVENT::HOVER)
	{
		// Price to recruit
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_unit_barracks_heavy, true);
		App->gui_manager->SetElementsVisibility(HUD_title_barracks, false);
		App->gui_manager->SetElementsVisibility(HUD_description_barracks, false);
		App->gui_manager->SetElementsVisibility(HUD_description_barracks2, false);
		App->gui_manager->SetElementsVisibility(HUD_description_barracks3, false);
		App->gui_manager->SetElementsVisibility(HUD_title_heavy, true);
	}
	if (element == HUD_unit_heavy_barracks && ui_event == GUI_EVENT::UNHOVER)
	{
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_unit_barracks_heavy, false);
		App->gui_manager->SetElementsVisibility(HUD_title_barracks, true);
		App->gui_manager->SetElementsVisibility(HUD_description_barracks, true);
		App->gui_manager->SetElementsVisibility(HUD_description_barracks2, true);
		App->gui_manager->SetElementsVisibility(HUD_description_barracks3, true);
		App->gui_manager->SetElementsVisibility(HUD_title_heavy, false);
	}
	//Upgrade Heavy
	if (element == HUD_unit_upgrade_barracks_heavy && ui_event == GUI_EVENT::UNCLICKED)
	{
		// Upgrade Unit
		UnitUpgrade(3);
		
	}
	//Price Upgrade Heavy
	if (element == HUD_unit_upgrade_barracks_heavy && ui_event == GUI_EVENT::HOVER)
	{
		// Price to upgrade unit
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_upgrade_unit_barracks_heavy, true);
	}
	if (element == HUD_unit_upgrade_barracks_heavy && ui_event == GUI_EVENT::UNHOVER)
	{
		App->gui_manager->SetElementsVisibility(HUD_parent_resources_upgrade_unit_barracks_heavy, false);
	}


	// *****_____Missions HUD_____*****
	if (element == HUD_missions_tab && ui_event == GUI_EVENT::UNCLICKED)
	{
		App->audio->PlayFx(App->gui_manager->standard_button_clicked_fx, 0);

		App->gui_manager->CreateSlideAnimation(HUD_missions_background, 0.5f, false, iPoint(1277, HUD_missions_background->GetScreenPos().y), iPoint(902, HUD_missions_background->GetScreenPos().y));
		
		App->gui_manager->CreateSlideAnimation(HUD_missions_tab_close, 0.5f, false, iPoint(1252, HUD_missions_tab_close->GetScreenPos().y), iPoint(883, HUD_missions_tab_close->GetScreenPos().y));
		App->gui_manager->SetElementsVisibility(HUD_missions_tab_close, true);
		
		App->gui_manager->CreateSlideAnimation(HUD_missions_tab, 0.5f, false, iPoint(1252, HUD_missions_tab->GetScreenPos().y), iPoint(883, HUD_missions_tab->GetScreenPos().y));
		App->gui_manager->SetElementsVisibility(HUD_missions_tab, false);	
		
	}

	if (element == HUD_missions_tab_close && ui_event == GUI_EVENT::UNCLICKED)
	{
		App->audio->PlayFx(App->gui_manager->standard_button_clicked_fx, 0);

		App->gui_manager->CreateSlideAnimation(HUD_missions_background, 0.5f, false, iPoint(902, HUD_missions_background->GetScreenPos().y), iPoint(1277, HUD_missions_background->GetScreenPos().y));

		App->gui_manager->CreateSlideAnimation(HUD_missions_tab_close, 0.5f, false, iPoint(883, HUD_missions_tab_close->GetScreenPos().y), iPoint(1252, HUD_missions_tab_close->GetScreenPos().y));
		App->gui_manager->SetElementsVisibility(HUD_missions_tab_close, false);

		App->gui_manager->CreateSlideAnimation(HUD_missions_tab, 0.5f, false, iPoint(883, HUD_missions_tab->GetScreenPos().y), iPoint(1252, HUD_missions_tab->GetScreenPos().y));
		App->gui_manager->SetElementsVisibility(HUD_missions_tab, true);
	}

	//*****______Dialogs_____*****

	if (element == HUD_dialogs_background && ui_event == GUI_EVENT::UNCLICKED)
	{
		App->dialog_manager->is_clicked = true;
	}

	if (element == HUD_dialogs_screen_block && ui_event == GUI_EVENT::UNCLICKED)
	{
		App->dialog_manager->is_clicked = true;
	}

	if (element == HUD_dialogs_character_no_talking && ui_event == GUI_EVENT::UNCLICKED)
	{
		if (App->dialog_manager->dialog_state == DIALOG_STATE::NOT_ACTIVE && tutorial.tutorial_state != TutorialState::NOT_ACTIVE)
		{
			App->dialog_manager->StartDialog(App->dialog_manager->last_dialog);
		}

		if (App->dialog_manager->dialog_state == DIALOG_STATE::NOT_ACTIVE && tutorial.tutorial_state == TutorialState::NOT_ACTIVE)
		{
			int i = rand() % 8 + 6;

			while(i == last_random_hint)
			{
				i = rand() % 8 + 6;
			}

			last_random_hint = i;

			App->dialog_manager->StartDialog(i);
		}
	}

	if (element == HUD_dialogs_skip_tutorial && ui_event == GUI_EVENT::UNCLICKED)
	{
		App->audio->PlayFx(App->gui_manager->standard_button_clicked_fx, 0);

		tutorial.tutorial_state = TutorialState::NOT_ACTIVE;

		App->dialog_manager->EndDialog();

		App->gui_manager->SetElementsVisibility(HUD_dialogs_skip_tutorial, false);
	}
}

void GameplayScene::SwitchPauseMenuMode()
{
	if (!in_game_background->is_visible)
	{
		if (!in_game_background->is_transitioning)
		{
			App->pause = true;
		}

		// Slide in from the left
		iPoint current_pos = in_game_background->GetScreenPos();

		iPoint origin = { -600, current_pos.y };
		iPoint destination = { 380, current_pos.y };

		App->gui_manager->CreateSlideAnimation(in_game_background, 0.5f, false, origin, destination);
		/*App->gui_manager->CreateFadeAnimation(in_game_background, 0.5f, false, 0.0f, 255.0f);*/

		App->gui_manager->SetElementsVisibility(in_game_background, true);

	}
	else
	{
		if (!in_game_background->is_transitioning)
		{
			App->pause = false;

		}

		// Slide out to the right.
		iPoint origin = { 380, in_game_background->GetScreenPos().y };
		iPoint destination = { 1281, in_game_background->GetScreenPos().y };

		App->gui_manager->CreateSlideAnimation(in_game_background, 0.5f, true, origin, destination);
		/*App->gui_manager->CreateFadeAnimation(in_game_background, 0.5f, true, 255.0f, 0.0f);*/
	}

	// If we want to move the pause menu elsewhere than the center of the screen, the options menu has to change locations or be animated.
	App->gui_manager->SetElementsVisibility(in_game_options_parent, !in_game_options_parent);

	App->audio->PlayFx(App->gui_manager->pause_menu_button_clicked_fx, 0);

	//Mix_HaltMusic();
}

// --- ENTITY HUD METHODS ---
void GameplayScene::DebugHUDSpawn()
{
	if (App->player->building_selected != nullptr)
	{
		switch (App->player->building_selected->type)
		{
		case ENTITY_TYPE::TOWNHALL:
			/*App->gui_manager->CreateSlideAnimation(HUD_barracks_bar, 0.6, false, iPoint(HUD_barracks_bar->GetScreenPos().x, 560), iPoint(HUD_barracks_bar->GetScreenPos().x, 1000));*/
			App->gui_manager->SetElementsVisibility(HUD_barracks_bar, false);
			/*App->gui_manager->CreateSlideAnimation(HUD_enemy_barracks_bar, 0.6, false, iPoint(HUD_enemy_barracks_bar->GetScreenPos().x, 560), iPoint(HUD_enemy_barracks_bar->GetScreenPos().x, 1000));*/
			App->gui_manager->SetElementsVisibility(HUD_enemy_barracks_bar, false);
			/*App->gui_manager->CreateSlideAnimation(HUD_enemy_townhall_bar, 0.6, false, iPoint(HUD_enemy_townhall_bar->GetScreenPos().x, 560), iPoint(HUD_enemy_townhall_bar->GetScreenPos().x, 1000));*/
			App->gui_manager->SetElementsVisibility(HUD_enemy_townhall_bar, false);
			if (!HUD_townhall_bar->is_visible)
			{
				App->audio->PlayFx(App->entity_manager->townhall_clicked_fx, 0);
				App->gui_manager->CreateSlideAnimation(HUD_townhall_bar, 0.6, false, iPoint(HUD_townhall_bar->GetScreenPos().x, 1000), iPoint(HUD_townhall_bar->GetScreenPos().x, 560));
				App->gui_manager->SetElementsVisibility(HUD_townhall_bar, true);
			}

			break;

		case ENTITY_TYPE::ENEMY_TOWNHALL:
			App->gui_manager->SetElementsVisibility(HUD_enemy_barracks_bar, false);
			App->gui_manager->SetElementsVisibility(HUD_barracks_bar, false);
			App->gui_manager->SetElementsVisibility(HUD_townhall_bar, false);
			if (!HUD_enemy_townhall_bar->is_visible)
			{
				App->audio->PlayFx(App->entity_manager->townhall_clicked_fx, 0);
				App->gui_manager->CreateSlideAnimation(HUD_enemy_townhall_bar, 0.6, false, iPoint(HUD_enemy_townhall_bar->GetScreenPos().x, 1000), iPoint(HUD_enemy_townhall_bar->GetScreenPos().x, 560));
				App->gui_manager->SetElementsVisibility(HUD_enemy_townhall_bar, true);
			}

			break;

		case ENTITY_TYPE::BARRACKS:

			App->gui_manager->SetElementsVisibility(HUD_townhall_bar, false);
			App->gui_manager->SetElementsVisibility(HUD_enemy_barracks_bar, false);
			App->gui_manager->SetElementsVisibility(HUD_enemy_townhall_bar, false);
			if (!HUD_barracks_bar->is_visible)
			{
				App->audio->PlayFx(App->entity_manager->barracks_clicked_fx, 0);
				App->gui_manager->CreateSlideAnimation(HUD_barracks_bar, 0.6, false, iPoint(HUD_barracks_bar->GetScreenPos().x, 1000), iPoint(HUD_barracks_bar->GetScreenPos().x, 560));
				App->gui_manager->SetElementsVisibility(HUD_barracks_bar, true);

				if (HUD_townhall_bar->is_visible)
				{
					HUD_townhall_bar->is_visible = !HUD_barracks_bar->is_visible;

				}
				if (HUD_enemy_townhall_bar->is_visible)
				{
					HUD_enemy_townhall_bar->is_visible = !HUD_barracks_bar->is_visible;

				}
				if (HUD_enemy_barracks_bar->is_visible)
				{
					HUD_enemy_barracks_bar->is_visible = !HUD_barracks_bar->is_visible;

				}
			}

			break;

		case ENTITY_TYPE::ENEMY_BARRACKS:

			App->gui_manager->SetElementsVisibility(HUD_enemy_townhall_bar, false);
			App->gui_manager->SetElementsVisibility(HUD_barracks_bar, false);
			App->gui_manager->SetElementsVisibility(HUD_townhall_bar, false);
			if (!HUD_enemy_barracks_bar->is_visible)
			{
				App->audio->PlayFx(App->entity_manager->barracks_clicked_fx, 0);
				App->gui_manager->CreateSlideAnimation(HUD_enemy_barracks_bar, 0.6, false, iPoint(HUD_enemy_barracks_bar->GetScreenPos().x, 1000), iPoint(HUD_enemy_barracks_bar->GetScreenPos().x, 560));
				App->gui_manager->SetElementsVisibility(HUD_enemy_barracks_bar, true);

				if (HUD_enemy_townhall_bar->is_visible)
				{
					HUD_enemy_townhall_bar->is_visible = !HUD_enemy_barracks_bar->is_visible;

				}
				if (HUD_townhall_bar->is_visible)
				{
					HUD_townhall_bar->is_visible = !HUD_enemy_barracks_bar->is_visible;

				}

				if (HUD_barracks_bar->is_visible)
				{
					HUD_barracks_bar->is_visible = !HUD_enemy_barracks_bar->is_visible;

				}
			}

			break;
		}
	}
	else
	{
		if (HUD_townhall_bar->is_visible)
		{
			App->gui_manager->CreateSlideAnimation(HUD_townhall_bar, 0.7, true, iPoint(HUD_townhall_bar->GetScreenPos().x, 560), iPoint(HUD_townhall_bar->GetScreenPos().x, 800));
			/*App->gui_manager->SetElementsVisibility(HUD_townhall_bar, false);*/
		}
		if (HUD_barracks_bar->is_visible)
		{
			App->gui_manager->CreateSlideAnimation(HUD_barracks_bar, 0.7, true, iPoint(HUD_townhall_bar->GetScreenPos().x, 560), iPoint(HUD_barracks_bar->GetScreenPos().x, 800));
			/*App->gui_manager->SetElementsVisibility(HUD_barracks_bar, false);*/
		}
		if (HUD_enemy_townhall_bar->is_visible)
		{
			App->gui_manager->CreateSlideAnimation(HUD_enemy_townhall_bar, 0.7, true, iPoint(HUD_enemy_townhall_bar->GetScreenPos().x, 560), iPoint(HUD_enemy_townhall_bar->GetScreenPos().x, 800));
			/*App->gui_manager->SetElementsVisibility(HUD_enemy_townhall_bar, false);*/
		}
		if (HUD_enemy_barracks_bar->is_visible)
		{
			App->gui_manager->CreateSlideAnimation(HUD_enemy_barracks_bar, 0.7, true, iPoint(HUD_enemy_barracks_bar->GetScreenPos().x, 560), iPoint(HUD_enemy_barracks_bar->GetScreenPos().x, 800));
			/*App->gui_manager->SetElementsVisibility(HUD_enemy_barracks_bar, false);*/
		}
	}
}

void GameplayScene::AdjustVolumeWithScrollbar()
{
	// --- Audio Scrollbars
	if (in_game_music_scrollbar != nullptr)
	{
		float local_thumb_pos = (float)in_game_music_scrollbar->GetThumbHitbox().x - (float)in_game_music_scrollbar->GetHitbox().x;

		float offset = local_thumb_pos / in_game_music_scrollbar->GetHitbox().w;											// Value from 0.0f to 1.0f


		App->audio->volume = offset * 100;																					// Will make the offset a valid value to modify the volume.
	}

	if (in_game_sfx_scrollbar != nullptr)
	{
		float local_thumb_pos = (float)in_game_sfx_scrollbar->GetThumbHitbox().x - (float)in_game_sfx_scrollbar->GetHitbox().x;

		float start_offset = local_thumb_pos / in_game_sfx_scrollbar->GetHitbox().w;										// Value from 0.0f to 1.0f

		uint offset = (int)floor(start_offset * 100);																			// Will make the offset a valid value to modify the volume.					

		App->audio->volume_fx = offset;
	}
}

// --------------- DEBUG METHODS ---------------
void GameplayScene::UnitDebugKeys()
{
	if (App->player->god_mode)
	{
		if (App->pathfinding->IsWalkable(iPoint(App->player->cursor_tile.x, App->player->cursor_tile.y)))
		{
			// UNITS
			if (App->input->GetKey(SDL_SCANCODE_G) == KEY_STATE::KEY_DOWN)
			{
				(Gatherer*)App->entity_manager->CreateEntity(ENTITY_TYPE::GATHERER, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_STATE::KEY_DOWN)
			{
				(Scout*)App->entity_manager->CreateEntity(ENTITY_TYPE::SCOUT, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_I) == KEY_STATE::KEY_DOWN)
			{
				(Infantry*)App->entity_manager->CreateEntity(ENTITY_TYPE::INFANTRY, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_O) == KEY_STATE::KEY_DOWN)
			{
				(Heavy*)App->entity_manager->CreateEntity(ENTITY_TYPE::HEAVY, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_7) == KEY_STATE::KEY_DOWN)
			{
				(EnemyGatherer*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_GATHERER, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
				//SpawnEnemyWave(5, 5, 5, 5);
			}

			if (App->input->GetKey(SDL_SCANCODE_8) == KEY_STATE::KEY_DOWN)
			{
				(EnemyScout*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_SCOUT, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_9) == KEY_STATE::KEY_DOWN)
			{
				(EnemyInfantry*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_INFANTRY, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_0) == KEY_STATE::KEY_DOWN)
			{
				(EnemyHeavy*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_HEAVY, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}

			// BUILDINGS
			if (App->input->GetKey(SDL_SCANCODE_H) == KEY_STATE::KEY_DOWN)
			{
				(TownHall*)App->entity_manager->CreateEntity(ENTITY_TYPE::TOWNHALL, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_B) == KEY_STATE::KEY_DOWN)
			{
				(Barracks*)App->entity_manager->CreateEntity(ENTITY_TYPE::BARRACKS, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_STATE::KEY_DOWN)
			{
				(Wall*)App->entity_manager->CreateEntity(ENTITY_TYPE::WALL, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_J) == KEY_STATE::KEY_DOWN)
			{
				(EnemyTownHall*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_TOWNHALL, App->player->cursor_tile.x, App->player->cursor_tile.y);
			}

			if (App->input->GetKey(SDL_SCANCODE_N) == KEY_STATE::KEY_DOWN)
			{
				(EnemyBarracks*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_BARRACKS, App->player->cursor_tile.x, App->player->cursor_tile.y);
			}

			if (App->input->GetKey(SDL_SCANCODE_E) == KEY_STATE::KEY_DOWN)
			{
				(EnemyWall*)App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_WALL, App->player->cursor_tile.x, App->player->cursor_tile.y);
			}

			// RESOURCES
			if (App->input->GetKey(SDL_SCANCODE_R) == KEY_STATE::KEY_DOWN)
			{
				(Rock*)App->entity_manager->CreateEntity(ENTITY_TYPE::ROCK, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_T) == KEY_STATE::KEY_DOWN)
			{
				(Tree*)App->entity_manager->CreateEntity(ENTITY_TYPE::TREE, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_STATE::KEY_DOWN)
			{
				(Obelisk*)App->entity_manager->CreateEntity(ENTITY_TYPE::OBELISK, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}

			if (App->input->GetKey(SDL_SCANCODE_K) == KEY_STATE::KEY_DOWN)
			{
				App->entity_manager->resource_data += 300;
				App->entity_manager->resource_electricity += 300;
				App->entity_manager->resource_bits += 300;
			}

			// OBSTACLES
			if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_STATE::KEY_DOWN)
			{
				(Boulder*)App->entity_manager->CreateEntity(ENTITY_TYPE::BOULDER, App->player->cursor_tile.x, App->player->cursor_tile.y, 1);
			}
		}
	}
}

void GameplayScene::PathfindingDebug()
{
	if (App->map->pathfinding_meta_debug)
	{
		static iPoint origin;
		static bool origin_selected = false;

		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->map->WorldToMap(p.x, p.y);

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_DOWN)
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
}

void GameplayScene::DrawPathfindingDebug()
{
	if (App->map->pathfinding_meta_debug)
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
}

float GameplayScene::N_Lerp(float start, float end, float rate, bool smash_in)			//Rate goes from 0 to rate*rate || (1 - rate) * (1 - rate)
{																						//Simillar to Lerp() but allows for ease-ins.
	float increment = 0;

	if (smash_in)
	{
		increment = (end - start) * rate * rate;										// Ease-Out Smash-In --> Slow out, Fast In
	}
	else
	{
		float r = 1 - ((1 - rate) * (1 - rate));										// Smash-Out Ease-In --> Fast out, Slow In. Full version.
		increment = (end - start) * r;													// This one accepts the parameters in the same order as Lerp.
	}

	return start + increment;
}

void GameplayScene::CheckCompletedQuests()
{


	for (std::vector <Quest*>::iterator it = App->quest_manager->quests.begin(); it != App->quest_manager->quests.end(); it++)
	{
		int quest_id = (*it)->id;

		switch (quest_id)
		{
		case 0:
			if ((*it)->completed)
			{
				HUD_missions_checkbox_in_progress_main_quest_on->is_visible = true;
			}
			else
			{
				HUD_missions_checkbox_in_progress_main_quest_off->is_visible = true;
				HUD_missions_checkbox_in_progress_main_quest_on->is_visible = false;
			}
			break;

		case 1:
			if ((*it)->completed)
			{
				HUD_missions_checkbox_in_progress_side_quest_on->is_visible = true;
			}
			else
			{
				HUD_missions_checkbox_in_progress_side_quest_off->is_visible = true;
				HUD_missions_checkbox_in_progress_side_quest_on->is_visible = false;
			}
			break;

		case 2:
			if ((*it)->completed)
			{
				HUD_missions_checkbox_in_progress_side_quest2_on->is_visible = true;
			}
			else
			{
				HUD_missions_checkbox_in_progress_side_quest2_off->is_visible = true;
				HUD_missions_checkbox_in_progress_side_quest2_on->is_visible = false;
			}
			break;

		case 3:
			if ((*it)->completed)
			{
				HUD_missions_checkbox_in_progress_side_quest3_on->is_visible = true;
			}
			else
			{
				HUD_missions_checkbox_in_progress_side_quest3_off->is_visible = true;
				HUD_missions_checkbox_in_progress_side_quest3_on->is_visible = false;
			}
			break;

		default:
			break;
		}
	}
}
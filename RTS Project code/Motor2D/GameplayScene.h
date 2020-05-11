#ifndef __GAMEPLAY_SCENE_H__
#define __GAMEPLAY_SCENE_H__

#include "Scene.h"

struct SDL_Texture;

class Timer;
class PerfTimer;

enum class ENTITY_TYPE;

class UI;
class UI_Image;
class UI_Text;
class UI_Button;
class UI_InputBox;
class UI_Scrollbar;

class GameplayScene : public Scene
{
public:

	GameplayScene();
	virtual ~GameplayScene();							// Destructor

	bool Awake(pugi::xml_node&);						// Called before render is available
	bool Start();										// Called before the first frame
	bool PreUpdate();									// Called before all Updates
	bool Update(float dt);								// Called each loop iteration
	bool PostUpdate();									// Called before all Updates
	bool CleanUp();										// Called before quitting

	//bool Load(pugi::xml_node& data);
	//bool Save(pugi::xml_node& data) const;
	void CheckForWinLose();


public:
	void InitScene();

	void ExecuteTransition();

	// --- Entity Spawn Methods.
	void SpawnAllyUnit(ENTITY_TYPE type);
	void SpawnEnemyUnit(ENTITY_TYPE type);

	void SpawnEnemyWave(int gatherer_amount, int scout_amount, int infantry_amount, int heavy_amount);		// TMP. Put in EnemyAI module.

	void BuildingUpgrade();

	void UnitUpgrade();

	bool CheckResources(uint required_data, uint required_electricity);

	// --- UI & HUD
	void LoadGuiElements();

	void LoadInGameOptionsMenu();

	void OnEventCall(UI* element, UI_EVENT ui_event);

	void DebugHUDSpawn();																	// HUD SPAWN

	void AdjustVolumeWithScrollbar();
	
	// --- Debug Methods
	void UnitDebugKeys();

	void PathfindingDebug();
	void DrawPathfindingDebug();

	float N_Lerp(float start, float end, float rate, bool smash_in = false);		// Method that will be used to non-linearly interpolate the speed of a given transition.

public:
	bool transition_to_main_menu_scene;									// Will keep track whether or not the transition to the main menu scene has been triggered.
	bool transition_to_win_scene;										// Will keep track whether or not the transition to the win scene has been triggered.
	bool transition_to_lose_scene;										// Will keep track whether or not the transition to the lose scene has been triggered.
	
	UI_Image*				background;
	SDL_Rect				background_rect;

	//SDL_Texture*			mouse_debug_tex;							//Texture that will appear at mouse position when pathfinding is being debugged.
	SDL_Texture*			path_debug_tex;								//Texture that will appear at the path tiles when pathfinding is being debugged.
	SDL_Texture*			background_texture;
	

	// In-game menu
	UI_Image*				main_in_menu2;
	UI_Image*				label_1_in;
	
	UI_Text*				in_text;
	UI_Text*				button_in_text;
	
	
	UI_Button*				in_buttons_resume;
	UI_Button*				in_buttons_save;
	UI_Button*				in_buttons_load;
	UI_Button*				in_buttons_exit;
	UI_Button*				unmute_in;
	UI_Button*				mute_in;
	
	UI_Scrollbar*			scrollbar_in;

	iPoint					firstOriginalPos;
	iPoint					secondOrigianlPos;
	
	bool					firstScrollPosCalc;
	bool					secondScrollPosCalc;

	bool					tab_clicked;

	// In-game menu

	UI_Image*				in_game_background;
	UI_Button*				in_game_continue_button;
	UI_Button*				in_game_options_button;
	UI_Button*				in_game_exit_button;
	UI_Button*				in_game_back_to_menu;
	UI_Text*				in_game_title_text;

	// In-game options
	UI_Image*				in_game_options_parent;
	
	UI_Button*				in_game_back_button;
	UI_Scrollbar*			in_game_music_scrollbar;
	UI_Scrollbar*			in_game_sfx_scrollbar;

	// Text:
	UI_Text*				in_game_resolution_text;
	UI_Text*				in_game_sfx_text;
	UI_Text*				in_game_music_text;
	UI_Text*				in_game_options_text;


	// HUD
	UI_Button*				 HUD_group_button;
	UI_Button*				 HUD_home_button;
	UI_Button*				 HUD_pause_button;
	UI_Button*				 HUD_play_button;
	UI_Image*				 HUD_resource_bar;


	// HUD townhall
	UI_Image*				 HUD_townhall_bar;
	UI_Text*				 HUD_title_townhall;
	UI_Text*				 HUD_description_townhall;
	UI_Button*				 HUD_unit_townhall;

	UI_Image*				 HUD_parent_resources_unit_townhall_gatherer;
	UI_Image*				 HUD_resources_unit_townhall_gatherer;
	UI_Text*				 HUD_prices_unit_townhall_gatherer;
	UI_Image*				 HUD_resources2_unit_townhall_gatherer;
	UI_Text*				 HUD_prices2_unit_townhall_gatherer;

	UI_Text*				 HUD_title_gatherer;
	UI_Text*				 HUD_description_gatherer;

	UI_Image*				 HUD_parent_resources_upgrade_unit_townhall_gatherer;
	UI_Image*				 HUD_resources_upgrade_unit_townhall_gatherer;
	UI_Text*				 HUD_prices_upgrade_unit_townhall_gatherer;
	UI_Image*				 HUD_resources2_upgrade_unit_townhall_gatherer;
	UI_Text*				 HUD_prices2_upgrade_unit_townhall_gatherer;

	UI_Image*				 HUD_parent_resources_upgrade_townhall;
	UI_Image*				 HUD_resources_upgrade_townhall;
	UI_Text*				 HUD_prices_upgrade_townhall;
	UI_Image*				 HUD_resources2_upgrade_townhall;
	UI_Text*				 HUD_prices2_upgrade_townhall;

	UI_Button*				 HUD_unit_upgrade_townhall_gatherer;
	UI_Button*				 HUD_upgrade_townhall;

	// HUD Barracks
	UI_Image*				 HUD_barracks_bar;
	UI_Text*				 HUD_title_barracks;
	UI_Text*				 HUD_description_barracks;
	UI_Button*				 HUD_unit_barracks;

	UI_Image*				 HUD_parent_resources_unit_barracks_infantry;
	UI_Image*				 HUD_resources_unit_barracks_infantry;
	UI_Text*				 HUD_prices_unit_townhall_infantry;
	UI_Image*				 HUD_resources2_unit_barracks_infantry;
	UI_Text*				 HUD_prices2_unit_townhall_infantry;

	UI_Text*				 HUD_title_infantry;
	UI_Text*				 HUD_description_infantry;
							 
	UI_Image*				 HUD_parent_resources_upgrade_unit_barracks_infantry;
	UI_Image*				 HUD_resources_upgrade_unit_barracks_infantry;
	UI_Text*				 HUD_prices_upgrade_unit_barracks_infantry;
	UI_Image*				 HUD_resources2_upgrade_unit_barracks_infantry;
	UI_Text*				 HUD_prices2_upgrade_unit_barracks_infantry;
							 
	UI_Image*				 HUD_parent_resources_upgrade_barracks;
	UI_Image*				 HUD_resources_upgrade_barracks;
	UI_Text*				 HUD_prices_upgrade_barracks;
	UI_Image*				 HUD_resources2_upgrade_barracks;
	UI_Text*				 HUD_prices2_upgrade_barracks;

	UI_Button*				 HUD_unit_upgrade_barracks_infantry;
	UI_Button*				 HUD_upgrade_barracks;

	UI_Text*				 God_Mode_Activated;

	//Resources HUD

	UI_Text*				HUD_data_resource_text;
	UI_Text*				HUD_electricity_resource_text;

	UI_Image*				HUD_data_resource;
	UI_Image*				HUD_electricity_resource;

	std::string				HUD_data_resource_string;
	std::string				HUD_electricity_resource_string;

	//Enemy HUD 
	// Townhall
	UI_Image*			    HUD_enemy_townhall_bar;
	UI_Text*				HUD_enemy_title_townhall;
	UI_Text*				HUD_enemy_description_townhall;

	// Barracks
	UI_Image*				HUD_enemy_barracks_bar;
	UI_Text*				HUD_enemy_title_barracks;
	UI_Text*				HUD_enemy_description_barracks;


	//Missions HUD
	// Tab

	UI_Button*				HUD_missions_tab;
	UI_Image*				HUD_missions_background;

private:

	//Audio

	uint					inGame_song;

};
#endif // !__GAMEPLAY_SCENE_H__
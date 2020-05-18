#ifndef __GAMEPLAY_SCENE_H__
#define __GAMEPLAY_SCENE_H__

#include "Scene.h"

struct SDL_Texture;
struct _TTF_Font;

class Timer;
class PerfTimer;

enum class ENTITY_TYPE;

class GuiElement;
class GuiImage;
class GuiText;
class GuiButton;
class GuiInputBox;
class GuiScrollbar;

enum class TutorialState
{
	NOT_ACTIVE,
	SELECT_UNIT,
	MOVE_UNIT,
	GATHER_RESOURCE,
	SELECT_BARRACKS,
	RECRUIT_INFANTRY,
	ATTACK_ENEMY
};

struct Tutorial
{
	bool lock_camera;
	iPoint camera_position;
	TutorialState tutorial_state;

	void NextStep(TutorialState state);
};

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
	

public:
	void InitScene();

	void ExecuteTransition();

	void CheckForWinLose();
	bool CheckForTownHall();
	void CheckCompletedQuests();

	// --- Entity Spawn Methods.
	void SpawnAllyUnit(ENTITY_TYPE type);
	void SpawnEnemyUnit(ENTITY_TYPE type);

	void SpawnEnemyWave(int gatherer_amount, int scout_amount, int infantry_amount, int heavy_amount);		// TMP. Put in EnemyAIManager module.

	void BuildingUpgrade();
	void UnitUpgrade(int unit);

	bool CheckResources(uint required_data, uint required_electricity, uint required_bits);

	// --- UI & HUD
	void LoadGuiElements();
	void LoadInGameOptionsMenu();

	void OnEventCall(GuiElement* element, GUI_EVENT ui_event);
	void SwitchPauseMenuMode();																// Will switch between enabling and disabling the pause menu.
	void DebugHUDSpawn();																	// HUD SPAWN
	void AdjustVolumeWithScrollbar();
	
	//Tutorial
	void HandleTutorial();

	// --- Debug Methods
	void UnitDebugKeys();

	void PathfindingDebug();
	void DrawPathfindingDebug();

	float N_Lerp(float start, float end, float rate, bool smash_in = false);		// Method that will be used to non-linearly interpolate the speed of a given transition.

public:
	bool transition_to_main_menu_scene;									// Will keep track whether or not the transition to the main menu scene has been triggered.
	bool transition_to_win_scene;										// Will keep track whether or not the transition to the win scene has been triggered.
	bool transition_to_lose_scene;										// Will keep track whether or not the transition to the lose scene has been triggered.
	
	GuiImage*				background;
	SDL_Rect				background_rect;

	//SDL_Texture*			mouse_debug_tex;							//Texture that will appear at mouse position when pathfinding is being debugged.
	SDL_Texture*			path_debug_tex;								//Texture that will appear at the path tiles when pathfinding is being debugged.
	SDL_Texture*			background_texture;
	

	// In-game menu
	GuiImage*				main_in_menu2;
	GuiImage*				label_1_in;
	
	GuiText*				in_text;
	GuiText*				button_in_text;
	
	
	GuiButton*				in_buttons_resume;
	GuiButton*				in_buttons_save;
	GuiButton*				in_buttons_load;
	GuiButton*				in_buttons_exit;
	GuiButton*				unmute_in;
	GuiButton*				mute_in;
	
	GuiScrollbar*			scrollbar_in;

	iPoint					firstOriginalPos;
	iPoint					secondOrigianlPos;

	Tutorial				tutorial;
	
	bool					firstScrollPosCalc;
	bool					secondScrollPosCalc;

	bool					tab_clicked;

	// In-game menu

	GuiImage*				in_game_background;
	GuiButton*				in_game_continue_button;
	GuiButton*				in_game_options_button;
	GuiButton*				in_game_exit_button;
	GuiButton*				in_game_back_to_menu;
	GuiText*				in_game_title_text;

	// In-game options
	GuiImage*				in_game_options_parent;
	
	GuiButton*				in_game_back_button;
	GuiScrollbar*			in_game_music_scrollbar;
	GuiScrollbar*			in_game_sfx_scrollbar;

	// Text:
	GuiText*				in_game_resolution_text;
	GuiText*				in_game_sfx_text;
	GuiText*				in_game_music_text;
	GuiText*				in_game_options_text;


	// HUD
	GuiButton*				 HUD_group_button;
	GuiButton*				 HUD_home_button;
	GuiButton*				 HUD_pause_button;
	GuiButton*				 HUD_play_button;
	GuiImage*				 HUD_resource_bar;


	// HUD townhall
	GuiImage*				 HUD_townhall_bar;
	GuiText*				 HUD_title_townhall;
	GuiText*				 HUD_description_townhall;
	GuiButton*				 HUD_unit_gatherer_townhall;
	GuiButton*				 HUD_unit_scout_townhall;

	GuiImage*				 HUD_parent_resources_upgrade_townhall;
	GuiImage*				 HUD_resources_data_upgrade_townhall;
	GuiText*				 HUD_prices_data_upgrade_townhall;
	GuiImage*				 HUD_resources_electricity_upgrade_townhall;
	GuiText*				 HUD_prices_electricity_upgrade_townhall;
	GuiImage*				 HUD_resources_bytes_upgrade_townhall;
	GuiText*				 HUD_prices_bytes_upgrade_townhall;

	GuiButton*				 HUD_upgrade_townhall;

	//HUD gatherer
	GuiText*				 HUD_title_gatherer;
	GuiText*				 HUD_description_gatherer;

	GuiImage*				 HUD_parent_resources_unit_townhall_gatherer;
	GuiImage*				 HUD_resources_data_unit_townhall_gatherer;
	GuiText*				 HUD_prices_data_unit_townhall_gatherer;
	GuiImage*				 HUD_resources_electricity_unit_townhall_gatherer;
	GuiText*				 HUD_prices_electricity_unit_townhall_gatherer;
	GuiImage*				 HUD_resources_bytes_unit_townhall_gatherer;
	GuiText*				 HUD_prices_bytes_unit_townhall_gatherer;

	GuiImage*				 HUD_parent_resources_upgrade_unit_townhall_gatherer;
	GuiImage*				 HUD_resources_data_upgrade_unit_townhall_gatherer;
	GuiText*				 HUD_prices_upgrade_data_unit_townhall_gatherer;
	GuiImage*				 HUD_resources_electricity_upgrade_unit_townhall_gatherer;
	GuiText*				 HUD_prices_electricity_upgrade_unit_townhall_gatherer;
	GuiImage*				 HUD_resources_bytes_upgrade_unit_townhall_gatherer;

	GuiButton*				 HUD_unit_upgrade_townhall_gatherer;

	//HUD scout
	GuiText*				 HUD_title_scout;
	GuiText*				 HUD_description_scout;
							 
	GuiImage*				 HUD_parent_resources_unit_townhall_scout;
	GuiImage*				 HUD_resources_data_unit_townhall_scout;
	GuiText*				 HUD_prices_data_unit_townhall_scout;
	GuiImage*				 HUD_resources_electricity_unit_townhall_scout;
	GuiText*				 HUD_prices_electricity_unit_townhall_scout;
	GuiImage*				 HUD_resources_bytes_unit_townhall_scout;
	GuiText*				 HUD_prices_bytes_unit_townhall_scout;

	GuiImage*				 HUD_parent_resources_upgrade_unit_townhall_scout;
	GuiImage*				 HUD_resources_data_upgrade_unit_townhall_scout;
	GuiText*				 HUD_prices_upgrade_data_unit_townhall_scout;
	GuiImage*				 HUD_resources_electricity_upgrade_unit_townhall_scout;
	GuiText*				 HUD_prices_electricity_upgrade_unit_townhall_scout;
	GuiImage*				 HUD_resources_bytes_upgrade_unit_townhall_scout;

	GuiButton*				 HUD_unit_upgrade_townhall_scout;

	// HUD Barracks
	GuiImage*				 HUD_barracks_bar;
	GuiText*				 HUD_title_barracks;
	GuiText*				 HUD_description_barracks;
	GuiButton*				 HUD_unit_infantry_barracks;
	GuiButton*				 HUD_unit_heavy_barracks;

	GuiImage*				 HUD_parent_resources_upgrade_barracks;
	GuiImage*				 HUD_resources_data_upgrade_barracks;
	GuiText*				 HUD_prices_data_upgrade_barracks;
	GuiImage*				 HUD_resources_electricity_upgrade_barracks;
	GuiText*				 HUD_prices_electricity_upgrade_barracks;
	GuiImage*				 HUD_resources_bytes_upgrade_barracks;
	GuiText*				 HUD_prices_bytes_upgrade_barracks;

	GuiButton*				 HUD_upgrade_barracks;

	// HUD Infantry
	GuiText*				 HUD_title_infantry;
	GuiText*				 HUD_description_infantry;

	GuiImage*				 HUD_parent_resources_unit_barracks_infantry;
	GuiImage*				 HUD_resources_data_unit_barracks_infantry;
	GuiText*				 HUD_prices_data_unit_townhall_infantry;
	GuiImage*				 HUD_resources_electricity_unit_barracks_infantry;
	GuiText*				 HUD_prices_electricity_unit_townhall_infantry;
	GuiImage*				 HUD_resources_bytes_unit_barracks_infantry;
	GuiText*				 HUD_prices_bytes_unit_townhall_infantry;
							
	GuiImage*				 HUD_parent_resources_upgrade_unit_barracks_infantry;
	GuiImage*				 HUD_resources_data_upgrade_unit_barracks_infantry;
	GuiText*				 HUD_prices_data_upgrade_unit_barracks_infantry;
	GuiImage*				 HUD_resources_electricity_upgrade_unit_barracks_infantry;
	GuiText*				 HUD_prices_electricity_upgrade_unit_barracks_infantry;
	GuiImage*				 HUD_resources_bytes_upgrade_unit_barracks_infantry;
	GuiText*				 HUD_prices_bytes_upgrade_unit_barracks_infantry;
							 
	GuiButton*				 HUD_unit_upgrade_barracks_infantry;
	

	// HUD Heavy
	GuiText*				 HUD_title_heavy;
	GuiText*				 HUD_description_heavy;

	GuiImage*				 HUD_parent_resources_unit_barracks_heavy;
	GuiImage*				 HUD_resources_data_unit_barracks_heavy;
	GuiText*				 HUD_prices_data_unit_townhall_heavy;
	GuiImage*				 HUD_resources_electricity_unit_barracks_heavy;
	GuiText*				 HUD_prices_electricity_unit_townhall_heavy;
	GuiImage*				 HUD_resources_bytes_unit_barracks_heavy;
	GuiText*				 HUD_prices_bytes_unit_townhall_heavy;

	GuiImage*				 HUD_parent_resources_upgrade_unit_barracks_heavy;
	GuiImage*				 HUD_resources_data_upgrade_unit_barracks_heavy;
	GuiText*				 HUD_prices_data_upgrade_unit_barracks_heavy;
	GuiImage*				 HUD_resources_electricity_upgrade_unit_barracks_heavy;
	GuiText*				 HUD_prices_electricity_upgrade_unit_barracks_heavy;
	GuiImage*				 HUD_resources_bytes_upgrade_unit_barracks_heavy;
	GuiText*				 HUD_prices_bytes_upgrade_unit_barracks_heavy;

	GuiButton*				 HUD_unit_upgrade_barracks_heavy;

	GuiText*				 God_Mode_Activated;

	//Resources HUD

	GuiText*				HUD_data_resource_text;
	GuiText*				HUD_electricity_resource_text;
	GuiText*				HUD_bytes_resource_text;

	GuiImage*				HUD_data_resource;
	GuiImage*				HUD_electricity_resource;
	GuiImage*				HUD_bytes_resources;

	std::string				HUD_data_resource_string;
	std::string				HUD_electricity_resource_string;
	std::string				HUD_bytes_resource_string;

	//Enemy HUD 
	// Townhall
	GuiImage*			    HUD_enemy_townhall_bar;
	GuiText*				HUD_enemy_title_townhall;
	GuiText*				HUD_enemy_description_townhall;

	// Barracks
	GuiImage*				HUD_enemy_barracks_bar;
	GuiText*				HUD_enemy_title_barracks;
	GuiText*				HUD_enemy_description_barracks;


	//Missions HUD

	GuiButton*				HUD_missions_tab;
	GuiButton*				HUD_missions_tab_close;
	GuiImage*				HUD_missions_background;
	GuiText*				HUD_missions_title;
	GuiText*				HUD_missions_title_primary;
	GuiText*				HUD_missions_primary_quest;
	GuiText*				HUD_missions_title_side;
	GuiText*				HUD_missions_side_quest;
	GuiText*				HUD_missions_side_quest2;
	GuiText*				HUD_missions_side_quest3;

	GuiImage* HUD_missions_checkbox_in_progress_main_quest_off;
	GuiImage* HUD_missions_checkbox_in_progress_main_quest_on;
	GuiImage* HUD_missions_checkbox_in_progress_side_quest_off;
	GuiImage* HUD_missions_checkbox_in_progress_side_quest_on;
	GuiImage* HUD_missions_checkbox_in_progress_side_quest2_off;
	GuiImage* HUD_missions_checkbox_in_progress_side_quest2_on;
	GuiImage* HUD_missions_checkbox_in_progress_side_quest3_off;
	GuiImage* HUD_missions_checkbox_in_progress_side_quest3_on;

	//Dialogs
	GuiImage*				HUD_dialogs_background;
	std::vector<GuiText*>	HUD_dialog_text;	//Lines of dialog text
	GuiImage*				HUD_dialogs_character_no_talking;
	GuiImage*				HUD_dialogs_character_talking;
	GuiImage*				HUD_dialogs_screen_block;
	GuiButton*				HUD_dialogs_skip_tutorial;

private:

	//Audio
	uint					inGame_song;

};
#endif // !__GAMEPLAY_SCENE_H__
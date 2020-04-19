#ifndef __GAMEPLAY_SCENE_H__
#define __GAMEPLAY_SCENE_H__

#include "Scene.h"

struct SDL_Texture;

class Timer;
class PerfTimer;

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

	void LoadGuiElements();
	
	void OnEventCall(UI* element, UI_EVENT ui_event);

	void ExecuteTransition();

	void LoadInGameOptionsMenu();


	//void DebugKeys();

	void UnitDebugKeys();

	void PathfindingDebug();
	void DrawPathfindingDebug();

	// --- Entity Spawn HUD
	void DebugHUDSpawn();


public:
	//std::list<std::string>	map_names;
	
	float					fade_time;
	
	std::string				music_path;
	std::string				music_path2;
	std::string				music_path3;

	bool					to_end;

public:
	//float					cam_debug_speed;							//Will store the movement speed for the camera while in debug mode. Done for readability.

	
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
	UI_Image*				 HUD_resource_bar;
	UI_Image*				 HUD_data_resource;
	UI_Image*				 HUD_electricity_resource;

	// HUD townhall
	UI_Image*				 HUD_townhall_bar;
	UI_Text*				 HUD_title_townhall;
	UI_Button*				 HUD_back_townhall;
	UI_Text*				 HUD_description_townhall;
	UI_Button*				 HUD_unit_townhall;

	UI_Image*				 HUD_parent_resources_unit_townhall;
	UI_Image*				 HUD_resources_unit_townhall;
	UI_Image*				 HUD_resources2_unit_townhall;

	UI_Text*				 HUD_title_gatherer;
	UI_Text*				 HUD_description_gatherer;

	UI_Image*				 HUD_parent_resources_upgrade_unit_townhall;
	UI_Image*				 HUD_resources_upgrade_unit_townhall;
	UI_Image*				 HUD_resources2_upgrade_unit_townhall;

	UI_Image*				 HUD_parent_resources_upgrade_townhall;
	UI_Image*				 HUD_resources_upgrade_townhall;
	UI_Image*				 HUD_resources2_upgrade_townhall;

	UI_Button*				 HUD_unit_upgrade_townhall;
	UI_Button*				 HUD_upgrade_townhall;

	// HUD Barracks
	UI_Image*				 HUD_barracks_bar;
	UI_Text*				 HUD_title_barracks;
	UI_Button*				 HUD_back_barracks;
	UI_Text*				 HUD_description_barracks;
	UI_Button*				 HUD_unit_barracks;

	UI_Image*				 HUD_parent_resources_unit_barracks;
	UI_Image*				 HUD_resources_unit_barracks;
	UI_Image*				 HUD_resources2_unit_barracks;

	UI_Text*				 HUD_title_infantry;
	UI_Text*				 HUD_description_infantry;
							 
	UI_Image*				 HUD_parent_resources_upgrade_unit_barracks;
	UI_Image*				 HUD_resources_upgrade_unit_barracks;
	UI_Image*				 HUD_resources2_upgrade_unit_barracks;
							 
	UI_Image*				 HUD_parent_resources_upgrade_barracks;
	UI_Image*				 HUD_resources_upgrade_barracks;
	UI_Image*				 HUD_resources2_upgrade_barracks;

	UI_Button*				 HUD_unit_upgrade_barracks;
	UI_Button*				 HUD_upgrade_barracks;

};
#endif // !__GAMEPLAY_SCENE_H__
#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1PerfTimer.h"
#include "j1Timer.h"
#include "p2List.h"
#include "j1Gui.h"
#include "UI_Image.h"

struct SDL_Texture;

class UI;
class UI_Image;
class UI_Text;
class UI_Button;
class UI_InputBox;
class UI_Scrollbar;

enum ListOfMapNames
{
	TutorialLevel = 0,
	FirstLevel
};

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node& data);
	bool Save(pugi::xml_node& data) const;

public:
	bool Load_lvl(int time);

	void LoadGuiElements();


public:
	int					currentMap;
	p2List<p2SString*>	map_names;
	float				fade_time;
	bool				to_end;
	p2SString			music_path;
	p2SString			music_path2;
	p2SString			music_path3;
	bool				firstMap;			//Keeps track of which map is loaded. In this case this one keeps track of the first map.
	bool				secondMap;			//Keeps track of which map is loaded. In this case this one keeps track of the second map.

	UI_Image* test;

	// Testing the UI
	UI_Image* banner;
	
	UI_Text*		interactibleText;
	UI_Text*		buttonLabel;
	UI_Button*		button;
	UI_Button*		escButton;
	UI_Button*		draggableButton;
	UI_Button*		draggableButton2;

	UI_InputBox*	inputBox;

	UI_Image*		scrollWindow;
	UI_Text*		scrollText;
	UI_InputBox*	scrollInputBox;
	UI_Scrollbar*	scrollbar;
	UI_Image*		thumbo;

private:
	float				cam_debug_speed;	//Will store the movement speed for the camera while in debug mode. Done for readability.
	//SDL_Texture*		mouse_debug_tex;	//Texture that will appear at mouse position when pathfinding is being debugged.
	SDL_Texture*		path_debug_tex;		//Texture that will appear at the path tiles when pathfinding is being debugged.

public:

	// Main menu
	UI_Image*			main_window;
	UI_Image*			background_image;
	UI_Image*			side_bars;
	UI_Image*			label_1;
	UI_Image*			label_2;
	UI_Button*			main_button_play;
	UI_Button*			main_button_continue;
	UI_Button*			main_button_settings;
	UI_Button*			main_button_exit;
	UI_Text*			title_text;
	UI_Text*			button_text;

	// Settings menu
	UI_Image*			main_settings_menu;
	UI_Image*			label_1_settings;
	UI_Text*			settings_text;
	UI_Text*			settings_button_text;
	UI_Button*			unmute;
	UI_Button*			mute;
	UI_Button*			check;
	UI_Button*			back;
	UI_Button*			github;
	UI_Button*			credits;
	UI_Scrollbar*		scrollbar_settings;

	// Credits menu
	UI_Image*			main_credits_menu;
	UI_Image*			label_1_credits;
	UI_Text*			credits_text;
	UI_Text*			text;
	UI_Button*			back_credits;

	// HUD
	UI_Image*			upper_bar;
	UI_Image*			upper_bar1;
	UI_Image*			p1;
	UI_Image*			p2;
	UI_Image*			coin;
	UI_Text*			names;
	UI_Text*			title;
	UI_Image*			hearts[6] = { nullptr,nullptr,nullptr };
	UI_Text*			score_player;
	UI_Text*			timer;
	UI_Text*			count_coins;

	// In-game menu
	UI_Image*			main_in_menu;
	UI_Image*			label_1_in;
	UI_Text*			in_text;
	UI_Text*			button_in_text;
	UI_Button*			in_buttons_resume;
	UI_Button*			in_buttons_save;
	UI_Button*			in_buttons_load;
	UI_Button*			in_buttons_exit;
	UI_Button*			unmute_in;
	UI_Button*			mute_in;
	UI_Scrollbar*		scrollbar_in;

	
	iPoint				firstOriginalPos;
	iPoint				secondOrigianlPos;
	bool				firstScrollPosCalc;
	bool				secondScrollPosCalc;
};

#endif // __j1SCENE_H__
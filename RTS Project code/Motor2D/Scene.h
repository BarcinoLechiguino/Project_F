#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "Module.h"
#include "PerfTimer.h"
#include "Timer.h"
#include "Gui.h"
#include "UI_Image.h"
#include "Rock.h"
#include <vector>

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

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

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

	bool LoadFirstMap();						//Loads first map + walkability map

	bool Load_lvl(int time);

	void LoadGuiElements();

	void DebugKeys();

	void PathfindingDebug();



public:

	int						currentMap;
	std::list<std::string>	map_names;
	float					fade_time;
	bool					to_end;
	std::string				music_path;
	std::string				music_path2;
	std::string				music_path3;
	bool					firstMap;			//If First map is loaded
	bool					secondMap;			//If Second map is loaded

	std::vector<Rock*>	rock_test;
	std::vector<Enemy*>   enemy_test;

private:

	float					cam_debug_speed;	//Will store the movement speed for the camera while in debug mode. Done for readability.
	//SDL_Texture*			mouse_debug_tex;	//Texture that will appear at mouse position when pathfinding is being debugged.
	SDL_Texture*			path_debug_tex;		//Texture that will appear at the path tiles when pathfinding is being debugged.

public:

	// In-game menu
	UI_Image*				main_in_menu;
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
};
#endif // __j1SCENE_H__
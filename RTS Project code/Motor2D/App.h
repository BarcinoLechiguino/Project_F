#ifndef __j1APP_H__
#define __j1APP_H__

#include <list>
#include <string>
#include "Module.h"
#include "PerfTimer.h"
#include "Timer.h"
#include "PugiXml\src\pugixml.hpp"

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class j1SceneMenu;
class Scene;
class Map;
class PathFinding;
class Fade_Scene;
class Collisions;
class EntityManager;
class Fonts;
class Gui;
class Console;
class Main_Menu;
class Player;

class j1App
{
public:

	// Constructor
	j1App(int argc, char* args[]);

	// Destructor
	virtual ~j1App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	float GetDt();
	void LoadGame(const char* file);
	void SaveGame(const char* file) const;
	void GetSaveGames(std::list<std::string>& list_to_fill);

private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow(); //Chenged to non const due to list unknown problem

public:

	// Modules
	Window*			win;
	Input*			input;
	Render*			render;
	Textures*		tex;
	Audio*			audio;
	Fonts*			font;
	Scene*			scene;
	Map*			map;
	PathFinding*	pathfinding;
	EntityManager*	entityManager;
	Collisions*		collisions;
	Fade_Scene*		fadescene;
	Gui*			gui;
	Console*		console;
	Main_Menu*		mainmenu;
	Player*			player;

	uint				frame_cap;				//Stores the frames per second cap to be applied.
	uint				original_frame_cap;		//Stores the original frame cap at application start.
	float				seconds_since_startup;	//Secons that have elapsed since app start.

	bool				framesAreCapped;		//Keeps track whether the frame cap is on or off.
	bool				vsyncIsActive;			//Keeps track whether Vsync is on or off.
	bool				pause;

private:

	std::list<Module*>	modules;
	int					argc;
	char**				args;

	std::string			title;
	std::string			organization;

	mutable bool		want_to_save;
	bool				want_to_load;
	std::string			load_game;
	mutable std::string	save_game;

	//Framerate
	uint64				frame_count;			
	Timer				startup_timer;			//Used to keep track of time since app start.
	Timer				frame_timer;			//Keeps track of everything time related in the span of a frame.
	PerfTimer			perf_timer;				//Creates a pointer to j1PerfTimer tool. Gives access to j1PerfTimer's elements. Used to keep track of time since app start.
	PerfTimer			last_second_timer;		//Creates a pointer to j1PerfTimer tool. Used to calculate variables in spans of one second.
	uint32				last_update_ms;			//Calculates the amount of milliseconds that the last update spent running.
	uint32				frames_last_second;		//Calculates the amount of frames that where processed the last second.
	uint32				prev_sec_frames;		

	PerfTimer			true_delay_timer;		//Timer that will be used to see the actual amount of delay that was applied to cap the framerate.
	float				dt;						//Keeps track of the amount of time in milliseconds that has passed in a frame. Will be used to make everything (update()) be in the same timestep.

	char*				frameCapOnOff;			//String that is set to 'On' when the frame cap is on and  'Off' when it is off.
	char*				vsyncOnOff;				//String that is set to 'On' when Vsync is on and 'Off' when it is off.
};

extern j1App* App; // No student is asking me about that ... odd :-S

#endif
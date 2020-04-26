#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "PugiXml\src\pugixml.hpp"
#include <vector>
#include <string>
#include "Module.h"
#include "PerfTimer.h"
#include "Timer.h"

// Utility Classes
class Timer;
class PerfTimer;

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class Map;
class PathFinding;
class Collisions;
class EntityManager;
class Fonts;
class GuiManager;
class Console;
class Player;
class TransitionManager;
class SceneManager;
class Minimap;

class Application
{
public:
	
	Application(int argc, char* args[]);						// Constructor
	virtual ~Application();										// Destructor

	bool Awake();												// Called before render is available
	bool Start();												// Called before the first frame
	bool Update();												// Called each loop iteration
	bool CleanUp();												// Called before quitting

	void AddModule(Module* module);								// Add a new module to handle

	int GetArgc() const;										// Exposing some properties for reading
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	float GetDt();
	void LoadGame(const char* file);
	void SaveGame(const char* file) const;

	void GetSaveGames(std::vector<std::string>& list_to_fill);


private:
	
	pugi::xml_node LoadConfig(pugi::xml_document&) const;		// Load config file

	void PrepareUpdate();										// Call modules before each loop iteration

	void FinishUpdate();										// Call modules before each loop iteration

	bool PreUpdate();											// Call modules before each loop iteration

	bool DoUpdate();											// Call modules on each loop iteration

	bool PostUpdate();											// Call modules after each loop iteration

	bool LoadGameNow();											// Load / Save
	bool SavegameNow();											//Chenged to non const due to list unknown problem

public:

	// Modules
	Window*				win;
	Input*				input;
	Render*				render;
	Textures*			tex;
	Audio*				audio;
	Fonts*				font;
	Map*				map;
	PathFinding*		pathfinding;
	EntityManager*		entity_manager;
	Collisions*			collisions;
	GuiManager*			gui_manager;
	Console*			console;
	Player*				player;
	TransitionManager*	transition_manager;
	SceneManager*		scene_manager;
	Minimap*			minimap;

	uint				frame_cap;								//Stores the frames per second cap to be applied.
	uint				original_frame_cap;						//Stores the original frame cap at application start.
	float				seconds_since_startup;					//Secons that have elapsed since app start.

	bool				frames_are_capped;						//Keeps track whether the frame cap is on or off.
	bool				vsync_is_active;							//Keeps track whether Vsync is on or off.
	bool				pause;

private:

	std::vector<Module*>	modules;
	int						argc;
	char**					args;

	std::string				title;
	std::string				organization;

	mutable bool			want_to_save;
	bool					want_to_load;
	std::string				load_game;
	mutable std::string		save_game;

	//Framerate
	uint64				frame_count;			
	Timer				startup_timer;							//Used to keep track of time since app start.
	Timer				frame_timer;							//Keeps track of everything time related in the span of a frame.
	PerfTimer			perf_timer;								//Creates a pointer to PerfTimer tool. Gives access to j1PerfTimer's elements. Used to keep track of time since app start.
	PerfTimer			last_second_timer;						//Creates a pointer to PerfTimer tool. Used to calculate variables in spans of one second.
	uint32				last_update_ms;							//Calculates the amount of milliseconds that the last update spent running.
	uint32				frames_last_second;						//Calculates the amount of frames that where processed the last second.
	uint32				prev_sec_frames;		

	PerfTimer			true_delay_timer;						//Timer that will be used to see the actual amount of delay that was applied to cap the framerate.
	float				dt;										//Keeps track of the amount of time in milliseconds that has passed in a frame. 
																//Will be used to make everything (update()) be in the same timestep.
	char*				frameCapOnOff;							//String that is set to 'On' when the frame cap is on and  'Off' when it is off.
	char*				vsyncOnOff;								//String that is set to 'On' when Vsync is on and 'Off' when it is off.
};

extern Application* App; // No student is asking me about that ... odd :-S

#endif
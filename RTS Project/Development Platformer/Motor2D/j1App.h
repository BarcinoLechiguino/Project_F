#ifndef __j1APP_H__
#define __j1APP_H__

#include "p2List.h"
#include "j1Module.h"
#include "j1PerfTimer.h"
#include "j1Timer.h"
#include "PugiXml\src\pugixml.hpp"

// Modules
class j1Window;
class j1Input;
class j1Render;
class j1Textures;
class j1Audio;
class j1SceneMenu;
class j1Scene;
class j1Map;
class j1PathFinding;
class j1Fade_Scene;
class j1Collisions;
class j1EntityManager;
class j1Player1;
class j1Player2;
class j1Fonts;
class j1Gui;
class j1Console;

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
	void AddModule(j1Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	float GetDt();
	void LoadGame(const char* file);
	void SaveGame(const char* file) const;
	void GetSaveGames(p2List<p2SString>& list_to_fill) const;

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
	bool SavegameNow() const;

public:

	// Modules
	j1Window*			win;
	j1Input*			input;
	j1Render*			render;
	j1Textures*			tex;
	j1Audio*			audio;
	j1Fonts*			font;
	j1Scene*			scene;
	j1Map*				map;
	j1PathFinding*		pathfinding;
	j1EntityManager*	entityManager;
	j1Collisions*		collisions;
	j1Player1*			player1;
	j1Player2*			player2;
	j1Fade_Scene*		fadescene;
	j1Gui*				gui;
	j1Console*			console;

	uint				frame_cap;				//Stores the frames per second cap to be applied.
	uint				original_frame_cap;		//Stores the original frame cap at application start.
	float				seconds_since_startup;	//Secons that have elapsed since app start.

	bool				framesAreCapped;		//Keeps track whether the frame cap is on or off.
	bool				vsyncIsActive;			//Keeps track whether Vsync is on or off.
	bool				pause;

private:

	p2List<j1Module*>	modules;
	int					argc;
	char**				args;

	p2SString			title;
	p2SString			organization;

	mutable bool		want_to_save;
	bool				want_to_load;
	p2SString			load_game;
	mutable p2SString	save_game;

	//Framerate
	uint64				frame_count;			//Calculates the amount of frames per second. frames++ after each Update() loop.
	j1Timer				startup_timer;			//Creates a pointer to j1Timer tool. Gives acces to j1Timer's elements. Used to keep track of time since app start.
	j1Timer				frame_timer;			//Keeps track of everything time related in the span of a frame.
	j1PerfTimer			perf_timer;				//Creates a pointer to j1PerfTimer tool. Gives access to j1PerfTimer's elements. Used to keep track of time since app start.
	j1PerfTimer			last_second_timer;		//Creates a pointer to j1PerfTimer tool. Used to calculate variables in spans of one second.
	uint32				last_update_ms;			//Calculates the amount of milliseconds that the last update spent running.
	uint32				frames_last_second;		//Calculates the amount of frames that where processed the last second.
	uint32				prev_sec_frames;		//Stores the number of frames processed last second.

	j1PerfTimer			true_delay_timer;		//Timer that will be used to see the actual amount of delay that was applied to cap the framerate.
	float				dt;						//Keeps track of the amount of time in milliseconds that has passed in a frame. Will be used to make everything (update()) be in the same timestep.

	char*				frameCapOnOff;			//String that is set to 'On' when the frame cap is on and  'Off' when it is off.
	char*				vsyncOnOff;				//String that is set to 'On' when Vsync is on and 'Off' when it is off.
};

extern j1App* App; // No student is asking me about that ... odd :-S

#endif
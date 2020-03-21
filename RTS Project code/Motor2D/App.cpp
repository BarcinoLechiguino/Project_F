#include <iostream> 
#include <list>
#include "p2Defs.h"
#include "p2Log.h"
#include "App.h"
#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Scene.h"
#include "Main_Menu.h"
#include "Map.h"
#include "Fonts.h"
#include "EntityManager.h"
#include "Pathfinding.h"
#include "Collisions.h"
#include "Gui.h"
#include "Console.h"
#include "FadeScene.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"

//#include "mmgr/mmgr.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(perf_timer);

	want_to_save = want_to_load = false;

	input			= new Input();
	win				= new Window();
	render			= new Render();
	tex				= new Textures();
	audio			= new Audio();
	scene			= new Scene();
	map				= new Map();
	pathfinding		= new PathFinding();
	collisions		= new Collisions();
	entityManager	= new EntityManager();
	fadescene		= new Fade_Scene();
	font			= new Fonts();
	gui				= new Gui();
	console			= new Console();
	mainmenu		= new Main_Menu();
	player			= new Player();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(pathfinding);
	AddModule(font);
	AddModule(gui);
	AddModule(console);
	AddModule(mainmenu);
	AddModule(scene);
	AddModule(entityManager);
	AddModule(player);
	AddModule(collisions);
	AddModule(fadescene);
	
	// render last to swap buffer
	AddModule(render);

	scene->is_active = false;

	pause = false;

	PERF_PEEK(perf_timer);
}

// Destructor
j1App::~j1App()
{
	// release modules
	for (std::list<Module*>::iterator item = modules.begin(); item != modules.end() ; ++item)
	{
		RELEASE((*item));
	}

	modules.clear();
}

void j1App::AddModule(Module* module)
{
	module->Init();
	modules.push_back(module);
}

// Called before render is available
bool j1App::Awake()
{
	PERF_START(perf_timer);
	
	pugi::xml_document	config_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	bool ret = false;
		
	config = LoadConfig(config_file);

	frame_cap = CAP_AT_60;
	
	if(!config.empty())
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title = (app_config.child("title").child_value()); //Calling constructor
		organization = (app_config.child("organization").child_value());
		frame_cap = config.child("app").attribute("framerate_cap").as_uint();
		framesAreCapped = config.child("app").attribute("frame_cap_on").as_bool();

		original_frame_cap = frame_cap;
	}

	if(ret)
	{
		for (std::list<Module*>::iterator item = modules.begin() ; item != modules.end() && ret; ++item)
		{
			ret  = (*item)->Awake(config.child((*item)->name.c_str()));
		}
	}
	
	PERF_PEEK(perf_timer);

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	PERF_START(perf_timer);
	
	bool ret = true;

	//Disable scene
	scene->Disable();

	for (std::list<Module*>::iterator item = modules.begin(); item != modules.end() && ret ; ++item)
	{
		ret = (*item)->Start();
	}

	startup_timer.Start();

	PERF_PEEK(perf_timer);

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	BROFILER_CATEGORY("Update_App.cpp", Profiler::Color::Aqua)
	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT))
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	frame_count++;
	frames_last_second++;

	dt = frame_timer.ReadSec();						//Keeps track of the amount of time that has passed since last frame in seconds (processing time of a frame: Frame 1: 0.033secs, ...).
	frame_timer.Start();

	//LOG("The differential time since last frame: %f", dt);
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	if(want_to_save)
		SavegameNow();

	if(want_to_load)
		LoadGameNow();

	//------------ Framerate Calculations ------------
	if (last_second_timer.ReadMs() > 1000)
	{
		last_second_timer.Start();
		prev_sec_frames = frames_last_second;
		frames_last_second = 0;
	}

	float frame_cap_ms = 1000 / frame_cap;
	float current_frame_ms = frame_timer.Read();					

	if (framesAreCapped)
	{
		if (current_frame_ms < frame_cap_ms)						//If the current frame processing time is lower than the specified frame_cap. Timer instead of PerfTimer was used because SDL_Delay is inaccurate.
		{
			true_delay_timer.Start();

			SDL_Delay(frame_cap_ms - current_frame_ms);				//SDL_Delay delays processing for a specified time. In this case, it delays for the difference in ms between the frame cap (30fps so 33,3ms per frame) and the current frame.

			int intended_delay = frame_cap_ms - current_frame_ms;

			//LOG("We waited for %d milliseconds and got back in %f", intended_delay, true_delay_timer.ReadMs());
		}
	}
	
	float avg_fps = frame_count / startup_timer.ReadSec();
	seconds_since_startup = startup_timer.ReadSec();				
	uint32 last_frame_ms = frame_timer.Read();
	uint32 frames_on_last_update = prev_sec_frames;					//Keeps track of how many frames were processed the last second.

	if (framesAreCapped)
	{
		frameCapOnOff = "On";
	}
	else
	{
		frameCapOnOff = "Off";
	}

	if (vsyncIsActive)
	{
		vsyncOnOff = "On";
	}
	else
	{
		vsyncOnOff = "Off";
	}

	static char title[256];

	sprintf_s(title, 256, "Av.FPS: %.2f / Last Frame Ms: %02u / Last sec frames: %i / Last dt: %.3f / Time since startup: %.3f / Frame Count: %lu / %d / Vsync: %s / Frame cap: %s",
		avg_fps, last_frame_ms, frames_on_last_update, dt, seconds_since_startup, frame_count, vsyncOnOff, frameCapOnOff , frameCapOnOff);

	App->win->SetTitle(title);
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	BROFILER_CATEGORY("PreUpdate_App.cpp", Profiler::Color::Aqua)
	bool ret = true;

	for (std::list<Module*>::iterator item = modules.begin(); item != modules.end() && ret ; ++item)
	{
		if(!(*item)->is_active)
		{
			continue;
		}

		ret = (*item)->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;

	for (std::list<Module*>::iterator item = modules.begin(); item != modules.end() && ret; ++item)
	{
		if(!(*item)->is_active)
		{
			continue;
		}

		if (!pause)
		{
			ret = (*item)->Update(dt);			//Passes the calculated dt as an argument to all modules. This will make every update run in the same timestep.
		}
		else
		{
			ret = (*item)->Update(0.0f);
		}
	}
	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	BROFILER_CATEGORY("PostUpdate_App.cpp", Profiler::Color::Aqua)
	bool ret = true;

	for (std::list<Module*>::iterator item = modules.begin(); item != modules.end() && ret; ++item)
	{

		if(!(*item)->is_active)
		{
			continue;
		}

		ret = (*item)->PostUpdate();
	}
	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	bool ret = true;

	for (std::list<Module*>::iterator item = modules.begin(); item != modules.end() && ret; ++item)
	{
		if ((*item)->name.empty())
		{
			ret = (*item)->CleanUp();
		}
	}

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.c_str();
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.c_str();
}

float j1App::GetDt()
{
	//LOG("dt is: %f", dt);
	if (!pause)
	{
		return dt;
	}
	else
	{
		return 0.0f;
	}
}

// Load / Save
void j1App::LoadGame(const char* file)
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
}

// ---------------------------------------
void j1App::SaveGame(const char* file) const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	want_to_save = true;
	save_game = (file);
}

// ---------------------------------------
void GetSaveGames(std::list<std::string>& list_to_fill)
{
	// need to add functionality to file_system module for this to work
}

bool j1App::LoadGameNow()
{
	bool ret = false;

	load_game = ("save_game.xml");

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.load_file(load_game.c_str());

	if (result != NULL)
	{
		LOG("Loading new Game State from %s...", load_game.c_str());

		root = data.child("game_state");

		ret = true;

		for (std::list<Module*>::iterator item = modules.begin(); item != modules.end() && ret; ++item)
		{
			ret = (*item)->Load(root.child((*item)->name.c_str()));
		}

		data.reset();
		
		//Commented
		/*if (ret == true)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", ((*item) != NULL) ? (*item)->name.GetString() : "unknown");*/
	}
	else
	{
		LOG("Could not parse game state xml file %s. pugi error: %s", load_game.c_str(), result.description());
	}

	want_to_load = false;

	return ret;
}

bool j1App::SavegameNow() //Chenged to non const due to list unknown problem
{
	bool ret = true;

	save_game = ("save_game.xml");

	LOG("Saving Game State to %s...", save_game.c_str());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	root = data.append_child("game_state");

	for (std::list<Module*>::iterator item = modules.begin() ; item != modules.end() && ret; ++item)
	{
		ret = (*item)->Save(root.append_child((*item)->name.c_str()));
	}

	/*if (ret == true)
	{
		data.save_file(save_game.GetString());
		LOG("... finished saving", );
	}
	else
		LOG("Save process halted from an error in module %s", ((*item) != NULL) ? (*item)->name.GetString() : "unknown");*/


	data.reset();
	want_to_save = false;

	return ret;
}														
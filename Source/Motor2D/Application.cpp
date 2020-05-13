#include <iostream> 
#include <list>
#include <vector>
#include "Definitions.h"
#include "Log.h"
#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Map.h"
#include "Fonts.h"
#include "EntityManager.h"
#include "Pathfinding.h"
#include "Collisions.h"
#include "GuiManager.h"
#include "Console.h"
#include "Player.h"
#include "Minimap.h"
#include "TransitionManager.h"
#include "SceneManager.h"
#include "Movement.h"
#include "FowManager.h"
#include "DialogSystem.h"
#include "ParticleManager.h"

#include "Brofiler\Brofiler.h"

//#include "mmgr/mmgr.h"

// Constructor
Application::Application(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(perf_timer);

	want_to_save = want_to_load = false;

	frame_count = 0;

	input				= new Input();
	win					= new Window();
	render				= new Render();
	tex					= new Textures();
	audio				= new Audio();
	map					= new Map();
	pathfinding			= new PathFinding();
	collisions			= new Collisions();
	entity_manager		= new EntityManager();
	minimap				= new Minimap();
	font				= new Fonts();
	gui_manager			= new GuiManager();
	console				= new Console();
	player				= new Player();
	transition_manager	= new TransitionManager();
	scene_manager		= new SceneManager();
	movement			= new Movement();
	fow_manager			= new FowManager();
	dialog				= new DialogSystem();
	particle_manager	= new ParticleManager();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(pathfinding);
	AddModule(font);
	AddModule(console);
	AddModule(collisions);
	AddModule(movement);
	

	// scene_manager last before render.
	AddModule(particle_manager);
	AddModule(minimap);
	AddModule(gui_manager);
	AddModule(scene_manager);
	AddModule(transition_manager);
	AddModule(entity_manager);
	AddModule(fow_manager);
	AddModule(dialog);
	AddModule(player);

	// render last to swap buffer
	AddModule(render);

	pause = false;

	PERF_PEEK(perf_timer);
}

// Destructor
Application::~Application()
{
	std::vector<Module*>::iterator item = modules.begin();

	// release modules
	for (; item != modules.end() ; ++item)
	{
		RELEASE((*item));
	}

	modules.clear();
}

void Application::AddModule(Module* module)
{
	module->Init();
	modules.push_back(module);
}

// Called before render is available
bool Application::Awake()
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
		app_config			= config.child("app");
		title				= (app_config.child("title").child_value());					//Calling constructor
		organization		= (app_config.child("organization").child_value());
		frame_cap			= config.child("app").attribute("framerate_cap").as_uint();
		frames_are_capped	= config.child("app").attribute("frame_cap_on").as_bool();

		original_frame_cap	= frame_cap;
	}
	int i = 0;
	if(ret)
	{
		std::vector<Module*>::iterator item = modules.begin();

		for (; item != modules.end() && ret; ++item)
		{
			i++;
			ret  = (*item)->Awake(config.child((*item)->name.c_str()));
			if (!ret)
			{
				LOG("AWAKE RET IS FALSE %d", i);
			}
			
		}
	}
	
	PERF_PEEK(perf_timer);

	return ret;
}

// Called before the first frame
bool Application::Start()
{
	PERF_START(perf_timer);
	
	bool ret = true;

	int i = 0;

	std::vector<Module*>::iterator item = modules.begin();

	for (; item != modules.end() && ret ; ++item)
	{
		if ((*item)->is_active)
		{
			i++;
			ret = (*item)->Start();
			if (!ret)
			{
				LOG("START RET IS FALSE %d", i);
			}
		}
	}

	startup_timer.Start();

	PERF_PEEK(perf_timer);

	return ret;
}

// Called each loop iteration
bool Application::Update()
{
	BROFILER_CATEGORY("Update_App.cpp", Profiler::Color::Aqua)
	bool ret = true;
	PrepareUpdate();


	/*if(input->GetWindowEvent(WE_QUIT))
		ret = false;*/

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
pugi::xml_node Application::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

pugi::xml_node Application::LoadParticleSystemConfig(pugi::xml_document& ps_file) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = ps_file.load_file("ps_config");

	if (result == NULL)
		LOG("Could not load xml file config.xml. pugi error: %s", result.description());
	else
		ret = ps_file.child("particlesystem");
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	frame_count++;
	frames_last_second++;

	dt = frame_timer.ReadSec();						//Keeps track of the amount of time that has passed since last frame in seconds (processing time of a frame: Frame 1: 0.033secs, ...).
	frame_timer.Start();

	//LOG("The differential time since last frame: %f", dt);
}

// ---------------------------------------------
void Application::FinishUpdate()
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

	uint frame_cap_ms = 1000 / frame_cap;
	uint current_frame_ms = frame_timer.Read();					

	if (frames_are_capped)
	{
		if (current_frame_ms < frame_cap_ms)						//If the current frame processing time is lower than the specified frame_cap. Timer instead of PerfTimer was used because SDL_Delay is inaccurate.
		{
			true_delay_timer.Start();

			SDL_Delay(frame_cap_ms - current_frame_ms);				//SDL_Delay delays processing for a specified time. In this case, it delays for the difference in ms between the frame cap (30fps so 33,3ms per frame) and the current frame.

			uint intended_delay = frame_cap_ms - current_frame_ms;

			//LOG("We waited for %d milliseconds and got back in %f", intended_delay, true_delay_timer.ReadMs());
		}
	}
	
	float avg_fps = frame_count / startup_timer.ReadSec();
	seconds_since_startup = startup_timer.ReadSec();				
	uint32 last_frame_ms = frame_timer.Read();
	uint32 frames_on_last_update = prev_sec_frames;					//Keeps track of how many frames were processed the last second.

	if (frames_are_capped)
	{
		frame_cap_on_off = "On";
	}
	else
	{
		frame_cap_on_off = "Off";
	}

	if (vsync_is_active)
	{
		vsync_on_off = "On";
	}
	else
	{
		vsync_on_off = "Off";
	}

	static char title[256];

	sprintf_s(title, 256, "Av.FPS: %.2f / Last Frame Ms: %02u / Last sec frames: %i / Last dt: %.3f / Time since startup: %.3f / Frame Count: %llu / Vsync: %s / Frame cap: %s",
		avg_fps, last_frame_ms, frames_on_last_update, dt, seconds_since_startup, frame_count, vsync_on_off, frame_cap_on_off);

	App->win->SetTitle(title);
}

// Call modules before each loop iteration
bool Application::PreUpdate()
{
	BROFILER_CATEGORY("PreUpdate_App.cpp", Profiler::Color::Aqua)
	bool ret = true;

	std::vector<Module*>::iterator item = modules.begin();

	for (; item != modules.end() && ret ; ++item)
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
bool Application::DoUpdate()
{
	bool ret = true;
	int i = 0;

	std::vector<Module*>::iterator item = modules.begin();

	for (; item != modules.end() && ret; ++item)
	{
		i++;
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
bool Application::PostUpdate()
{
	BROFILER_CATEGORY("PostUpdate_App.cpp", Profiler::Color::Aqua)
	bool ret = true;

	std::vector<Module*>::iterator item = modules.begin();

	for (; item != modules.end() && ret; ++item)
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
bool Application::CleanUp()
{
	bool ret = true;

	std::vector<Module*>::iterator item = modules.begin();

	for (; item != modules.end() && ret; ++item)
	{
		if ((*item)->name.empty())
		{
			ret = (*item)->CleanUp();
		}
	}

	return ret;
}

// ---------------------------------------
int Application::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* Application::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* Application::GetTitle() const
{
	return title.c_str();
}

// ---------------------------------------
const char* Application::GetOrganization() const
{
	return organization.c_str();
}

float Application::GetDt() const
{
	if (!pause)
	{
		return dt;
	}
	else
	{
		return 0.0f;
	}
}

float Application::GetUnpausableDt() const
{
	return dt;
}

// Load / Save
void Application::LoadGame(const char* file)
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
}

// ---------------------------------------
void Application::SaveGame(const char* file) const
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

bool Application::LoadGameNow()
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

		std::vector<Module*>::iterator item = modules.begin();

		for (; item != modules.end() && ret; ++item)
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

bool Application::SavegameNow() //Chenged to non const due to list unknown problem
{
	bool ret = true;

	save_game = ("save_game.xml");

	LOG("Saving Game State to %s...", save_game.c_str());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	root = data.append_child("game_state");

	std::vector<Module*>::iterator item = modules.begin();

	for (; item != modules.end() && ret; ++item)
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
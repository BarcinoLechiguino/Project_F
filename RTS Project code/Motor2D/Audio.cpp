#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Audio.h"
#include "Input.h"



#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

Audio::Audio() : Module()
{
	//music = NULL;
	name = ("audio");
}

// Destructor
Audio::~Audio()
{}

// Called before render is available
bool Audio::Awake(pugi::xml_node& config)
{

	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		is_active = false;
		ret = true;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	volume = (float)config.child("volume").attribute("value").as_int(96);
	volume_fx = config.child("volume_fx").attribute("value").as_int(26);
	scale = config.child("distance").attribute("scale").as_int();	// Load the distance scale if you want to change it

	if((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		is_active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		is_active = false;
		ret = true;
	}

	//music_folder = (config.child("folder").child_value());

	Mix_AllocateChannels(360);

	return ret;
}

bool Audio::PreUpdate(float dt) {

	if (App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KeyState::KEY_REPEAT)
	{
		volume++;
	}
	else if (App->input->GetKey(SDL_SCANCODE_KP_MINUS) == KeyState::KEY_REPEAT)
	{
		volume--;
	}
	return true;
}

bool Audio::Update(float dt)
{
	Mix_VolumeMusic((int)volume);

	std::vector<Mix_Chunk*>::iterator chunk_item = fx.begin();

	for (; chunk_item != fx.end() ; ++chunk_item)
	{
		Mix_VolumeChunk((*chunk_item), volume_fx);
	}

	return true;
}

// Called before quitting
bool Audio::CleanUp()
{
	if(!is_active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");
	std::vector<Mix_Music*>::iterator stl_item = music.begin(); //Release music list
	for (; stl_item != music.end(); stl_item++)
	{
		Mix_FreeMusic(*stl_item);
	}

	std::vector<Mix_Chunk*>::iterator stl_item2 = fx.begin(); //Release fx list
	for (; stl_item2 != fx.end(); stl_item2++)
	{
		Mix_FreeChunk(*stl_item2);
	}

	fx.clear();
	music.clear();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}


unsigned int Audio::LoadMusic(std::string path) // Loads the audio on the Mix_Music* 
{
	unsigned int ret = 0;

	if (!is_active)
		return 0;

	Mix_Music* music_chunk = Mix_LoadMUS(path.c_str());

	if (music_chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		//TODO 5.1 Add the previous audio into the list
		music.push_back(music_chunk);
		ret = music.size();
	}

	return ret;
}

// Play a music file
bool Audio::PlayMusic(uint id, float fade_time)
{
	//std::string tmp = music_folder + path;

	bool ret = true;

	if (!is_active)
		return false;

	if (id > 0 && id <= music.size())
	{
		std::vector <Mix_Music*>::const_iterator it;
		it = std::next(music.begin(), id - 1);

		if (*it != NULL)
		{
			if (fade_time > 0.0f)
			{
				Mix_FadeOutMusic(int(fade_time * 1000.0f));
			}
			else
			{
				Mix_HaltMusic();
			}
		}
		if (fade_time > 0.0f)
		{
			Mix_FadeInMusic(*it, -1, (int)(fade_time * 1000.0f));
		}
		else if (fade_time == 0.0f)
		{
			Mix_PlayMusic(*it, -1);
		}

	}

	return ret;
}

void Audio::PauseMusic(float fade_time)
{
	if (is_active)
	{
		if (Mix_PlayingMusic() == 1)	// Sees if there is music playing
		{
			if (Mix_PausedMusic() == 1)	// If there is resume it
			{
				Mix_ResumeMusic();
			}
			else
			{
				Mix_PauseMusic();
			}
		}
	}
}

// Load WAV
unsigned int Audio::LoadFx(std::string path)
{
	unsigned int ret = 0;

	if(!is_active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());

	if(chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
uint Audio::PlayFx(unsigned int id, int repeat)
{
	uint channel;

	if(!is_active)
		return false;

	if(id > 0 && id <= fx.size())
	{
		std::vector<Mix_Chunk*>::iterator fx_item = fx.begin();

		std::advance(fx_item, id - 1);

		channel = Mix_PlayChannel(-1, (*fx_item) , repeat);
	}

	return channel;
}

bool Audio::PlaySpatialFx(uint id, uint channel_angle, uint distance, int repeat)
{
	bool ret = false;

	if (!is_active)
		return false;

	Mix_Chunk* chunk = NULL;

	if (id > 0 && id <= fx.size())
	{
		std::vector <Mix_Chunk*>::const_iterator it;
		it = std::next(fx.begin(), id - 1);
		chunk = *it;
	}
	if (chunk != nullptr)
	{
		while (Mix_Playing(channel_angle) == 1)	// If the channel is already playing, choose the next channel that we already allocated with Mix_AllocateChannels()
		{
			channel_angle++;

			if (channel_angle > 360)
				channel_angle = 0;
		}

		// TODO 2 Set a channel in a position given a channel, an angle and a distance, There is SDL_Mixer function already explained 
		// Play the channel that we already placed with Mix_SetPosition()
		Mix_SetPosition(channel_angle, channel_angle, distance);	// Set a channel in a position given a channel, an angle and a distance

		Mix_PlayChannel(channel_angle, chunk, repeat);				// Play the channel that we already placed with Mix_SetPosition()

		ret = true;
	}

	return ret;
}

uint Audio::GetAngle(iPoint player_pos, iPoint enemy_pos)
{
	iPoint vector_pos = player_pos - enemy_pos;				// The vector of the player and enemy positions
	iPoint vector_axis = { 0, 1 };							// We use the this vector because we want the angle that is formed with the Y axis

	double dot_x = vector_axis.y * vector_pos.y;			// Product of the two vectors to get the X position
	double det_y = -(vector_axis.y * vector_pos.x);			// Determinant of the two vectors to get the Y position

	float f_angle = ((float)atan2(det_y, dot_x)) * RAD_TO_DEG;		// Arc tangent of the previous X and Y, multiply the result with RAD_TO_DEG to get the result in degrees instead of radiants

	if (f_angle < 0)										// If the angle is negative we add +360 because in PlaySpatialFx() we need the channel to be positive
		f_angle += 360;

	return uint(f_angle);
}

uint Audio::GetDistance(iPoint player_pos, iPoint enemy_pos)
{

	// TODO 3 Calculate the distance between the player and the enemy passed by reference using pythagoras
	uint distance = (int)sqrt(pow(player_pos.x - enemy_pos.x, 2) + pow(player_pos.y - enemy_pos.y, 2));	// Calculate the distance with Pythagoras

	uint distance_scaled = (distance * MAX_DISTANCE) / scale;										// We can scale the maximum hear distance by modifying scale in the config XML

	if (distance_scaled > MAX_DISTANCE)																// If the distance is greater than the MAX_DISTANCE(255), keep it in 255
		distance_scaled = MAX_DISTANCE;

	return distance_scaled;
}

bool Audio::Load(pugi::xml_node & data)
{
	volume = (float)data.child("volume").attribute("value").as_uint();
	return true;
}

bool Audio::Save(pugi::xml_node& data) const
{
	pugi::xml_node vol = data.append_child("volume");

	vol.append_attribute("value") = volume;

	return true;
}

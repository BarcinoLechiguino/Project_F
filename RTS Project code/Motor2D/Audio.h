#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "SDL_mixer\include\SDL_mixer.h"
#include "Point.h"
#include "Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define MAX_FX 500						// The maximum number of fx
#define RAD_TO_DEG 57.32f				// The result of 180 / 3.14 for pass radiants to degrees
#define MAX_DISTANCE 255				// The maximum distance from which you can listen

struct _Mix_Music;
struct Mix_Chunk;

class Audio : public Module
{
public:

	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	bool PreUpdate(float dt);
	bool Update(float dt);

	// Load an OGG in memory
	unsigned int LoadMusic(std::string path); // research a way to convert to std::string

	// Play a music file
	bool PlayMusic(uint id, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Pause the music
	void PauseMusic(float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(std::string path);

	// Play a previously loaded WAV
	uint PlayFx(uint id, int repeat = 0);
	
	// Play a WAV like a 3D audio reciving an fx, a channel, an angle and a distance
	bool PlaySpatialFx(uint id, uint channel_angle = 1, uint distance = 1, int repeat = 0);

	// Get the angle of the Y axis with the position of the enemy regard the player position
	uint GetAngle(iPoint player_pos, iPoint enemy_pos);

	// Get the distance with the position of the player and the enemy
	uint GetDistance(iPoint player_pos, iPoint enemy_pos);

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

private:

	//_Mix_Music*			music;
	std::list<Mix_Chunk*>	fx;
	std::list<Mix_Music*> music;
	int					scale;

public:
	float				volume;
	uint				volume_fx;
	//std::string			music_folder;
	//std::string			sfx_folder;
	uint				general_volume;
};

#endif // __AUDIO_H__
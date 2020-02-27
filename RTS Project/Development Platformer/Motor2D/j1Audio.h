#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"
#include "SDL_mixer\include\SDL_mixer.h"
#include "p2List.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	bool PreUpdate(float dt);
	bool Update(float dt);

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

private:

	_Mix_Music*			music;
	p2List<Mix_Chunk*>	fx;
	
	uint				volume_fx;

public:
	float				volume;
	p2SString			music_folder;
	p2SString			sfx_folder;
	uint general_volume;
};

#endif // __j1AUDIO_H__
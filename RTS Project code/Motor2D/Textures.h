#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include "Module.h"
#include "Render.h"
#include "p2Defs.h"

struct SDL_Texture;
struct SDL_Surface;
struct SDL_Renderer;

class Textures : public Module
{
public:

	Textures();

	// Destructor
	virtual ~Textures();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before quitting
	bool CleanUp();

	// Load Texture
	SDL_Texture* const	Load(const char* path, SDL_Renderer* renderer = App->render->renderer);
	bool				UnLoad(SDL_Texture* texture);
	SDL_Texture* const	LoadSurface(SDL_Surface* surface, SDL_Renderer* renderer = App->render->renderer);
	void				GetSize(const SDL_Texture* texture, uint& width, uint& height) const;

public:

	std::list<SDL_Texture*>	textures;
};


#endif // __TEXTURES_H__
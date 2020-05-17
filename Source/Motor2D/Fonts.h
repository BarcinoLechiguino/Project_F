#ifndef __FONTS_H__
#define __FONTS_H__

#include "Dependencies\SDL\include\SDL_pixels.h"
#include "Module.h"

#define DEFAULT_FONT "fonts/arial.ttf"
#define DEFAULT_FONT_SIZE 22

struct SDL_Texture;
struct _TTF_Font;

class Fonts : public Module
{
public:
	Fonts();
	virtual ~Fonts();												// Destructor
	bool Awake(pugi::xml_node&);									// Called before render is available
	bool Start();													// Called before the first frame.
	bool CleanUp();													// Called before quitting

public:
	_TTF_Font* const Load(const char* path, int size = 12);														// Load Font.
	
	SDL_Texture* Print(const char* text, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL);		// Create a surface from text.

	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL) const;						//Returns the size of of the texture of a given text.

public:
	std::vector<_TTF_Font*>	fonts;

	_TTF_Font*				default;
	_TTF_Font*				default_bigger;
	_TTF_Font*				title_buttons;
	_TTF_Font*				main_title_buttons;
	_TTF_Font*				main_title;
	_TTF_Font*				subtitle;
	_TTF_Font*				titles_other;
	_TTF_Font*				hud;
};


#endif // __FONTS_H__

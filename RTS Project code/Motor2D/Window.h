#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;

class Window : public Module
{
public:

	Window();
	virtual ~Window();														// Destructor

	bool Awake(pugi::xml_node&);											// Called before render is available
	bool CleanUp();															// Called before quitting

	void SetTitle(const char* new_title);									// Change title
	void GetWindowSize(int& width, int& height) const;						// Retrive window size
	void GetWindowRect(SDL_Rect& rect) const;								// Method that will get the current dimensions of the screen and assign them to a given rect.

	void ToggleFullscreen();

	float GetScale() const;													// Retrieve window scale
	void SetScale(float new_scale);											// Modify window scale.

public:
	SDL_Window* window;														//The window we'll be rendering to
	SDL_Surface* screen_surface;											//The surface contained by the window

	std::string	title;
	int			width;
	int			height;
	float		scale;
};

#endif // __WINDOW_H__
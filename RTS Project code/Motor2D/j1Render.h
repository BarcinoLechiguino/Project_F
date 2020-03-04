#ifndef __j1RENDER_H__
#define __j1RENDER_H__

#include "SDL/include/SDL.h"
#include "p2Point.h"
#include "j1Module.h"

struct fixedCamera
{
	p2Point<float>	MidPos;					//Keeps track of the mid position between P1 and P2
	p2Point<float>	MidPosPostMovement;		//MidPosition after movement. Related to lerp.
	p2Point<float>	p1;						//Keeps track of the position the camera would be if it was fixed on P1
	p2Point<float>	p2;						//Keeps track of the position the camera would be if it was fixed on P1
	p2Point<float>	p1PostMovement;			//Keeps track of the position of P1 after movement. Related to lerp.
	p2Point<float>	p2PostMovement;			//Keeps track of the position of P1 after movement. Related to lerp.
	p2Point<uint>	mapLimit;				//Keeps track of the map's delimitations.

	uint			WinWidth;				//Width of the executing window.
	uint			WinHeight;				//Height of the executing window.
	float			smoothingSpeed;			//Value that defines how much time it takes the camera to reach its destination. Should be kept between 0 and 1 (Closer to 0 slower, closer to 1 faster). Related to lerp.

	bool			camera_debug;			//Will enable or disable camera limits
	float			camera_debug_speed;	//Camera's movement speed while in camera debug mode.

	//Lerp Method. Takes the position where, for example, P1 is at, the position  P1 wants to go to and the smoothingSpeed.
	float lerp(float position, float target, float amount) 
	{
		float trail = (target - position) * amount;

		return position + trail;
	}
};

class j1Render : public j1Module
{
public:

	j1Render();

	// Destructor
	virtual ~j1Render();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	//Useful Methods
	void SetViewPort(const SDL_Rect& collider);
	void ResetViewPort();
	iPoint ScreenToWorld(int x, int y) const;		//Translates a position (x, y) on the screen to a position in the world.

	// Blit
	bool Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, bool flip = false, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;
	bool DrawQuad(const SDL_Rect& collider, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;

	// Set background color
	void SetBackgroundColor(SDL_Color color);

public:
	fixedCamera cam;

	SDL_Renderer*	renderer;
	SDL_Rect		camera;
	SDL_Rect		viewport;
	SDL_Color		background;
};

#endif // __j1RENDER_H__
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1EntityManager.h"		
#include "j1Player1.h"
#include "j1Player2.h"
#include "Brofiler\Brofiler.h"

#define VSYNC true

j1Render::j1Render() : j1Module()
{
	name.create("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)									//Renderer VSync --> Flags turn on and of the VSync
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	//App->vsyncIsActive = config.child("vsync").attribute("value").as_bool();

	if (config.child("vsync").attribute("value").as_bool(true) == true)	//If vsync is true in the xml file.
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
		App->vsyncIsActive = true;
	}
	else
	{
		App->vsyncIsActive = false;
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if (renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = config.child("camera").attribute("x").as_float();
		camera.y = config.child("camera").attribute("y").as_float();

		cam.camera_debug = config.child("camera_debug").attribute("value").as_bool();				//Loads the cameraLimit bool state from the config xml file.
		cam.camera_debug_speed = config.child("camera_debug").attribute("speed").as_float();		//Loads the camera's movement speed value while in debug mode  from the config xml file.
		cam.smoothingSpeed = config.child("camera").attribute("smoothing_speed").as_float();		//Loads the camera smoothing speed value from the config xml file.
	}

	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	BROFILER_CATEGORY("Render PreUpdate", Profiler::Color::DeepSkyBlue);
	SDL_RenderClear(renderer);
	return true;
}

bool j1Render::Update(float dt)
{
	BROFILER_CATEGORY("Render Update", Profiler::Color::DeepSkyBlue);
	App->win->GetWindowSize(cam.WinWidth, cam.WinHeight);		//Last pixel of window is the 0,0 of the window we see. So we need to add + cam.WinWidth or + cam.WinHeight to set it where we want it to be.

	fPoint p1Pos = App->entityManager->player->position;													//Sets an fPoint with the position data members of the player1 player entity. Done for readability.
	iPoint p1Size( App->entityManager->player->sprite_width , App->entityManager->player->sprite_height);	//Sets an iPoint with the size data members of the player1 player entity. Done for readability.	
	fPoint p2Pos = App->entityManager->player2->position;													//Sets an fPoint with the position data members of the player2 player entity. Done for readability.
	iPoint p2Size(App->entityManager->player->sprite_width , App->entityManager->player->sprite_height);	//Sets an iPoint with the size data members of the player2 player entity. Done for readability.	


	//Positions of the camera if it was centered around only one player. Used a p2Point<float> to translate all those long and convoluted expressions to a much more readable state.
	cam.p1.x = -p1Pos.x + cam.WinWidth / 2 - p1Size.x;				//Sets the camera p1 point X axis position with the position data members of player 1.
	cam.p1.y = -p1Pos.y + (cam.WinHeight / 2) - p1Size.y / 2;		//Sets the camera p2 point Y axis position with the position data members of player 2.
	cam.p2.x = -p2Pos.x + cam.WinWidth / 2 - p2Size.x;				//Sets the camera p1 point X axis position with the position data members of player 1.	
	cam.p2.y = -p2Pos.y + (cam.WinHeight / 2) - p2Size.y / 2;		//Sets the camera p2 point Y axis position with the position data members of player 2.

	//Calculating the central position. 
	if (p2Pos.x > p1Pos.x)											//THIS HERE. Change when the player 2 player subclass is created.
	{
		cam.MidPos.x = cam.p2.x - ((cam.p2.x - cam.p1.x) / 2);
		cam.MidPos.y = cam.p2.y - ((cam.p2.y - cam.p1.y) / 2);
	}
	else
	{
		cam.MidPos.x = cam.p1.x - ((cam.p1.x - cam.p2.x) / 2);
		cam.MidPos.y = cam.p1.y - ((cam.p1.y - cam.p2.y) / 2);
	}

	//We calculate the delimitations of the map making use of the map data we already have.
	cam.mapLimit.x = -(App->map->data.tile_width * App->map->data.width) + cam.WinWidth;		//data.tile_width refers to the tile's width in pixels and data.width refers to the map's total width in tiles
	cam.mapLimit.y = -(App->map->data.tile_height * App->map->data.height) + cam.WinHeight;		//data.tile_height refers to the tile's height in pixels and data.height refers to the map's total height in tiles.
	
	if (cam.camera_debug == false)									//If camera_debug is true then the camera is freed and can be operated to be moved wherever on the world.
	{
		//We set the camera position according to the mid positions.
		camera.x = cam.MidPos.x;
		camera.y = cam.MidPos.y;

		//Camera limits
		if (camera.x >= 0)											//Camera is at the leftmost part of the map in the x axis.
		{
			camera.x = 0;
		}
		else if (camera.x <= cam.mapLimit.x)						//Camera is at the rightmost part of the map in the x axis.
		{
			camera.x = cam.mapLimit.x;
		}

		if (camera.y > 0)											//Camera is at the highest part of the map in the y axis.
		{
			camera.y = 0;
		}
		else if (camera.y < cam.mapLimit.y)							//Camera is at the lowest part of the map.
		{
			camera.y = cam.mapLimit.y;
		}
	}
	
	return true;
}

bool j1Render::PostUpdate()
{
	BROFILER_CATEGORY("Render PostUpdate", Profiler::Color::DeepSkyBlue);
	//Trying to implement lerping on the y axis of the camera.
	/*if (App->player2->p2.position.x > App->player1->p1.position.x)
	{
		cam.MidPosPostMovement.y = cam.p2.y - ((cam.p2.y - cam.p1.y) / 2);
	}
	else
	{
		cam.MidPosPostMovement.y = cam.p1.y - ((cam.p1.y - cam.p2.y) / 2);
	}

	camera.y = cam.lerp(cam.MidPos.y, cam.MidPosPostMovement.y, cam.smoothingSpeed);*/
	
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool j1Render::Load(pugi::xml_node& data)
{
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void j1Render::SetViewPort(const SDL_Rect& collider)
{
	SDL_RenderSetViewport(renderer, &collider);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

iPoint j1Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = App->win->GetScale();

	ret.x = (x - camera.x / scale);
	ret.y = (y - camera.y / scale);

	return ret;
}

// Blit to screen
bool j1Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, bool flip, float speed, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_Rect rect;
	if (flip)
	{
		rect.x = (int)(camera.x * speed) + x * scale + 38; //Add players sprite width.
		rect.y = (int)(camera.y * speed) + y * scale;
	}
	else
	{
		rect.x = (int)(camera.x * speed) + x * scale;
		rect.y = (int)(camera.y * speed) + y * scale;
	}
	
	if(section != NULL)
	{
		if (flip)
		{
			rect.w = -section->w; //Sprite will be printed from top right to left down instead of top left right down.
			rect.h = section->h;
		}
		else
		{
			rect.w = section->w;
			rect.h = section->h;
		}
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawQuad(const SDL_Rect& collider, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(collider);
	if(use_camera)
	{
		rec.x = (int)(camera.x + collider.x * scale);
		rec.y = (int)(camera.y + collider.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}
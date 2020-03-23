#include <math.h>
#include "p2Log.h"
#include "Application.h"
#include "FadeScene.h"
#include "Render.h"
#include "Gui.h"
#include "Scene.h"
#include "Window.h"
#include "Audio.h"
#include "Map.h"
#include "Collisions.h"
#include "EntityManager.h"
#include "SDL\include\SDL_render.h"
#include "SDL\include\SDL_timer.h"


Fade_Scene::Fade_Scene()
{
}

Fade_Scene::~Fade_Scene()
{}

bool Fade_Scene::Awake(pugi::xml_node&)
{
	screen = { 0, 0, App->win->width*App->win->scale, App->win->height*App->win->scale };
	return true;
}

bool Fade_Scene::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

bool Fade_Scene::Update(float dt)
{
	if (current_step == fade_step::none)
	{
		return true;
	}

	uint now = SDL_GetTicks() - start_time;
	float normalized = 1.0f < ((float)now / (float)total_time) ? 1.0f : ((float)now / (float)total_time);


	switch (current_step)
	{
		case fade_step::fade_to_black:
		{
			if (now >= total_time)											//Point where the screen is totally black, and the new map is loaded.
			{
				App->map->ChangeMap(mapName);								//Here the map is changed
				
				total_time += total_time;
				start_time = SDL_GetTicks();
				fading = false;
				current_step = fade_step::fade_from_black;
			}
		}
		break;

		case fade_step::fade_from_black:
		{
			normalized = 1.0f - normalized;

			if (now >= total_time)
			{
				current_step = fade_step::none;
			}

		}
		break;
	}

	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

bool Fade_Scene::FadeToBlack2(Module* module_off, Module* module_on, float time)
{
	bool ret = false;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		to_enable = module_on;
		to_disable = module_off;
		ret = true;
	}

	return ret;
}

bool Fade_Scene ::FadeToBlack(const char* mapname, float time)
{
	bool ret = false;

	mapName = mapname;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;
	}

	//App->audio->PlayMusic(App->map->data.music_File.GetString());

	return ret;
}

bool Fade_Scene::IsChanging() const
{
	return current_step != fade_step::none;
}
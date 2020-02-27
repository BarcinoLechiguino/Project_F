#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "SDL/include/SDL_rect.h"
#include "p2Log.h"
#define MAX_FRAMES 60

class Animation
{
public:
	bool loop = true;
	float speed = 1.0f;
	SDL_Rect frames[MAX_FRAMES];

private:
	float current_frame;
	int last_frame = 0;
	int loops = 0;

	pugi::xml_document animations_file;

public:

	void PushBack(const SDL_Rect& collider)
	{
		frames[last_frame++] = collider;
	}

	void LoadAnimation(p2SString entity, p2SString animationName)
	{
		pugi::xml_node animations;
		pugi::xml_parse_result animations_config = animations_file.load_file("animations.xml");										//Loads the animation_data.xml file.

		if (animations_config != NULL)																								//If the animation_data.xml could be loaded.
		{
			animations = animations_file.child("animations").child(entity.GetString()).child(animationName.GetString());								//Get the set of animations' name (passed as argument).
			
			speed = animations.attribute("speed").as_float();																		//Loads the animations set's speed from the xml file.
			loop = animations.attribute("loop").as_bool();																			//Loads the loop bool state from the xml file.

			for (pugi::xml_node pushback = animations.child("pushback"); pushback; pushback = pushback.next_sibling("pushback"))	//Loop that runs for however many pushbacks the animation has.
			{
				PushBack({ pushback.attribute("x").as_int(), pushback.attribute("y").as_int(), pushback.attribute("w").as_int(), pushback.attribute("h").as_int() });	//Loads a pushback from the file.													//Return the pushback of the animation (position of the rect that gets the sprite from the spritesheet)
			}
		}
		else
		{
			LOG("Could not load animations from the animations. pugi error: %s", animations_config.description());
			return;
		}

	}

	SDL_Rect& GetCurrentFrame(float dt)
	{
		current_frame += speed * dt;
		if (current_frame >= last_frame)
		{
			current_frame = (loop) ? 0.0f : last_frame - 1;
			loops++;
		}

		return frames[(int)current_frame];
	}

	bool Finished() const
	{
		return loops > 0;
	}

	void Reset()
	{
		current_frame = 0;
		loops = 0;
	}
};

#endif

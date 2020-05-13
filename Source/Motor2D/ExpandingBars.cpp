#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "SceneManager.h"
#include "TransitionManager.h"
#include "Color.h"
#include "Bar.h"

#include "ExpandingBars.h"


ExpandingBars::ExpandingBars(SCENES next_scene, float step_duration, bool non_lerp, int bar_number, bool vertical, bool random_colours, Color even_colour, Color odd_colour) 
	: Transition(next_scene, step_duration, non_lerp)
	, bar_number(bar_number)
	, random_colours(random_colours)
	, even_colour(even_colour)
	, odd_colour(odd_colour)
	, vertical(vertical)
{
	InitExpandingBars();
}

ExpandingBars::~ExpandingBars()
{

}

void ExpandingBars::StepTransition()
{
	switch (step)
	{
	case TRANSITION_STEP::ENTERING:

		Entering();

		break;

	case TRANSITION_STEP::CHANGING:

		Changing();

		break;

	case TRANSITION_STEP::EXITING:

		Exiting();

		break;
	}

	ExpandBars();
}

void ExpandingBars::Entering()
{
	current_cutoff += GetCutoffRate(step_duration);

	if (current_cutoff >= MAX_CUTOFF)
	{
		current_cutoff = MAX_CUTOFF;
		
		step = TRANSITION_STEP::CHANGING;
	}
}

void ExpandingBars::Changing()
{
	App->scene_manager->SwitchScene(next_scene);

	step = TRANSITION_STEP::EXITING;
}

void ExpandingBars::Exiting()
{
	current_cutoff -= GetCutoffRate(step_duration);
	
	if (current_cutoff <= MIN_CUTOFF)
	{
		step = TRANSITION_STEP::NONE;

		bars.clear();

		App->transition_manager->DeleteActiveTransition();
	}
}

void ExpandingBars::ExpandBars()
{
	if (!vertical)
	{
		ExpandHorizontalBars();
	}
	else
	{
		ExpandVerticalBars();
	}

	DrawBars();
}

void ExpandingBars::ExpandHorizontalBars()
{
	if (step == TRANSITION_STEP::ENTERING)
	{
		if (!non_lerp)
		{
			for (int i = 0; i < (int)bars.size(); ++i)												// cutoff goes from 0 to 1 to 0, so there would be no need to
			{																					// separate expansion and reduction in different steps.
				bars[i].bar.x = (int)Lerp(screen_center.x, 0.f, current_cutoff);
				bars[i].bar.w = (int)Lerp(0.f, (float)win_width, current_cutoff);
			}
		}
		else
		{
			for (int i = 0; i < (int)bars.size(); ++i)												// cutoff goes from 0 to 1 to 0, so there would be no need to
			{																					// separate expansion and reduction in different steps.
				if (i % 2 == 0)																	// Ex: 0 --> win_width --> 0.
				{
					bars[i].bar.x = (int)N_Lerp(screen_center.x, 0.f, current_cutoff);
					bars[i].bar.w = (int)N_Lerp(0.f, (float)win_width, current_cutoff);
				}
				else
				{
					bars[i].bar.x = (int)N_Lerp(screen_center.x, 0.f, current_cutoff, true);
					bars[i].bar.w = (int)N_Lerp(0.f, (float)win_width, current_cutoff, true);
				}
			}
		}
	}

	if (step == TRANSITION_STEP::EXITING)
	{
		for (int i = 0; i < (int)bars.size(); ++i)												// As in the exiting step all bars will be at the same position/size, we need Lerp().
		{																					// By using Lerp(), non_lerp and even/odd bar distinction is not needed.
			bars[i].bar.x = (int)Lerp(screen_center.x, 0.f, current_cutoff);
			bars[i].bar.w = (int)Lerp(0.f, (float)win_width, current_cutoff);
		}
	}
}

void ExpandingBars::ExpandVerticalBars()
{
	if (step == TRANSITION_STEP::ENTERING)
	{
		if (!non_lerp)
		{
			for (int i = 0; i < (int)bars.size(); ++i)
			{
				bars[i].bar.y = (int)Lerp(screen_center.y, 0.f, current_cutoff);
				bars[i].bar.h = (int)Lerp(0.f, (float)win_height, current_cutoff);
			}
		}
		else
		{
			for (int i = 0; i < (int)bars.size(); ++i)												// cutoff goes from 0 to 1 to 0, so there would be no need to
			{																					// separate expansion and reduction in different steps.
				if (i % 2 == 0)																	// Ex: 0 --> win_width --> 0.
				{
					bars[i].bar.y = (int)N_Lerp(screen_center.y, 0.f, current_cutoff);
					bars[i].bar.h = (int)N_Lerp(0.f, (float)win_height, current_cutoff);
				}
				else
				{
					bars[i].bar.y = (int)N_Lerp(screen_center.y, 0.f, current_cutoff, true);
					bars[i].bar.h = (int)N_Lerp(0.f, (float)win_height, current_cutoff, true);
				}
			}
		}
	}

	if (step == TRANSITION_STEP::EXITING)
	{
		for (int i = 0; i < (int)bars.size(); ++i)													// cutoff goes from 0 to 1 to 0, so there would be no need to									
		{																						// separate expansion and reduction in different steps.
			bars[i].bar.y = (int)Lerp(screen_center.y, 0.f, current_cutoff);							// Ex: 0 --> win_height --> 0.
			bars[i].bar.h = (int)Lerp(0.f, (float)win_height, current_cutoff);
		}
	}
}

void ExpandingBars::DrawBars()
{
	for (int i = 0; i < (int)bars.size(); ++i)
	{
		SDL_SetRenderDrawColor(App->render->renderer, (Uint8)bars[i].colour.r, (Uint8)bars[i].colour.g, (Uint8)bars[i].colour.b, 255);
		SDL_RenderFillRect(App->render->renderer, &bars[i].bar);
	}
}

void ExpandingBars::InitExpandingBars()
{
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	
	App->win->GetWindowSize(win_width, win_height);

	screen_center = { win_width * 0.5f, win_height * 0.5f };

	for (int i = 0; i < bar_number; ++i)
	{
		Bar new_bar;

		if (!vertical)
		{
			AssignHorizontalBar(new_bar, win_width, win_height, i);
		}
		else
		{
			AssignVerticalBar(new_bar, win_width, win_height, i);
		}
		
		AssignBarColour(new_bar, i);

		bars.push_back(new_bar);
	}

	step = TRANSITION_STEP::ENTERING;
}

void ExpandingBars::AssignHorizontalBar(Bar& new_bar, const int& win_width, const int& win_height, const int& index)
{
	int bar_width = win_width;

	float bar_num = (float)bar_number;																		// As bar_number is an integer, when win_height is divided by it
	float height_ratio = win_height / bar_num;														// the decimals are cut off. By creating 2 float buffers and using
	int bar_height = (int)ceil(height_ratio);															// the ceil() function, an approximation to the real number is made.

	new_bar.bar = { (int)screen_center.x, bar_height * index, 0, bar_height };
}

void ExpandingBars::AssignVerticalBar(Bar& new_bar, const int& win_width, const int& win_height, const int& index)
{
	int bar_height = win_height;
	
	float bar_num = (float)bar_number;																		// As bar_number is an integer, when win_width is divided by it
	float width_ratio = win_width / bar_num;														// the decimals are cut off. By creating 2 float buffers and using
	int bar_width = (int)ceil(width_ratio);																// the ceil() function, an approximation is made to the real number.

	new_bar.bar = { bar_width * index, (int)screen_center.y, bar_width, 0 };
}

void ExpandingBars::AssignBarColour(Bar& new_bar, const int& index)
{
	if (random_colours)
	{
		new_bar.colour = GetRandomColour();
	}
	else
	{
		if (index % 2 == 0)
		{
			new_bar.colour = even_colour;
		}
		else
		{
			new_bar.colour = odd_colour;
		}
	}
}
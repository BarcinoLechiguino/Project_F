#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "SceneManager.h"
#include "TransitionManager.h"
#include "Color.h"

#include "Slide.h"


Slide::Slide(SCENES next_scene, float step_duration, bool non_lerp, bool vertical, bool slide_from_right, bool slide_from_bottom, Color slide_colour)
	: Transition(next_scene, step_duration, non_lerp)
	, vertical(vertical)
	, slide_from_right(slide_from_right)
	, slide_from_bottom(slide_from_bottom)
	, slide_colour(slide_colour)
{
	InitSlide();
}

Slide::~Slide()
{

}

void Slide::StepTransition()
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

	TranslateSlide();
}

void Slide::Entering()
{
	current_cutoff += GetCutoffRate(step_duration);

	if (current_cutoff >= MAX_CUTOFF)
	{
		current_cutoff = MAX_CUTOFF;

		step = TRANSITION_STEP::CHANGING;
	}
}

void Slide::Changing()
{
	App->scene_manager->SwitchScene(next_scene);

	step = TRANSITION_STEP::EXITING;
}

void Slide::Exiting()
{
	current_cutoff -= GetCutoffRate(step_duration);

	if (current_cutoff <= MIN_CUTOFF)
	{
		current_cutoff = MIN_CUTOFF;
		
		step = TRANSITION_STEP::NONE;

		App->transition_manager->DeleteActiveTransition();
	}
}

void Slide::TranslateSlide()
{
	if (!vertical)
	{
		HorizontalSlide();
	}
	else
	{
		VerticalSlide();
	}

	DrawSlide();
}

void Slide::HorizontalSlide()
{
	if (!slide_from_right)
	{
		if (!non_lerp)
		{
			screen.x = (int)Lerp((float)-screen.w, 0.f, current_cutoff);									// Horizontal linearly interpolated slide coming from the left.
		}
		else
		{
			screen.x = (int)N_Lerp((float)-screen.w, 0.f, current_cutoff);								// Horizontal non-linearly interpolated slide coming from the left.
		}
	}
	else
	{
		if (!non_lerp)
		{
			screen.x = (int)Lerp((float)screen.w, 0.f, current_cutoff);									// Horizontal linearly interpolated slide coming from the right.
		}
		else
		{
			screen.x = (int)N_Lerp((float)screen.w, 0.f, current_cutoff);									// Horizontal non-linearly interpolated slide coming from the right.
		}
	}
}

void Slide::VerticalSlide()
{
	if (!slide_from_bottom)
	{
		if (!non_lerp)
		{
			screen.y = (int)Lerp((float)-screen.h, 0.f, current_cutoff);									// Vertical linearly interpolated slide coming from the top.
		}
		else
		{
			screen.y = (int)N_Lerp((float)-screen.h, 0.f, current_cutoff);								// Vertical non-linearly interpolated slide coming from the top.
		}
	}
	else
	{
		if (!non_lerp)
		{
			screen.y = (int)Lerp((float)screen.h, 0.f, current_cutoff);									// Vertical linearly interpolated slide coming from the bottom.
		}
		else
		{
			screen.y = (int)N_Lerp((float)screen.h, 0.f, current_cutoff);									// Vertical non-linearly interpolated slide coming from the bottom.
		}
	}
}

void Slide::DrawSlide()
{
	SDL_SetRenderDrawColor(App->render->renderer, (Uint8)slide_colour.r, (Uint8)slide_colour.g, (Uint8)slide_colour.b, (Uint8)255);
	SDL_RenderFillRect(App->render->renderer, &screen);
}

void Slide::InitSlide()
{
	App->win->GetWindowRect(screen);

	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);

	step = TRANSITION_STEP::ENTERING;
}
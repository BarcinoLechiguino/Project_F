#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "SceneManager.h"
#include "TransitionManager.h"
#include "Color.h"

#include "FadeToColour.h"


FadeToColour::FadeToColour(SCENES next_scene, float step_duration, Color fade_colour) : Transition(next_scene, step_duration)
, fade_colour(fade_colour)
{	
	InitFadeToColour();
}

FadeToColour::~FadeToColour()
{

}

void FadeToColour::StepTransition()
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

	ApplyFade();
}

void FadeToColour::Entering()
{
	current_cutoff += GetCutoffRate(step_duration);

	if (current_cutoff >= MAX_CUTOFF)
	{
		current_cutoff = MAX_CUTOFF;

		step = TRANSITION_STEP::CHANGING;
	}
}

void FadeToColour::Changing()
{
	App->scene_manager->SwitchScene(next_scene);

	step = TRANSITION_STEP::EXITING;
}

void FadeToColour::Exiting()
{
	current_cutoff -= GetCutoffRate(step_duration);

	if (current_cutoff <= MIN_CUTOFF)
	{
		current_cutoff = MIN_CUTOFF;

		step = TRANSITION_STEP::NONE;

		App->transition_manager->DeleteActiveTransition();
	}
}

void FadeToColour::ApplyFade()
{
	SDL_SetRenderDrawColor(App->render->renderer, (Uint8)fade_colour.r, (Uint8)fade_colour.g, (Uint8)fade_colour.b, (Uint8)(current_cutoff * 255.0f));

	SDL_RenderFillRect(App->render->renderer, &screen);
}

void FadeToColour::InitFadeToColour()
{
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	
	App->win->GetWindowRect(screen);

	step = TRANSITION_STEP::ENTERING;
}
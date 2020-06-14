#include "Color.h"

#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "SceneManager.h"
#include "TransitionManager.h"

#include "OldMonitor.h"

OldMonitor::OldMonitor(SCENES next_scene, float step_duration, bool non_lerp, Color rect_colour) : Transition(next_scene, step_duration, non_lerp) , rect_colour(rect_colour)
{
	InitOldMonitor();
}

OldMonitor::~OldMonitor()
{

}

void OldMonitor::StepTransition()
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

	TranslateMonitor();
}

void OldMonitor::Entering()
{
	current_cutoff += GetCutoffRate(step_duration);

	if (current_cutoff >= MAX_CUTOFF)
	{
		current_cutoff = MAX_CUTOFF;

		step = TRANSITION_STEP::CHANGING;
	}
}

void OldMonitor::Changing()
{
	App->scene_manager->SwitchScene(next_scene);

	step = TRANSITION_STEP::EXITING;
}

void OldMonitor::Exiting()
{
	current_cutoff -= GetCutoffRate(step_duration);

	if (current_cutoff <= MIN_CUTOFF)
	{
		current_cutoff = MIN_CUTOFF;

		step = TRANSITION_STEP::NONE;

		App->transition_manager->DeleteActiveTransition();
	}
}

void OldMonitor::TranslateMonitor()
{
	if (!non_lerp)
	{
		top_rect.y		= (int)Lerp((float)-top_rect.h, 0.f, current_cutoff);
		bottom_rect.y	= (int)Lerp((float)window_height, (window_height * 0.5f), current_cutoff);
		left_rect.x		= (int)Lerp((float)-left_rect.w, 0.f, current_cutoff);
		right_rect.x	= (int)Lerp((float)window_width, (window_width * 0.5f), current_cutoff);
	}
	else
	{
		/*top_rect.y		= (int)N_Lerp((float)-top_rect.h, 0.f, current_cutoff);
		bottom_rect.y	= (int)N_Lerp((float)window_height, (window_height * 0.5f), current_cutoff);
		left_rect.x		= (int)Lerp((float)-left_rect.w, 0.f, current_cutoff);
		right_rect.x	= (int)Lerp((float)window_width, (window_width * 0.5f), current_cutoff);*/

		/*top_rect.y		= (int)N_Lerp((float)-top_rect.h, 0.f, current_cutoff);
		bottom_rect.y	= (int)N_Lerp((float)window_height, (window_height * 0.5f), current_cutoff);
		left_rect.x		= (int)N_Lerp((float)-left_rect.w, 0.f, current_cutoff);
		right_rect.x	= (int)N_Lerp((float)window_width, (window_width * 0.5f), current_cutoff);*/
		
		top_rect.y		= (int)N_Lerp((float)-top_rect.h, 0.f, current_cutoff);
		bottom_rect.y	= (int)N_Lerp((float)window_height, (window_height * 0.5f), current_cutoff);
		left_rect.x		= (int)N_Lerp((float)-left_rect.w, 0.f, current_cutoff, true);
		right_rect.x	= (int)N_Lerp((float)window_width, (window_width * 0.5f), current_cutoff, true);
	}

	DrawMonitor();
}

void OldMonitor::DrawMonitor()
{
	SDL_SetRenderDrawColor(App->render->renderer, (Uint8)rect_colour.r, (Uint8)rect_colour.g, (Uint8)rect_colour.b, (Uint8)rect_colour.a);

	SDL_RenderFillRect(App->render->renderer, &top_rect);
	SDL_RenderFillRect(App->render->renderer, &bottom_rect);
	SDL_RenderFillRect(App->render->renderer, &left_rect);
	SDL_RenderFillRect(App->render->renderer, &right_rect);
}

void OldMonitor::InitOldMonitor()
{
	App->win->GetWindowSize(window_width, window_height);
	
	App->win->GetWindowRect(top_rect);
	App->win->GetWindowRect(bottom_rect);
	App->win->GetWindowRect(left_rect);
	App->win->GetWindowRect(right_rect);

	top_rect.h *= 0.5f;
	bottom_rect.h *= 0.5f;
	left_rect.w *= 0.5f;
	right_rect.w *= 0.5f;
	
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);

	step = TRANSITION_STEP::ENTERING;
}
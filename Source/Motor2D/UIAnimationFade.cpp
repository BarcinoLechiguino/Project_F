#include "Application.h"
#include "Render.h"
#include "GuiManager.h"

#include "UI.h"

#include "UIAnimationFade.h"

UIAnimationFade::UIAnimationFade(UI* element, float animation_duration, bool hide_on_completion, float start_alpha, float end_alpha) 
	: UIAnimation(element, UI_ANIMATION_TYPE::FADE,  animation_duration, hide_on_completion)
	, start_alpha(start_alpha)
	, end_alpha(end_alpha)
{
	InitFade();
}

UIAnimationFade::~UIAnimationFade()
{

}

void UIAnimationFade::Start()
{

}

void UIAnimationFade::StepAnimation()
{
	current_animation_step_rate += GetAnimationStepRate(animation_duration);
	
	if (current_animation_step_rate > MAX_RATE)
	{
		to_delete = true;
		element->is_transitioning = false;

		if (hide_on_completion)
		{
			App->gui_manager->SetElementsVisibility(element, false);
		}
	}
	else
	{
		ApplyFade();
	}
}

void UIAnimationFade::CleanUp()
{
	return;
}

void UIAnimationFade::FinishAnimation()
{

}

void UIAnimationFade::ApplyFade()
{

}

void UIAnimationFade::InitFade()
{
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	
	if (start_alpha > end_alpha)
	{
		fading_to_zero_alpha = true;
		fading_to_max_alpha = false;
	}
	else
	{
		fading_to_zero_alpha = false;
		fading_to_max_alpha = true;
	}

	element->is_transitioning = true;
}

void UIAnimationFade::GetTexturesToFade()
{
	std::vector<UI*> childs = App->gui_manager->GetElementChilds(element);

	//textures_to_fade.push_back()
}
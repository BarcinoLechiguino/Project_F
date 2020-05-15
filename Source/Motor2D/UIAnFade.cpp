#include "Application.h"

#include "UIAnFade.h"

UIAnFade::UIAnFade(float animation_duration, float start_alpha, float end_alpha, std::vector<SDL_Texture*> textures_to_fade) 
	: UIAnimation(animation_duration)
	, start_alpha(start_alpha)
	, end_alpha(end_alpha)
{
	InitFade();
}

UIAnFade::~UIAnFade()
{

}

void UIAnFade::Start()
{

}

void UIAnFade::StepAnimation()
{
	current_animation_step_rate += GetAnimationStepRate(animation_duration);
	
	if (current_animation_step_rate > MAX_RATE)
	{
		to_delete = true;
	}
	else
	{
		ApplyFade();
	}
}

void UIAnFade::CleanUp()
{

}

void UIAnFade::ApplyFade()
{

}

void UIAnFade::InitFade()
{
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
}
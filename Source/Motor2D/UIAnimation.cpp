#include "Application.h"

#include "UIAnimation.h"

UIAnimation::UIAnimation(float animation_duration) : animation_duration(animation_duration), animation_step_rate(0.0f), current_animation_step_rate(0.0f), to_delete(false)
{

}

UIAnimation::~UIAnimation()
{

}

void UIAnimation::Start()
{
	return;
}

void UIAnimation::StepAnimation()
{
	return;
}

void UIAnimation::CleanUp()
{
	return;
}

float UIAnimation::Lerp(float start, float end, float rate)
{
	float increment = (end - start) * rate;

	return start + increment;
}

float UIAnimation::N_Lerp(float start, float end, float rate)
{
	float r = 1 - ((1 - rate) * (1 - rate));
	float increment = (end - start) * r;

	return start + increment;
}

float UIAnimation::GetAnimationStepRate(float animation_duration)
{
	animation_step_rate = App->GetDt() / animation_duration;

	return animation_step_rate;
}
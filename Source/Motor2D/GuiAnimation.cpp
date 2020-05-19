#include "Application.h"

#include "GuiAnimation.h"

GuiAnimation::GuiAnimation(GuiElement* element, GUI_ANIMATION_TYPE type, float animation_duration, bool hide_on_completion) 
	: element(element)
	, type(type)
	, animation_duration(animation_duration)
	, hide_on_completion(hide_on_completion)
	, animation_step_rate(0.0f)
	, current_animation_step_rate(0.0f)
	, to_delete(false)
{

}

GuiAnimation::~GuiAnimation()
{

}

void GuiAnimation::Start()
{
	return;
}

void GuiAnimation::StepAnimation()
{
	return;
}

void GuiAnimation::CleanUp()
{
	return;
}

void GuiAnimation::FinishAnimation()
{
	return;
}

float GuiAnimation::Lerp(float start, float end, float rate)
{
	float increment = (end - start) * rate;

	return start + increment;
}

float GuiAnimation::N_Lerp(float start, float end, float rate)
{
	float r = 1 - ((1 - rate) * (1 - rate));
	float increment = (end - start) * r;

	return start + increment;
}

float GuiAnimation::GetAnimationStepRate(float animation_duration)
{
	animation_step_rate = App->GetUnpausableDt() / animation_duration;

	return animation_step_rate;
}
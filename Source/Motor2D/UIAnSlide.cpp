#include "Animation.h"

#include "UIAnSlide.h"

UIAnSlide::UIAnSlide(float animation_step, iPoint initial_position, iPoint final_position, bool horizontal_slide, bool vertical_slide) 
	: UIAnimation(animation_step)
	, initial_position(0, 0)
	, final_position(0, 0)
	, horizontal_slide(false)
	, vertical_slide(false)
{
	InitSlide();
}

UIAnSlide::~UIAnSlide()
{

}

void UIAnSlide::Start()
{

}

void UIAnSlide::StepAnimation()
{
	current_animation_step_rate += GetAnimationStepRate(animation_duration);

	if (current_animation_step_rate > MAX_RATE)
	{
		to_delete = true;
	}
	else
	{
		TranslateSlide();
	}
}

void UIAnSlide::CleanUp()
{

}

void UIAnSlide::TranslateSlide()
{

}

void UIAnSlide::InitSlide()
{

}
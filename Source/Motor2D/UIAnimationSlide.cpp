#include "Application.h"
#include "GuiManager.h"
#include "UI.h"

#include "UIAnimationSlide.h"

UIAnimationSlide::UIAnimationSlide(UI* element, float animation_duration, bool hide_on_completion, iPoint initial_position, iPoint final_position) 
	: UIAnimation(element, animation_duration, hide_on_completion)
	, initial_position(initial_position)
	, final_position(final_position)
{
	InitSlide();
}

UIAnimationSlide::~UIAnimationSlide()
{

}

void UIAnimationSlide::Start()
{

}

void UIAnimationSlide::StepAnimation()
{
	current_animation_step_rate += GetAnimationStepRate(animation_duration);

	if (current_animation_step_rate > MAX_RATE)
	{
		to_delete = true;

		for (int i = 0; i < elements_to_slide.size(); ++i)
		{	
			elements_to_slide[i]->is_transitioning = false;

			if (hide_on_completion)
			{
				elements_to_slide[i]->is_visible = false;
			}
		}

		elements_to_slide.clear();
	}
	else
	{
		TranslateSlide();
	}
}

void UIAnimationSlide::CleanUp()
{

}

void UIAnimationSlide::TranslateSlide()
{
	int x_displacement = (int)N_Lerp((float)initial_position.x, (float)final_position.x, current_animation_step_rate);
	int y_displacement = (int)N_Lerp((float)initial_position.y, (float)final_position.y, current_animation_step_rate);
	
	iPoint new_position = { x_displacement, y_displacement };

	
	
	for (int i = 0; i < elements_to_slide.size(); ++i)
	{	
		elements_to_slide[i]->SetElementPosition(new_position);
	}
}

void UIAnimationSlide::InitSlide()
{	
	elements_to_slide = App->gui_manager->GetElementChilds(element);

	elements_to_slide.push_back(element);

	for (int i = 0; i < elements_to_slide.size(); ++i)
	{
		elements_to_slide[i]->is_transitioning = true;
	}
}


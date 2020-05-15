#include "Application.h"
#include "GuiManager.h"
#include "UI.h"

#include "UIAnimationSlide.h"

UIAnimationSlide::UIAnimationSlide(UI* element, float animation_duration, bool hide_on_completion, iPoint initial_position, iPoint final_position) 
	: UIAnimation(element, UI_ANIMATION_TYPE::SLIDE, animation_duration, hide_on_completion)
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

		FinishAnimation();

		elements_to_slide.clear();
	}
	else
	{
		TranslateSlide();
	}
}

void UIAnimationSlide::CleanUp()
{
	if (element->is_transitioning)															// In case the animation was cancelled halfway through.
	{
		FinishAnimation();
	}
}

void UIAnimationSlide::FinishAnimation()
{
	element->SetElementPosition(final_position);
	element->is_transitioning = false;

	if (hide_on_completion)
	{
		element->is_visible = false;
	}

	for (int i = 0; i < elements_to_slide.size(); ++i)
	{
		iPoint local_position = elements_to_slide[i]->GetLocalPos();

		elements_to_slide[i]->SetElementPosition(final_position + local_position);
		elements_to_slide[i]->is_transitioning = false;

		if (hide_on_completion)
		{
			elements_to_slide[i]->is_visible = false;
		}
	}
}

void UIAnimationSlide::TranslateSlide()
{
	int x_displacement = (int)N_Lerp((float)initial_position.x, (float)final_position.x, current_animation_step_rate);
	int y_displacement = (int)N_Lerp((float)initial_position.y, (float)final_position.y, current_animation_step_rate);
	
	iPoint new_position = { x_displacement, y_displacement };

	element->SetElementPosition(new_position);
	
	for (int i = 0; i < elements_to_slide.size(); ++i)
	{	
		iPoint local_position = elements_to_slide[i]->GetLocalPos();
		
		elements_to_slide[i]->SetElementPosition(new_position + local_position);
	}
}

void UIAnimationSlide::InitSlide()
{	
	elements_to_slide = App->gui_manager->GetElementChilds(element);

	//elements_to_slide.push_back(element);											// The parent element will be handled independently from it's childs.

	element->is_transitioning = true;

	for (int i = 0; i < elements_to_slide.size(); ++i)
	{
		elements_to_slide[i]->is_transitioning = true;
	}
}


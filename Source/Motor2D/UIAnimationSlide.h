#ifndef __UI_ANIMATION_SLIDE_H__
#define __UI_ANIMATION_SLIDE_H__

#include "UIAnimation.h"
#include "Point.h"

class UIAnimationSlide : public UIAnimation
{
public:
	UIAnimationSlide(UI* element, float animation_duration, bool hide_on_completion, iPoint initial_position, iPoint final_position);
	~UIAnimationSlide();

	void Start();
	void StepAnimation();
	void CleanUp();

public:
	void FinishAnimation();
	
	void TranslateSlide();

	void InitSlide();

private:
	iPoint initial_position;
	iPoint final_position;

	std::vector<UI*> elements_to_slide;
};

#endif // !__UI_ANIMATION_SLIDE_H__
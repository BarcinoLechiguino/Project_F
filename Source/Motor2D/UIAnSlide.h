#ifndef __UI_AN_SLIDE_H__
#define __UI_AN_SLIDE_H__

#include "UIAnimation.h"
#include "Point.h"

class UIAnSlide : public UIAnimation
{
public:
	UIAnSlide(float animation_duration, iPoint initial_position, iPoint final_position, bool horizontal_slide, bool vertical_slide);
	~UIAnSlide();

	void Start();
	void StepAnimation();
	void CleanUp();

public:
	void TranslateSlide();

	void InitSlide();

private:
	iPoint initial_position;
	iPoint final_position;

	bool horizontal_slide;
	bool vertical_slide;
};

#endif // !__UI_AN_SLIDE_H__
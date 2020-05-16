#ifndef __UI_ANIMATION_H__
#define __UI_ANIMATION_H__

#define MAX_RATE 1.0f
#define MIN_RATE 0.0f

class UI;

enum class UI_ANIMATION_TYPE									// Will be managed internally. There are no empty animations, so type will always be the same for each animation.
{
	FADE,
	SLIDE
};

class UIAnimation
{
public:
	UIAnimation(UI* element, UI_ANIMATION_TYPE type, float animation_duration, bool hide_on_completion);
	virtual ~UIAnimation();

	virtual void Start();
	virtual void StepAnimation();
	virtual void CleanUp();

public:
	virtual void FinishAnimation();										// Will end the animation by making the animated element reach the goal state (end_alpha, final_position...)
	
	float Lerp(float start, float end, float rate);
	float N_Lerp(float start, float end, float rate);

	float GetAnimationStepRate(float animation_duration);

public:
	UI*		element;

	UI_ANIMATION_TYPE type;

	float	animation_duration;
	float	current_animation_step_rate;

	bool	to_delete;
	bool	hide_on_completion;

private:
	float	animation_step_rate;
};

#endif // !__UI_ANIMATION_H__
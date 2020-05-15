#ifndef __UI_ANIMATION_FADE_H__
#define __UI_ANIMATION_FADE_H__

#include "UIAnimation.h"

class UIAnimationFade : public UIAnimation
{
public:
	UIAnimationFade(UI* element, float animation_duration, bool hide_on_completion, float start_alpha, float end_alpha);
	~UIAnimationFade();

	void Start();
	void StepAnimation();
	void CleanUp();

public:
	void ApplyFade();
	
	void GetTexturesToFade();

	void InitFade();

private:
	float start_alpha;
	float end_alpha;

	bool fading_to_max_alpha;
	bool fading_to_zero_alpha;

	std::vector<SDL_Texture*> textures_to_fade;
};

#endif // !__UI_ANIMATION_FADE_H__
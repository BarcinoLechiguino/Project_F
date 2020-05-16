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
	void FinishAnimation();
	
	void ApplyFade();
	
	void GetAndInitializeTexturesToFade();

	void InitFade();

private:
	float start_alpha;
	float end_alpha;

	bool fade_from_higher_to_lower;

	SDL_Texture* element_texture;

	std::vector<UI*> child_elements;
	std::vector<SDL_Texture*> textures_to_fade;
};

#endif // !__UI_ANIMATION_FADE_H__
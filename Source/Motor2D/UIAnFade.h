#ifndef __UI_AN_FADE_H__
#define __UI_AN_FADE_H__

#include "UIAnimation.h"

class UIAnFade : public UIAnimation
{
public:
	UIAnFade(float animation_duration, float start_alpha, float end_alpha, std::vector<SDL_Texture*> textures_to_fade);
	~UIAnFade();

	void Start();
	void StepAnimation();
	void CleanUp();

public:
	void ApplyFade();
	
	void InitFade();

private:
	float start_alpha;
	float end_alpha;

	bool fading_to_max_alpha;
	bool fading_to_zero_alpha;

	std::vector<SDL_Texture*> textures_to_fade;
};

#endif // !__UI_AN_FADE_H__
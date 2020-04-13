#ifndef __FADE_TO_COLOUR_H__
#define __FADE_TO_COLOUR_H__

#include "Transition.h"

struct Color;

class FadeToColour : public Transition
{
public:
	FadeToColour(SCENES next_scene, float step_duration, Color colour);
	~FadeToColour();

	void StepTransition();

public:
	void Entering();														// Method that will be used to execute the ENTERING transition step.
	void Changing();														// Method that will be used to execute the CHANGING transition step.
	void Exiting();															// Method that will be used to execute the EXITING transition step.
	
	void ApplyFade();														// Method that will be used to update the alpha value of the fade.

	void InitFadeToColour();												// Method that will be used to initialize all the variables related with the FadeToColour.

private:
	SDL_Rect	screen;														// Rect that will have the size parameters of the screen.
	Color		fade_colour;												// The colour that will be assigned to the FadeToColour transition.
};

#endif // !__FADE_TO_COLOUR_H__
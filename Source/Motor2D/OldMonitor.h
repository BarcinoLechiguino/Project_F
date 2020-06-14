#ifndef __OLD_MONITOR_H__
#define __OLD_MONITOR_H__

#include "Transition.h"

struct Color;

class OldMonitor : public Transition
{
public:
	OldMonitor(SCENES next_scene, float step_duration, bool non_lerp, Color rect_colour);
	~OldMonitor();

	void StepTransition();

public:
	void Entering();
	void Changing();
	void Exiting();

	void TranslateMonitor();
	void DrawMonitor();

	void InitOldMonitor();

private:
	int window_width;
	int window_height;
	
	SDL_Rect top_rect;
	SDL_Rect bottom_rect;
	SDL_Rect left_rect;
	SDL_Rect right_rect;

	Color rect_colour;
};

#endif // !__OLD_MONITOR_H__
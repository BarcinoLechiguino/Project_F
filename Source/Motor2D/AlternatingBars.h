#ifndef __ALTERNATING_BARS_H__
#define __ALTERNAING_BARS_H__

#include "Transition.h"

struct Color;
struct Bar;

class AlternatingBars : public Transition
{
public:
	AlternatingBars(SCENES next_scene, float step_duration, bool non_lerp, int bar_number, bool vertical, bool random_colours, Color even_colour, Color odd_colour);
	~AlternatingBars();

	void StepTransition();

public:
	void Entering();
	void Changing();
	void Exiting();

	void AlternateBars();
	void TranslateHorizontalBars();
	void TranslateVerticalBars();
	void DrawBars();
	
	void InitAlternatingBars();
	void AssignHorizontalBar(Bar& new_bar, const int& win_width, const int& win_height, const int& index);
	void AssignVerticalBar(Bar& new_bar, const int& win_width, const int& win_height, const int& index);
	void AssignBarColour(Bar& new_bar, const int& index);

private:
	std::vector<Bar>	bars;
	
	bool				vertical;
	bool				random_colours;
	
	int					bar_number;
	int					win_width;
	int					win_height;

	Color				even_colour;
	Color				odd_colour;
};

#endif // !__ALTERNATE_BARS_H__


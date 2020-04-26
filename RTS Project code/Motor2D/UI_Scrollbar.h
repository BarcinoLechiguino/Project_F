#ifndef __UI_SCROLLBAR_H__
#define __UI_SCROLLBAR_H__

#include "UI.h"

class UI_Image;

class UI_Scrollbar : public UI
{
public:
	UI_Scrollbar(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, SDL_Rect thumbSize, iPoint thumbOffset, SDL_Rect drag_area, float drag_factor, bool drag_x_axis = false, bool drag_y_axis = true,
		bool inverted_scrolling = false, bool is_visible = true, bool is_interactible = false, bool is_draggable = false, Module* listener = nullptr, UI* parent = nullptr,
		SDL_Rect* scroll_mask = nullptr, iPoint maskOffset = iPoint(0, 0), bool emptyElements = false);

	bool Draw();

	void CheckInput();

	void CleanUp();

public:
	void DrawScrollbarElements();
	void LinkScroll(UI* element);
	void UpdateLinkedElements();
	bool LinkedElementsBeingHovered();
	bool MouseWithinDragArea();
	float GetDragFactor(UI* element);

	bool GetDragXAxis() const;
	bool GetDragYAxis() const;
	iPoint GetThumbLocalPos();
	SDL_Rect GetThumbHitbox();
	void SetThumbHitbox(SDL_Rect hitbox);

	void PlaceThumbOnMousePos();
	void CheckKeyboardInputs();
	void DragThumbWithMousewheel();
	
	bool ThumbIsWithinVerticalScrollbarBounds();
	bool ThumbIsAtUpperBound();
	bool ThumbIsAtLowerBound();
	bool ThumbIsWithinHorizontalScrollbarBounds();
	bool ThumbIsAtLeftBound();
	bool ThumbIsAtRightBound();
	void CheckScrollbarBounds();

private:
	UI_Image* bar;
	UI_Image* thumb;
	UI_Image* scroll_mask;
	
	int scrollbar_width;
	int scrollbar_height;

	SDL_Rect	drag_area;
	float		drag_factor;
	iPoint		drag_displacement;
	iPoint		mouse_wheel_scroll;
	bool		inverted_scrolling;
	float		arrow_position_factor;
	iPoint		new_thumb_position;

	std::vector<UI*> linked_elements;
};
#endif // !__UI_SCROLLBAR_H__
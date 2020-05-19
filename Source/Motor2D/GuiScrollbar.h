#ifndef __GUI_SCROLLBAR_H__
#define __GUI_SCROLLBAR_H__

#include "GuiElement.h"

class GuiImage;

class GuiScrollbar : public GuiElement
{
public:
	GuiScrollbar(GUI_ELEMENT_TYPE type, int x, int y, SDL_Rect hitbox, SDL_Rect thumbSize, iPoint thumbOffset, SDL_Rect drag_area, float drag_factor, bool drag_x_axis = false, bool drag_y_axis = true,
		bool inverted_scrolling = false, bool is_visible = true, bool is_interactible = false, bool is_draggable = false, Module* listener = nullptr, GuiElement* parent = nullptr,
		SDL_Rect* scroll_mask = nullptr, iPoint maskOffset = iPoint(0, 0), bool emptyElements = false);

	bool Draw();
	void CheckInput();
	void CleanUp();

	SDL_Texture* GetTexture() const;

public:
	void DrawScrollbarElements();
	void LinkScroll(GuiElement* element);
	void UpdateLinkedElements();
	bool LinkedElementsBeingHovered();
	bool MouseWithinDragArea();
	float GetDragFactor(GuiElement* element);

	bool GetDragXAxis() const;
	bool GetDragYAxis() const;
	iPoint GetThumbLocalPos();
	SDL_Rect GetThumbHitbox();
	void SetThumbHitbox(SDL_Rect hitbox);

	void PlaceThumbOnMousePos();
	void PlaceThumbOnCursorPos();											//TMP CONTROLLER
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
	GuiImage* bar;
	GuiImage* thumb;
	GuiImage* scroll_mask;
	
	int scrollbar_width;
	int scrollbar_height;

	SDL_Rect	drag_area;
	float		drag_factor;
	iPoint		drag_displacement;
	iPoint		mouse_wheel_scroll;
	bool		inverted_scrolling;
	float		arrow_position_factor;
	iPoint		new_thumb_position;

	std::vector<GuiElement*> linked_elements;
};
#endif // !__GUI_SCROLLBAR_H__
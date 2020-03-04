#ifndef __UI_BUTTON_H__
#define __UI_BUTTON_H__

#include "UI.h"

class UI_Button : public UI
{
public:
	UI_Button();
	UI_Button(UI_Element element, int x, int y, bool isVisible = true, bool isInteractible = true, bool isDraggable = false, UI* parent = nullptr,
			SDL_Rect* idle = nullptr, SDL_Rect* hover = nullptr, SDL_Rect* clicked = nullptr);

	bool Draw();

	void CheckInput();				//If any UI element can be interactible, then this function needs to be declared at UI as virtual void.

	void CleanUp();

private:
	SDL_Texture*	tex;			//Texture of the button. Its set with the UI Atlas/Spritesheet.
	SDL_Rect		idle;			//Position and size of the Button's Idle Sprite for the button's idle event.
	SDL_Rect		hover;			//Position and size of the Button's Hover Sprite for the button's hover event.
	SDL_Rect		clicked;		//Position and size of the Button's Clicked for the button's clicked event.
	SDL_Rect		currentRect;	//Buffer SDL_Rect that will hold the position and measures of the correspoinding sprite for each event.
};

#endif // !__UI_BUTTON_H__
#ifndef __UI_IMAGE_H__
#define __UI_IMAGE_H__

#include "UI.h"

class UI_Image : public UI
{
public:
	UI_Image();
	UI_Image(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, bool is_visible = true, bool is_interactible = false, bool is_draggable = false, Module* listener = nullptr, UI* parent = nullptr);

	bool Draw();

	void CheckInput();

	void CleanUp();

private:
	/*const*/ SDL_Texture*	tex;					//Texture of the UI_Image.
};

#endif // !__UI_IMAGE_H__
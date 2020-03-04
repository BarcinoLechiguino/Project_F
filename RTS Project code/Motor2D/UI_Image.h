#ifndef __UI_IMAGE_H__
#define __UI_IMAGE_H__

#include "UI.h"

class UI_Image : public UI
{
public:
	UI_Image();
	UI_Image(UI_Element element, int x, int y, SDL_Rect hitbox, bool isVisible = true, bool isInteractible = false, bool isDraggable = false, UI* parent = nullptr);

	bool Draw();

	void CheckInput();

	void CleanUp();

private:
	SDL_Texture*	tex;						//Texture of the UI_Image.
};

#endif // !__UI_IMAGE_H__
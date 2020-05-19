#ifndef __GUI_IMAGE_H__
#define __GUI_IMAGE_H__

#include "GuiElement.h"

class GuiImage : public GuiElement
{
public:
	GuiImage();
	GuiImage(GUI_ELEMENT_TYPE type, int x, int y, SDL_Rect hitbox, bool is_visible = true, bool is_interactible = false, bool is_draggable = false, Module* listener = nullptr, GuiElement* parent = nullptr);

	bool Draw();
	void CheckInput();
	void CleanUp();

	SDL_Texture* GetTexture() const;

private:
	/*const*/ SDL_Texture*	tex;					//Texture of the GuiImage.
};

#endif // !__GUI_IMAGE_H__
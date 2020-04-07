#ifndef __UI_LIFEBAR_H__
#define __UI_LIFEBAR_H__

#include "UI.h"

class UI_Lifebar : public UI
{
public:
	UI_Lifebar(UI_Element element, int x, int y, SDL_Rect hitbox, bool isVisible = true, Module* listener = nullptr, Entity* attached_unit = nullptr, UI* parent = nullptr);
	~UI_Lifebar();

	bool Draw();

	void CheckInput();

	void CleanUp();

private:
	SDL_Texture* tex;						//Texture of the UI_Image.
	Entity* attached_unit;
};

#endif // !__UI_LIFEBAR_H__

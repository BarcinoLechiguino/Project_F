#ifndef __GUI_HEALTHBAR_H__
#define __GUI_HEALTHBAR_H__

#include "GuiElement.h"

class Entity;

class GuiHealthbar : public GuiElement
{
public:
	GuiHealthbar(GUI_ELEMENT_TYPE type, int x, int y, bool is_visible, SDL_Rect* healthbar, SDL_Rect* background, Entity* attached_entity);
	~GuiHealthbar();

	bool Draw();

	void CheckInput();

	void CleanUp();

public:
	void UpdateHealthbarPosition();					// Will update the healthbar's position and hitbox rect according to the position of the entity the healthbar is attached to.
	void UpdateHealthbarValue();					// Will update the healthbar's value (healthbar rect's width) according to the current health of the attached entity.

private:
	SDL_Texture*	tex;
	
	SDL_Rect		healthbar;						//Size and sprite section of the healthbar.
	SDL_Rect		background;						//Size and sprite section of the healthbar background. Can be nullptr.

	Entity*			attached_entity;				// Entity to which the healthbar will be attached to.
};

#endif // !__GUI_HEALTHBAR_H__

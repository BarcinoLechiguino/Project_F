#ifndef __UI_HEALTHBAR_H__
#define __UI_HEALTHBAR_H__

#include "UI.h"

class Entity;

class UI_Healthbar : public UI
{
public:
	UI_Healthbar(UI_ELEMENT element, int x, int y, bool is_visible, SDL_Rect* healthbar, SDL_Rect* background, Entity* attached_entity);
	~UI_Healthbar();

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

#endif // !__UI_HEALTHBAR_H__

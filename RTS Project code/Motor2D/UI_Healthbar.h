#ifndef __UI_HEALTHBAR_H__
#define __UI_HEALTHBAR_H__

#include "UI.h"

class Entity;

class UI_Healthbar : public UI
{
public:
	UI_Healthbar(UI_ELEMENT element, int x, int y, bool is_visible, SDL_Rect* healthbar, SDL_Rect* background, Entity* attached_unit, bool is_creation_bar, Module* listener, UI* parent);
	~UI_Healthbar();

	bool Draw();

	void CheckInput();

	void CleanUp();

public:
	void UpdateHealthbarPosition();					// Will update the healthbar's position and hitbox rect according to the position of the entity the healthbar is attached to.

	void UpdateBarValue();							// Will update the bar's value according to the attached unit's current value (health or progress time).
	void UpdateHealthBarValue();					//
	void UpdateCreationBarValue();		// Will update the healthbar's health value according to the attached unit's current health.

	void ResetProgressBar();

private:
	/*const*/ SDL_Texture*	tex;				//Texture of the UI_Image.
	
	SDL_Rect		healthbar;				// Sprite and size of the lifebar.
	SDL_Rect		background;				// Sprite and size of the lifebar background. Can be nullptr.

	Entity*			attached_unit;			// Entity to which the healthbar will be attached to.

	bool			is_creation_bar;
	bool			creation_complete;
	float			creation_timer;

};

#endif // !__UI_HEALTHGBAR_H__

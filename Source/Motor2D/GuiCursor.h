#ifndef __GUI_CURSOR_H__
#define __GUI_CURSOR_H__

#include "GuiElement.h"

class GuiCursor : public GuiElement
{
public:
	GuiCursor();
	GuiCursor(GUI_ELEMENT_TYPE type, int x, int y, bool is_visible = true, SDL_Rect* idle = nullptr, SDL_Rect* clicked_idle = nullptr
			, SDL_Rect* hover_ally = nullptr, SDL_Rect* hover_enemy = nullptr, SDL_Rect* hover_resource = nullptr, SDL_Rect* hover_UI = nullptr
			, SDL_Rect* clicked_ally = nullptr, SDL_Rect* clicked_enemy = nullptr, SDL_Rect* clicked_resource = nullptr, SDL_Rect* clicked_UI = nullptr);

	bool Draw();

	void CheckInput();

	void CleanUp();

private:
	void InitializeCursorSections(SDL_Rect* idle, SDL_Rect* clicked_idle, SDL_Rect* hover_ally, SDL_Rect* hover_enemy, SDL_Rect* hover_resource, SDL_Rect* hover_UI
								, SDL_Rect* clicked_ally, SDL_Rect* clicked_enemy, SDL_Rect* clicked_resource, SDL_Rect* clicked_UI);

	void CheckIdleEvent(iPoint mouse_tile_position);									// ------------------------------------------------------------------------
	void CheckHoverEvent(iPoint mouse_tile_position);
	void CheckLeftClickEvent(iPoint mouse_tile_position);
	void CheckRightClickEvent(iPoint mouse_tile_position);								//  MOUSE / CONTROLLER CURSOR EVENTS
	void CheckLeftUnclickEvent(iPoint mouse_tile_position);
	void CheckRightUnclickEvent(iPoint mouse_tile_position);
																						// ------------------------------------------------------------------------
	bool ControllerInteractionButtonsPressed();
	bool ControllerInteractionButtonsReleased();

public:
	bool game_controller_mode;			// Will track whether the cursor is following the mouse or it's being moved by a game controller.

private:
	SDL_Texture* tex;					//Texture of the GuiImage.

	SDL_Rect current_section;			// Current sprite section of the cursor.

	SDL_Rect idle;						// IDLE Sprite of the cursor.
	SDL_Rect clicked_idle;				// IDLE Sprite of the cursor when it is clicked and there is nothing else under the mouse.

	SDL_Rect hover_ally;				// HOVER Sprite for when the cursor hovers an ally entity.
	SDL_Rect hover_enemy;				// HOVER Sprite for when the cursor hovers an enemy entity.
	SDL_Rect hover_resource;			// HOVER Sprite for when the cursor hovers a resource entity.
	SDL_Rect hover_UI;					// HOVER Sprite for when the cursor hovers an interactible UI_Element.

	SDL_Rect clicked_ally;				// CLICKED Sprite for when the cursor hovers an ally entity.
	SDL_Rect clicked_enemy;				// CLICKED Sprite for when the cursor hovers an enemy entity.
	SDL_Rect clicked_resource;			// CLICKED Sprite for when the cursor hovers a resource entity.
	SDL_Rect clicked_UI;				// CLICKED Sprite for when the cursor hovers an interactible UI_Element.

	bool ui_cursor_event_enabled;		// Will track whether or not the cursor has a ui event for when it hovers or clicks a GUI_ELEMENT_TYPE
};

#endif // !__GUI_CURSOR_H__
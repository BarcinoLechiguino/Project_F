#include "Log.h"
#include "Application.h"
#include "Input.h"
#include "Map.h"
#include "EntityManager.h"
#include "Entity.h"
#include "GuiManager.h"

#include "GuiCursor.h"

// Has neither a parent nor a listener argument as this element will represent the cursor on screen.
// Will not use any of the UI_EVENTS as its state will be dictated by whatever the GuiCursor is on top of.
// UI_Elements will get the priority over all other things refering to which section is shown.
GuiCursor::GuiCursor(GUI_ELEMENT_TYPE type, int x, int y, bool is_visible, SDL_Rect* idle, SDL_Rect* clicked_idle
					, SDL_Rect* hover_ally, SDL_Rect* hover_enemy, SDL_Rect* hover_resource, SDL_Rect* hover_UI
					, SDL_Rect* clicked_ally, SDL_Rect* clicked_enemy, SDL_Rect* clicked_resource, SDL_Rect* clicked_UI) : GuiElement(type, x, y, *idle, nullptr, nullptr)
{
	tex = App->gui_manager->GetAtlas();													//The atlas already has the path to the atlas spritesheet. Check how to work around the const

	if (tex == nullptr)
	{
		LOG("Atlas could not be loaded");
	}

	// --- Initializing the element's flags.
	this->is_visible	= is_visible;											//The GuiCursor will always be visible (Or most of the times at least).
	
	is_interactible		= false;												//The GuiCursor will never be interactible itself, it will only follow the mouse (or the controller). 
	is_draggable		= false;												//The GuiCursor will never be draggable itself, it will just follow the mouse (or the controller).
	drag_x_axis			= is_draggable;											//Same as is_draggable.
	drag_y_axis			= is_draggable;											//Same as is_draggable.
	
	previous_mouse_position = iPoint(0, 0);										//Initializes prevMousePos for this UI Element. Safety measure to avoid weird dragging behaviour.
	initial_position = GetScreenPos();											//Records the initial position where the element is at at app execution start.

	// --- Initializing the cursor's sprite sections.
	InitializeCursorSections(idle, clicked_idle, hover_ally, hover_enemy, hover_resource, hover_UI, clicked_ally, clicked_enemy, clicked_resource, clicked_UI);

	if (idle != nullptr)
	{
		current_section = *idle;
	}

	ui_event = GUI_EVENT::IDLE;

	// --- Initializing unnecessary variables, just for safety.
	this->is_filled = false;													// The GuiCursor will always have a sprite section.
	
	this->listener = nullptr;													// As the GuiCursor will never really be interacted with, the listener will not be needed.

	SetLocalPos(iPoint(0, 0));													// As GuiCursor will never have a parent element, the local position will never be used.
}

GuiCursor::GuiCursor()
{

}

bool GuiCursor::Draw()
{
	CheckInput();

	if (GetMousePos() != previous_mouse_position)											// Will move the cursor to the mouse position only if the mouse was moved the last frame.
	{
		SetElementPosition(GetMousePos());
		previous_mouse_position = GetMousePos();
		
		if (game_controller_mode)
		{
			game_controller_mode = false;
			App->gui_manager->focused_element = nullptr;
		}
	}
	else
	{
		if (GetScreenPos() != GetMousePos())												// If the cursor position is different than the cursor position, that means that the cursor was
		{																					// displaced by the game controller.
			game_controller_mode = true;
		}
	}

	//BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &current_section, 0.0f, 1.0f);

	return true;
}

void GuiCursor::CheckInput()
{
	if (is_visible)
	{
		if (!game_controller_mode)
		{
			iPoint mouse_tile_position = GetMouseTilePosition();

			// --- GUI_EVENT::IDLE
			if (ui_event != GUI_EVENT::CLICKED)
			{
				CheckIdleEvent(mouse_tile_position);
			}

			// --- GUI_EVENT::HOVER
			if (ui_event != GUI_EVENT::CLICKED)																		// If the cursor is not currently being clicked.
			{
				CheckHoverEvent(mouse_tile_position);
			}

			// --- GUI_EVENT::CLICKED
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_DOWN)
			{
				CheckLeftClickEvent(mouse_tile_position);
			}

			if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_STATE::KEY_DOWN)
			{
				CheckRightClickEvent(mouse_tile_position);
			}

			// --- GUI_EVENT::UNCLICKED
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_UP)
			{
				CheckLeftUnclickEvent(mouse_tile_position);
			}

			if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_STATE::KEY_UP)
			{
				CheckLeftUnclickEvent(mouse_tile_position);
			}
		}
		else
		{
			iPoint cursor_tile_position = GetScreenPos();

			// --- GUI_EVENT::IDLE
			if (ui_event != GUI_EVENT::CLICKED)
			{
				CheckIdleEvent(cursor_tile_position);
			}

			// --- GUI_EVENT::HOVER
			if (ui_event != GUI_EVENT::CLICKED)																		// If the cursor is not currently being clicked.
			{
				CheckHoverEvent(cursor_tile_position);
			}

			// --- GUI_EVENT::CLICKED
			if (App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_A) == BUTTON_STATE::BUTTON_DOWN
				|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == BUTTON_STATE::BUTTON_DOWN)
			{
				CheckLeftClickEvent(cursor_tile_position);
			}

			if (App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == BUTTON_STATE::BUTTON_DOWN)
			{
				CheckRightClickEvent(cursor_tile_position);
			}

			// --- GUI_EVENT::UNCLICKED
			if (App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_A) == BUTTON_STATE::BUTTON_UP
				|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == BUTTON_STATE::BUTTON_UP)
			{
				CheckLeftUnclickEvent(cursor_tile_position);
			}

			if (App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == BUTTON_STATE::BUTTON_UP)
			{
				CheckRightUnclickEvent(cursor_tile_position);
			}
		}
	}
}

void GuiCursor::CleanUp()
{
	tex = nullptr;
}

// ---  MOUSE CURSOR EVENTS --
void GuiCursor::CheckIdleEvent(iPoint mouse_tile_position)
{
	if (!App->gui_manager->VisibleElementIsUnderCursor() && App->entity_manager->GetEntityAt(mouse_tile_position) == nullptr)//crash
	{
		current_section = idle;
		ui_event = GUI_EVENT::IDLE;

		if (game_controller_mode)
		{
			//App->gui_manager->focused_element = nullptr;
		}
	}
}

void GuiCursor::CheckHoverEvent(iPoint mouse_tile_position)
{	
	if (App->gui_manager->VisibleElementIsUnderCursor())
	{
		if (ui_cursor_event_enabled)
		{
			current_section = hover_UI;
		}
		else
		{
			current_section = idle;
		}

		if (game_controller_mode)
		{
			if (App->gui_manager->FirstInteractibleElementUnderCursor() != nullptr)
			{
				App->gui_manager->focused_element = App->gui_manager->FirstInteractibleElementUnderCursor();
			}
		}

		ui_event = GUI_EVENT::HOVER;
	}
	else
	{
		Entity* entity = App->entity_manager->GetEntityAt(mouse_tile_position);

		if (entity != nullptr)																			// If there is an entity under the mouse.
		{
			if (entity->is_visible)
			{
				if (App->entity_manager->IsAllyEntity(entity))
				{
					current_section = hover_ally;
				}
				else if (App->entity_manager->IsEnemyEntity(entity))
				{
					current_section = hover_enemy;
				}
				else
				{
					current_section = hover_resource;
				}
			}
		}

		ui_event = GUI_EVENT::HOVER;
	}
}

void GuiCursor::CheckLeftClickEvent(iPoint mouse_tile_position)
{
	if (App->gui_manager->FirstInteractibleElementUnderCursor() != nullptr)								// If there is an interactible UI_Element under the mouse.
	{
		if (ui_cursor_event_enabled)
		{
			current_section = clicked_UI;
		}
		else
		{
			current_section = clicked_idle;
		}
	}
	else
	{
		Entity* entity = App->entity_manager->GetEntityAt(mouse_tile_position);
		
		if (entity != nullptr)																			// If there is an entity under the mouse.
		{
			if (entity->is_visible)
			{
				if (App->entity_manager->IsAllyEntity(entity))
				{
					current_section = clicked_ally;
				}
				else if (App->entity_manager->IsEnemyEntity(entity))
				{
					current_section = clicked_enemy;
				}
				else
				{
					current_section = clicked_resource;
				}
			}
		}
		else
		{
			if (!App->gui_manager->VisibleElementIsUnderCursor() || !ui_cursor_event_enabled)			// If there is nothing under the mouse inside the map.								
			{
				current_section = clicked_idle;
			}
		}
	}

	ui_event = GUI_EVENT::CLICKED;
}

void GuiCursor::CheckRightClickEvent(iPoint mouse_tile_position)
{
	if (App->gui_manager->FirstInteractibleElementUnderCursor() != nullptr)								// If there is an interactible UI_Element under the mouse.
	{
		if (ui_cursor_event_enabled)
		{
			current_section = clicked_UI;
		}
		else
		{
			current_section = clicked_idle;
		}
	}
	else
	{
		Entity* entity = App->entity_manager->GetEntityAt(mouse_tile_position);
		
		if (entity != nullptr)																			// If there is an entity under the mouse.
		{
			if (entity->is_visible)
			{
				if (App->entity_manager->IsAllyEntity(entity))
				{
					current_section = clicked_ally;
				}
				else if (App->entity_manager->IsEnemyEntity(entity))
				{
					current_section = clicked_enemy;
				}
				else
				{
					current_section = clicked_resource;
				}
			}
		}
		else
		{
			if (!App->gui_manager->VisibleElementIsUnderCursor() || !ui_cursor_event_enabled)			// If there is nothing under the mouse inside the map.								
			{
				current_section = clicked_idle;
			}
		}
	}

	ui_event = GUI_EVENT::CLICKED;
}

void GuiCursor::CheckLeftUnclickEvent(iPoint mouse_tile_position)
{
	if (App->gui_manager->VisibleElementIsUnderCursor())												// Cursor is over a UI_Element
	{
		if (ui_cursor_event_enabled)
		{
			current_section = hover_UI;
		}
		else
		{
			current_section = idle;
		}
	}
	else
	{
		Entity* entity = App->entity_manager->GetEntityAt(mouse_tile_position);
		
		if (entity != nullptr)																		// Cursor is over an Entity.
		{
			if (entity->is_visible)
			{
				if (App->entity_manager->IsAllyEntity(entity))
				{
					current_section = hover_ally;
				}
				else if (App->entity_manager->IsEnemyEntity(entity))
				{
					current_section = hover_enemy;
				}
				else
				{
					current_section = hover_resource;
				}
			}
		}
		else
		{
			current_section = idle;																	// Cursor is not over an entity or a UI_Element.
		}
	}

	ui_event = GUI_EVENT::UNCLICKED;
}

void GuiCursor::CheckRightUnclickEvent(iPoint mouse_tile_position)
{
	if (App->gui_manager->VisibleElementIsUnderCursor())												// Cursor is over a UI_Element
	{	
		if (ui_cursor_event_enabled)
		{
			current_section = hover_UI;
		}
		else
		{
			current_section = idle;
		}
	}
	else
	{
		Entity* entity = App->entity_manager->GetEntityAt(mouse_tile_position);
		
		if (entity != nullptr)																		// Cursor is over an Entity.
		{
			if (entity->is_visible)
			{
				if (App->entity_manager->IsAllyEntity(entity))
				{
					current_section = hover_ally;
				}
				else if (App->entity_manager->IsEnemyEntity(entity))
				{
					current_section = hover_enemy;
				}
				else
				{
					current_section = hover_resource;
				}
			}
		}
		else
		{
			current_section = idle;																	// Cursor is not over an entity or a UI_Element.
		}
	}

	ui_event = GUI_EVENT::UNCLICKED;
}

bool GuiCursor::ControllerInteractionButtonsPressed()
{
	return (App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_A) == BUTTON_STATE::BUTTON_DOWN
		|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == BUTTON_STATE::BUTTON_DOWN
		|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == BUTTON_STATE::BUTTON_DOWN);
}

bool GuiCursor::ControllerInteractionButtonsReleased()
{
	return (App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_A) == BUTTON_STATE::BUTTON_UP
		|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == BUTTON_STATE::BUTTON_UP
		|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == BUTTON_STATE::BUTTON_UP);
}

void GuiCursor::InitializeCursorSections(SDL_Rect* idle, SDL_Rect* clicked_idle, SDL_Rect* hover_ally, SDL_Rect* hover_enemy, SDL_Rect* hover_resource, SDL_Rect* hover_UI
										, SDL_Rect* clicked_ally, SDL_Rect* clicked_enemy, SDL_Rect* clicked_resource, SDL_Rect* clicked_UI)
{
	if (idle != nullptr)
	{
		this->idle = *idle;
	}

	if (clicked_idle != nullptr)
	{
		this->clicked_idle = *clicked_idle;
	}

	if (hover_ally != nullptr)
	{
		this->hover_ally = *hover_ally;
	}

	if (hover_enemy != nullptr)
	{
		this->hover_enemy = *hover_enemy;
	}

	if (hover_resource != nullptr)
	{
		this->hover_resource = *hover_resource;
	}

	if (hover_UI != nullptr)
	{
		this->hover_UI = *hover_UI;
	}

	if (clicked_ally != nullptr)
	{
		this->clicked_ally = *clicked_ally;
	}

	if (clicked_enemy != nullptr)
	{
		this->clicked_enemy = *clicked_enemy;
	}

	if (clicked_resource != nullptr)
	{
		this->clicked_resource = *clicked_resource;
	}

	if (clicked_UI != nullptr)
	{
		this->clicked_UI = *clicked_UI;
	}

	if (hover_UI == nullptr || clicked_UI == nullptr)
	{
		ui_cursor_event_enabled = false;
	}
	else
	{
		ui_cursor_event_enabled = true;
	}
}
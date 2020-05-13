#include "Brofiler\Brofiler.h"

#include "Application.h"
#include "Textures.h"
#include "Input.h"
#include "GuiManager.h"

#include "UI_Button.h"

//UI_Button can be interactible (will almost always be) and draggable. Can potentially receive all events.
//This element can receive up to 3 rects containing the coordinates of the sprites for each event (IDLE, HOVER & CLICKED).
UI_Button::UI_Button(UI_ELEMENT element, int x, int y, bool is_visible, bool is_interactible, bool is_draggable, Module* listener, UI* parent,
				SDL_Rect* idle, SDL_Rect* hover, SDL_Rect* clicked) : UI(element, x, y, *idle, listener, parent)
{
	tex = App->gui_manager->GetAtlas();

	// --- Setting this element's flags to the ones passed as argument.
	this->is_visible = is_visible;												//Sets the isVisible flag to the one passed as argument.
	this->is_interactible = is_interactible;									//Sets the isInteractible flag to the one passed as argument. 
	this->is_draggable = is_draggable;											//Sets the isDraggable flag to the one passed as argument.
	this->drag_x_axis = is_draggable;											//Sets the dragXaxis flag to the same as isDraggable. If it needs to be changed, it has to be done externally.
	this->drag_y_axis = is_draggable;											//Sets the dragYaxis flag to the same as isDraggable. If it needs to be changed, it has to be done externally.
	previous_mouse_position = iPoint(0, 0);										//Initializes prevMousePos for this UI Element. Safety measure to avoid weird dragging behaviour.
	initial_position = GetScreenPos();											//Records the initial position where the element is at at app execution start.
	
	//If the SDL_Rect pointers are not null, then set the button rects to their data memebers.
	if (idle != NULL)
	{
		this->idle = *idle;
	}

	if (hover != NULL)
	{
		this->hover = *hover;
	}

	if (clicked != NULL)
	{
		this->clicked = *clicked;
	}

	if (this->is_interactible)													//If the button element is interactible.
	{
		this->listener = listener;												//This button's listener is set to the App->gui module (For OnCallEvent()).
	}

	if (parent != NULL)															//If a parent is passed as argument.
	{

		int localPosX = x - this->parent->GetScreenPos().x;						//Gets the local position of the Button element in the X Axis.
		int localPosY = y - this->parent->GetScreenPos().y;						//Gets the local position of the Button element in the Y Axis.
		
		iPoint localPos = { localPosX,localPosY };								//Buffer iPoint to pass it as argument to SetLocalPos().

		SetLocalPos(localPos);													//Sets the local poisition of this Button element to the given localPos.
	}

	ui_event = UI_EVENT::IDLE;
	current_rect = this->idle;
}

UI_Button::UI_Button() : UI()
{}

bool UI_Button::Draw()
{
	CheckInput();																				//Calling "Update" and Draw at the same time. 

	BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &current_rect, 0.0f, 1.0f);

	return true;
}

// --- This Method checks for any inputs that the UI_Text element might have received and "returns" an event.
void UI_Button::CheckInput()
{
	BROFILER_CATEGORY("Button_CheckInput", Profiler::Color::GhostWhite);

	if (is_visible)																				//If the Button element is visible.
	{
		GetMousePos();																			//Gets the mouse's position on the screen.

		// --- IDLE EVENT
		if (!IsHovered() && (ui_event != UI_EVENT::HOVER))										//If the mouse is not on the button and event is not HOVER. TMP fix to have UNHOVER event.
		{
			ui_event = UI_EVENT::IDLE;
			current_rect = idle;																//Button Idle sprite.
		}

		// --- HOVER EVENT
		if ((IsHovered() && IsForemostElement()) || IsFocused())								//If the mouse is on the button.
		{
			ui_event = UI_EVENT::HOVER;															//Button Hover sprite.
			current_rect = hover;
		}

		// --- UNHOVER EVENT
		if ((!IsHovered() && (ui_event == UI_EVENT::HOVER) && !IsFocused()))									//If the mouse is on the button.
		{		
			ui_event = UI_EVENT::UNHOVER;
			current_rect = idle;
		}

		// --- CLICKED EVENT (Left Click)
		if (IsHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_DOWN)			//If the mouse is on the button and the left mouse button is clicked.
		{
			if (IsForemostElement())
			{
				previous_mouse_position = GetMousePos();										//Sets the previous mouse position.
				initial_position = GetScreenPos();												//Sets initialPosition with the current position at mouse KEY_DOWN.	
				is_drag_target = true;															//
			}
		}

		if ((IsHovered() || is_drag_target) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_REPEAT)		//If the mouse is on the button and the left mouse button is pressed continuously.
		{
			if (IsForemostElement() || is_drag_target)															//If the UI Text element is the foremost element under the mouse. 
			{
				ui_event = UI_EVENT::CLICKED;
				current_rect = clicked;															//Button Clicked sprite is maintained.

				if (ElementCanBeDragged() && is_draggable)										//If the UI Button element is draggable and is the foremost element under the mouse.
				{
					DragElement();																//The UI Button element is dragged.

					CheckElementChilds();														//Checks if this UI Button has any childs and updates them in consequence.

					previous_mouse_position = GetMousePos();									//Updates prevMousePos so it can be dragged again next frame.
				}
			}
		}

		// --- UNCLICKED EVENT (Left Click)
		if ((IsHovered() || is_drag_target) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_UP)			//If the mouse is on the button and the left mouse button is released.
		{
			if (IsForemostElement() && ElementRemainedInPlace())								//If the UI Text element is the foremost element under the mouse and has not been dragged. 
			{
				ui_event = UI_EVENT::UNCLICKED;
			}
			else
			{
				ui_event = UI_EVENT::UNHOVER;
			}

			if (is_drag_target)
			{
				is_drag_target = false;
				initial_position = GetScreenPos();
			}
		}

		if (listener != nullptr)
		{
			listener->OnEventCall(this, ui_event);												//This UI element's pointer and ui_event are passed as arguments to the OnEventCall() function.
		}
	}
}

void UI_Button::CleanUp()
{
	tex = nullptr;
}
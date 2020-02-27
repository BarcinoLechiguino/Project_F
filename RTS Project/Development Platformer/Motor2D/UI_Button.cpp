#include "j1App.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "UI_Button.h"

#include "Brofiler\Brofiler.h"

//UI_Button can be interactible (will almost always be) and draggable. Can potentially receive all events.
//This element can receive up to 3 rects containing the coordinates of the sprites for each event (IDLE, HOVER & CLICKED).
UI_Button::UI_Button(UI_Element element, int x, int y, bool isVisible, bool isInteractible, bool isDraggable, UI* parent,
				SDL_Rect* idle, SDL_Rect* hover, SDL_Rect* clicked) : UI(element, x, y, *idle, parent)
{
	tex = App->gui->GetAtlas();

	// --- Setting this element's flags to the ones passed as argument.
	this->isVisible = isVisible;												//Sets the isVisible flag to the one passed as argument.
	this->isInteractible = isInteractible;										//Sets the isInteractible flag to the one passed as argument. 
	this->isDraggable = isDraggable;											//Sets the isDraggable flag to the one passed as argument.
	this->dragXAxis = isDraggable;												//Sets the dragXaxis flag to the same as isDraggable. If it needs to be changed, it has to be done externally.
	this->dragYAxis = isDraggable;												//Sets the dragYaxis flag to the same as isDraggable. If it needs to be changed, it has to be done externally.
	prevMousePos = iPoint(0, 0);												//Initializes prevMousePos for this UI Element. Safety measure to avoid weird dragging behaviour.
	initialPosition = GetScreenPos();											//Records the initial position where the element is at at app execution start.
	
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

	if (this->isInteractible)													//If the button element is interactible.
	{
		listener = App->gui;													//This button's listener is set to the App->gui module (For OnCallEvent()).
	}

	if (parent != NULL)															//If a parent is passed as argument.
	{
		int localPosX = x - this->parent->GetScreenPos().x;						//Gets the local position of the Button element in the X Axis.
		int localPosY = y - this->parent->GetScreenPos().y;						//Gets the local position of the Button element in the Y Axis.
		
		iPoint localPos = { localPosX,localPosY };								//Buffer iPoint to pass it as argument to SetLocalPos().

		SetLocalPos(localPos);													//Sets the local poisition of this Button element to the given localPos.
	}
}

UI_Button::UI_Button() : UI()
{}

bool UI_Button::Draw()
{
	CheckInput();																//Calling "Update" and Draw at the same time. 

	BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &currentRect);

	return true;
}

// --- This Method checks for any inputs that the UI_Text element might have received and "returns" an event.
void UI_Button::CheckInput()
{
	BROFILER_CATEGORY("Button_CheckInput", Profiler::Color::GhostWhite);

	if (isVisible)																				//If the Button element is visible.
	{
		GetMousePos();																			//Gets the mouse's position on the screen.

		// --- IDLE EVENT
		if (!IsHovered())																		//If the mouse is not on the button.
		{
			ui_event = UI_Event::IDLE;
			currentRect = idle;																	//Button Idle sprite.
		}

		// --- HOVER EVENT
		if ((IsHovered() && IsForemostElement()) || IsFocused())								//If the mouse is on the button.
		{
			ui_event = UI_Event::HOVER;															//Button Hover sprite.
			currentRect = hover;
		}

		// --- CLICKED EVENT (Left Click)
		if (IsHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)			//If the mouse is on the button and the left mouse button is clicked.
		{
			if (IsForemostElement())
			{
				prevMousePos = GetMousePos();													//Sets the previous mouse position.
				initialPosition = GetScreenPos();												//Sets initialPosition with the current position at mouse KEY_DOWN.	
				isDragTarget = true;															//
			}
		}

		if ((IsHovered() || isDragTarget) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)		//If the mouse is on the button and the left mouse button is pressed continuously.
		{
			if (IsForemostElement() || isDragTarget)															//If the UI Text element is the foremost element under the mouse. 
			{
				ui_event = UI_Event::CLICKED;
				currentRect = clicked;															//Button Clicked sprite is maintained.

				if (ElementCanBeDragged() && isDraggable)										//If the UI Button element is draggable and is the foremost element under the mouse.
				{
					DragElement();																//The UI Button element is dragged.

					CheckElementChilds();														//Checks if this UI Button has any childs and updates them in consequence.

					prevMousePos = GetMousePos();												//Updates prevMousePos so it can be dragged again next frame.
				}
			}
		}

		// --- UNCLICKED EVENT (Left Click)
		if ((IsHovered() || isDragTarget) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)			//If the mouse is on the button and the left mouse button is released.
		{
			if (IsForemostElement() && ElementRemainedInPlace())								//If the UI Text element is the foremost element under the mouse and has not been dragged. 
			{
				ui_event = UI_Event::UNCLICKED;
			}

			if (isDragTarget)
			{
				isDragTarget = false;
				initialPosition = GetScreenPos();
			}
			
			//currentRect = clicked;															//Button Clicked sprite.
		}

		listener->OnEventCall(this, ui_event);													//This UI element's pointer and ui_event are passed as arguments to the OnEventCall() function.
	}
}

void UI_Button::CleanUp()
{
	App->tex->UnLoad(tex);
}
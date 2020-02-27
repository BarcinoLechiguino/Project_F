#include "j1App.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "UI.h"
#include "UI_Image.h"

#include "Brofiler\Brofiler.h"

//UI_Image will always be uninteractible and will have 2 events: IDLE & CLICKED (CLICKED when the image is clicked and can be dragged).
//UI_Image can be draggable and can have a parent element.
//hitbox argument serves the purpose of both setting the UI_Image's "input collider" and locating the right sprite in the Atlas(tex).
UI_Image::UI_Image(UI_Element element, int x, int y, SDL_Rect hitbox, bool isVisible, bool isInteractible, bool isDraggable, UI* parent) : UI(element, x, y, hitbox, parent) //When a UI_Image's constructor is called, a UI element is initialized as a IMAGE element.
{
	tex = App->gui->GetAtlas();													//The atlas already has the path to the atlas spritesheet. Check how to work around the const

	if (tex == NULL)
	{
		LOG("Atlas could not be loaded");
	}

	// --- Setting this element's flags to the ones passed as argument.
	this->isVisible = isVisible;												//Sets the isVisible flag to the one passed as argument.
	this->isInteractible = isInteractible;										//Sets the isInteractible flag to the one passed as argument. 
	this->isDraggable = isDraggable;											//Sets the isDraggable flag to the one passed as argument.
	this->dragXAxis = isDraggable;												//Sets the dragXaxis flag to the same as isDraggable. If it needs to be changed, it has to be done externally.
	this->dragYAxis = isDraggable;												//Sets the dragYaxis flag to the same as isDraggable. If it needs to be changed, it has to be done externally.
	this->isFilled = isFilled;
	prevMousePos = iPoint(0, 0);												//Initializes prevMousePos for this UI Element. Safety measure to avoid weird dragging behaviour.
	initialPosition = GetScreenPos();											//Records the initial position where the element is at at app execution start.

	if (this->isInteractible)													//If the Image element is interactible.
	{
		listener = App->gui;													//This Image's listener is set to the App->gui module (For OnCallEvent()).
	}

	if (parent != NULL)															//If a parent is passed as argument.
	{
		int localPosX = x - parent->GetScreenPos().x;							//Gets the local position of the Image element in the X Axis.
		int localPosY = y - parent->GetScreenPos().y;							//Gets the local position of the Image element in the Y Axis.
		
		iPoint localPos = { localPosX, localPosY };								//Buffer iPoint to pass it as argument to SetLocalPos().

		SetLocalPos(localPos);													//Sets the local poisition of this Image Element to the given localPos.
	}
}

UI_Image::UI_Image() : UI()	//Default Constructor
{}

bool UI_Image::Draw()
{
	CheckInput();
	
	if (this->element == UI_Element::IMAGE)
	{
		BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &GetScreenRect());		//GetPosition() is used as the position variable in the UI parent class will be initialized with the values of the UI_Image instance at constructor call
	}
	
	return true;
}

// --- This Method checks for any inputs that the UI_Image element might have received and "returns" an event.
void UI_Image::CheckInput()
{
	BROFILER_CATEGORY("Image_CheckInput", Profiler::Color::Gold);

	if (isVisible)																				//If the image element is visible
	{
		GetMousePos();																			//Gets the mouse's position on the screen.

		if (!IsHovered())																		//If the mouse is not on the image.
		{
			ui_event = UI_Event::IDLE;
		}

		if (isDraggable)																		//If the image element is draggable.
		{
			if (IsHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)		//If the mouse is on the image and the left mouse button is pressed.
			{
				if (IsForemostElement())
				{
					prevMousePos = GetMousePos();												//Sets the initial position where the mouse was before starting to drag the element.
					initialPosition = GetScreenPos();											//Sets initialPosition with the current position at mouse KEY_DOWN.
					isDragTarget = true;														//Sets the element as the drag target.
				}
			}

			if ((IsHovered() || isDragTarget) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)	//If the mouse is on the image and the left mouse button is continuously pressed.
			{
				if (IsForemostElement() || isDragTarget)												//If it is the first element under the mouse (in inverse order of draw) or is the drag target.
				{
					ui_event = UI_Event::CLICKED;
					
					if (ElementCanBeDragged())															//If the UI Image element meets all drag conditions or is the drag target.
					{
						if (this->parent != NULL)														//If this UI Image has a parent.
						{
							if (this->parent->element != UI_Element::SCROLLBAR)							//If this element is not the scrollbar's thumb.
							{
								DragElement();															//The element is dragged around.
							}
							else
							{
								AxisRestrictedDragElement(parent->dragXAxis, parent->dragYAxis);		//The element is dragged along a specific axis.
							}
						}
						else
						{
							DragElement();																//The element is dragged around.
						}

						CheckElementChilds();															//Checks if this image element has any childs and updates them in case the image element (parent) has had any change in position.

						prevMousePos = GetMousePos();													//prevMousePos is set with the new position where the mouse is after dragging for a frame.
					}
				}
			}

			if ((IsHovered() || isDragTarget) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)				// -------------------------------------------------------------------
			{
				if (isDragTarget)
				{
					isDragTarget = false;
					initialPosition = GetScreenPos();
				}
			}

			if (isInteractible)																	//If the image element is interactible.
			{
				listener->OnEventCall(this, ui_event);											//The listener call the OnEventCall() method passing this UI_Image and it's event as arguments.
			}
		}
	}
}

void UI_Image::CleanUp()
{
	LOG("Unloading UI_Image Textures");
	App->tex->UnLoad(tex);
}
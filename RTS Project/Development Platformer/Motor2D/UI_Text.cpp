#include "j1App.h"
#include "j1Fonts.h"
#include "j1Gui.h"
#include "UI.h"
#include "UI_Text.h"

#include "Brofiler\Brofiler.h"

//UI_Text can be interactible and draggable. Can potentially have all events.
//This element can receive up to 5 different strings (one for each possible event).
UI_Text::UI_Text(UI_Element element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, bool isVisible, bool isInteractible, bool isDraggable, UI* parent, p2SString* string,
			p2SString* hoverString, p2SString* leftClickString, p2SString* rightClickString): UI(element, x, y, hitbox, parent),
			idleTex(nullptr), hoverTex(nullptr), leftClickTex(nullptr), rightClickTex(nullptr), inputTextTex (nullptr), currentTex (nullptr)
{	
	// --- Setting this element's flags to the ones passed as argument.
	this->isVisible = isVisible;												//Sets the isVisible flag to the one passed as argument.
	this->isInteractible = isInteractible;										//Sets the isInteractible flag to the one passed as argument. 
	this->isDraggable = isDraggable;											//Sets the isDraggable flag to the one passed as argument.
	this->dragXAxis = isDraggable;												//Sets the dragXaxis flag to the same as isDraggable. If it needs to be changed, it has to be done externally.
	this->dragYAxis = isDraggable;												//Sets the dragYaxis flag to the same as isDraggable. If it needs to be changed, it has to be done externally.
	prevMousePos = iPoint(0, 0);												//Initializes prevMousePos for this UI Element. Safety measure to avoid weird dragging behaviour.
	initialPosition = GetScreenPos();											//Records the initial position where the element is at at app execution start.
	// ---------------------------------------------------------------

	//Loading all strings. Print() generates a texture with the given string with the a font and a colour.
	if (string != NULL)
	{
		idleTex = App->font->Print(string->GetString(), fontColour, font);					//Creates the texture for the idle state.
	}

	if (hoverString != NULL)
	{
		hoverTex = App->font->Print(hoverString->GetString(), fontColour, font);			//Creates the texture for the hover state.
	}
	
	if (leftClickString != NULL)
	{
		leftClickTex = App->font->Print(leftClickString->GetString(), fontColour, font);	//Creates the texture for the left-clicked state.
	}

	if (rightClickString != NULL)
	{
		rightClickTex = App->font->Print(rightClickString->GetString(), fontColour, font);	//Crates the texture for the right_clicked state.
	}
	// ----------------------------------------------------------------------------------------------------

	if (this->isInteractible)												//If the Image element is interactible.
	{
		listener = App->gui;												//This Text element's listener is set to the App->gui module (For OnCallEvent()).
	}

	if (parent != NULL)														//If a parent is passed as argument.
	{
		int localPosX = x - parent->GetScreenPos().x;						//Gets the local position of the Text element in the X Axis.
		int localPosY = y - parent->GetScreenPos().y;						//Gets the local position of the Text element in the Y Axis.

		iPoint localPos = { localPosX, localPosY };							//Buffer iPoint to pass it as argument to SetLocalPos().

		SetLocalPos(localPos);												//Sets the local poisition of this Text Element to the given localPos.
	}

	this->font = font;
	this->font_colour = fontColour;

	textRect = { 0, 0, 0, 0 };			//
}

UI_Text::UI_Text() : UI ()		//Default Constructor
{}

bool UI_Text::Draw()
{	
	CheckInput();

	if (currentTex != nullptr)
	{
		SDL_QueryTexture(currentTex, NULL, NULL, &textRect.w, &textRect.h);			//REVISE  THIS  LATER

		SetHitbox({ GetHitbox().x, GetHitbox().y, textRect.w, textRect.h });

		BlitElement(currentTex, GetScreenPos().x, GetScreenPos().y, NULL);
	}

	return true;
}

// --- This Method checks for any inputs that the UI_Text element might have received and "returns" an event.
void UI_Text::CheckInput()
{	
	BROFILER_CATEGORY("Text_CheckInput", Profiler::Color::Indigo);

	if (!isVisible)
	{
		currentTex = NULL;
	}
	
	if (isVisible)																						//If the Text element is visible.
	{	
		GetMousePos();																					//Gets the mouse's position on the screen.

		// --- IDLE EVENT
		if (!IsHovered() && ui_event != UI_Event::FOCUSED)												//If the mouse is not on the text.
		{
			ui_event = UI_Event::IDLE;

			if (inputTextTex == NULL)																	//This sometimes crashes the Query_texture.
			{
				currentTex = idleTex;																	//Blit the idle text.
			}
			else
			{
				currentTex = inputTextTex;
			}
		}

		/*if (ui_event == UI_Event::FOCUSED)
		{
			currentTex = inputTextTex;
		}*/

		if (isInteractible)																				//If the Text element is interactible.
		{
			// --- HOVER EVENT
			if ((IsHovered() && IsForemostElement()) /*|| IsFocused()*/)								//If the mouse is on the text.
			{
				ui_event = UI_Event::HOVER;

				if (hoverTex != NULL)
				{
					currentTex = hoverTex;																//Blit the hover text.
				}
			}

			// --- CLICKED EVENT (Left Click)
			if (IsHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)				//If the mouse is on the text and the left mouse button is pressed.
			{
				if (IsForemostElement())
				{
					prevMousePos = GetMousePos();														//Sets the initial position where the mouse was before starting to drag the element.
					initialPosition = GetScreenPos();													//Sets initialPosition with the current position at mouse KEY_DOWN.
					isDragTarget = true;																//Sets the element as the drag target.
				}
			}

			if ((IsHovered() || isDragTarget) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)		//If the mouse is on the text and the left mouse button is being pressed.
			{
				if (IsForemostElement() || isDragTarget)															//If the UI Text element is the foremost element under the mouse. 
				{
					ui_event = UI_Event::CLICKED;

					if (leftClickTex != NULL)
					{
						currentTex = leftClickTex;														//Blit the left click text.
					}

					if (ElementCanBeDragged())															//If the UI Text element is draggable and is the foremost element under the mouse. 
					{
						DragElement();																	//The Text element is dragged.
						
						CheckElementChilds();															//Checks if this Text element has any childs and updates them in consequence.

						prevMousePos = GetMousePos();													//Updates prevMousePos so it can be dragged again next frame.
					}
				}
			}

			// --- UNCLICKED EVENT (Left Click)
			if ((IsHovered() || isDragTarget) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)		//If the mouse is on the text and the left mouse button is released.
			{
				if (IsForemostElement() && ElementRemainedInPlace())									//If the UI Text element is the foremost element under the mouse and has not been dragged. 
				{
					ui_event = UI_Event::UNCLICKED;
				}

				if (isDragTarget)
				{
					isDragTarget = false;
					initialPosition = GetScreenPos();
				}

				//currentRect = clicked;																//Button Hover sprite.
			}

			// --- CLICKED EVENT (Right Click)
			if (IsHovered() == true && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)	//If the mouse is on the button and the right mouse button is pressed.
			{
				if (IsForemostElement())																//If the UI Text element is the foremost element under the mouse. 
				{
					ui_event = UI_Event::CLICKED;

					if (rightClickTex != NULL)
					{
						currentTex = rightClickTex;														//Blit the right click text.
					}
				}
			}

			listener->OnEventCall(this, ui_event);														//The listener call the OnEventCall() method passing this UI_Text and it's event as arguments.
		}
	}
}

void UI_Text::CleanUp()
{
	if (idleTex != NULL)
	{
		App->tex->UnLoad(idleTex);
	}

	if (inputTextTex != NULL)
	{
		App->tex->UnLoad(inputTextTex);
	}
}

SDL_Texture* UI_Text::GetCurrentStringTex()
{
	return currentTex;
}

void UI_Text::DeleteCurrentStringTex()
{
	currentTex = NULL;
}

// ----------------------------------------- INPUT TEXT METHODS -----------------------------------------
void UI_Text::RefreshTextInput(const char* newString)
{
	if (inputTextTex == NULL)
	{
		inputTextTex = App->font->Print(newString, font_colour, font);
	}
	else
	{
		App->tex->UnLoad(inputTextTex);
		inputTextTex = App->font->Print(newString, font_colour, font);
	}

	currentTex = inputTextTex;

	if (currentTex != NULL)
	{
		LOG("CurrentTex is %s", newString);
	}
}
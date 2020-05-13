#include "p2Log.h"

#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Fonts.h"
#include "GuiManager.h"
#include "UI_Image.h"
#include "UI_Text.h"

#include "UI_InputBox.h"

//UI_InputBox will always be interactible (although it can be set to not be), and can be draggable. Can potentially receive all events.
//The element receives as arguments all the requiered elements to create a UI_Image (Background), a UI_Text (input text) and another UI_Image (cursor).
UI_InputBox::UI_InputBox(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, SDL_Rect cursorSize, SDL_Color cursor_colour, iPoint text_offset,
				float blink_frequency, bool is_visible, bool is_interactible, bool is_draggable, Module* listener, UI* parent, std::string* defaultString, bool emptyElements) 
				: UI(element, x, y, hitbox, listener, parent)
{	
	if (is_interactible)															//If the Input Box element is interactible.
	{
		this->listener = listener;													//This input box's listener is set to the App->gui module (For OnCallEvent()).
	}

	if (parent != NULL)																//If a parent is passed as argument.
	{
		int localPosX = x - this->parent->GetScreenPos().x;							//Gets the local position of the Button element in the X Axis.
		int localPosY = y - this->parent->GetScreenPos().y;							//Gets the local position of the Button element in the Y Axis.

		iPoint localPos = { localPosX,localPosY };									//Buffer iPoint to pass it as argument to SetLocalPos().

		SetLocalPos(localPos);														//Sets the local poisition of this Button element to the given localPos.
	}

	// ----------------------------- INPUT BOX VARIABLES ---------------------------------
	// --- Input Box Flags
	this->is_visible = is_visible;													//Sets the isVisible flag of the input box to the one passed as argument.
	this->is_interactible = is_interactible;										//Sets the isInteractible flag of the input box to the one passed as argument.
	this->is_draggable = is_draggable;												//Sets the isDraggable flag of the input box to the one passed as argument.
	this->drag_x_axis = is_draggable;												//
	this->drag_y_axis = is_draggable;												//
	previous_mouse_position = iPoint(0, 0);											//Initializes prevMousePos for this UI Element. Safety measure to avoid weird dragging behaviour.
	initial_position = GetScreenPos();												//Records the initial position where the input box is at app execution start.

	// --- Input Box Elements
	if (!emptyElements)
	{
		this->background = UI_Image(UI_ELEMENT::IMAGE, x, y, hitbox, is_visible, false, false, nullptr, this);
		//this->cursor = UI_Image(UI_Element::IMAGE, x + textOffset.x, y + textOffset.y, cursorSize, isVisible, false, false, this);
	}
	else
	{
		this->background = UI_Image(UI_ELEMENT::EMPTY, x, y, hitbox, is_visible, false, false, nullptr, this);
	}

	this->cursor = UI_Image(UI_ELEMENT::EMPTY, x + text_offset.x, y + text_offset.y, cursorSize, is_visible, false, false, nullptr, this);
	this->text = UI_Text(UI_ELEMENT::TEXT, x + text_offset.x, y + text_offset.y, hitbox, font, fontColour, is_visible, false, false, nullptr, this, defaultString);
	
	// --- Text Variables
	this->font = font;																//Sets the UI input box font to the one being passed as argument.
	this->cursor_colour = cursor_colour;											//Sets the cursor colour to the cursor colour being passed as argument. (See DrawCursor())
	this->text_offset = text_offset;												//Sets the text offset to the text offset being passed as arguments.
	this->text_width = 0;															//As the initial input text will be empty, text width is set to 0.
	this->text_height = 0;															//As the initial input text will be empty, text height is set to 0. 
	this->previous_length = 0;
	
	// ---  Cursor Variables
	this->cursor_positions[GetCurrentCursorIndex()] = cursor.GetScreenPos().x;		//As there will be no initial text, the first cursor position (index 0) will be the cursor's origin position.
	cursor.is_visible = false;
	blink_timer = 0.0f;
	
	if (blink_frequency >= 0.0f)
		this->blink_frequency = blink_frequency;

	if (blink_frequency < 0.0f)
		this->blink_frequency = -blink_frequency;
	// --------------------------------------------------------------------------------------
}

UI_InputBox::UI_InputBox() : UI()
{}

bool UI_InputBox::Draw()
{
	CheckInput();

	DrawInputBoxElements();

	//BlitElement(tex, GetScreenPos().x, GetScreenPos().y, &GetScreenRect());

	return true;
}

void UI_InputBox::CheckInput()
{
	if (is_visible)
	{
		CheckInputBoxState();
		
		GetMousePos();																				//Gets the mouse's position on the screen.

		// --- IDLE EVENT
		if (!IsHovered())																			//If the mouse is not on the text.
		{
			ui_event = UI_EVENT::IDLE;
		}

		if (is_interactible)																		//If the Text element is interactible.
		{
			// --- HOVER EVENT
			if ((IsHovered() && IsForemostElement()) /*|| IsFocused()*/)							//If the mouse is on the text.
			{
				ui_event = UI_EVENT::HOVER;
			}

			
			// --- FOCUS EVENT
			//CheckFocus();
			
			// --- CLICKED EVENT (Left Click)
			if (IsHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_DOWN)			//If the mouse is on the text and the left mouse button is pressed.
			{
				previous_mouse_position = GetMousePos();											//Sets the initial position where the mouse was before starting to drag the element.
				initial_position = GetScreenPos();													//Sets initialPosition with the current position at mouse KEY_DOWN.
				is_drag_target = true;
				App->gui_manager->focused_element = this;													//Set the focus on the Input Box element when it is clicked.
			}

			if (!IsHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_DOWN)		//Unfocus when the mouse clicks outside the input box.
			{
				if (App->gui_manager->focused_element == this)
				{
					App->gui_manager->focused_element = nullptr;
				}
			}

			if (is_draggable)
			{
				if ((IsHovered() || is_drag_target) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_REPEAT)		//If the mouse is on the text and the left mouse button is being pressed.
				{
					if (IsForemostElement() || is_drag_target)											//If the UI Text element is the foremost element under the mouse. 
					{
						ui_event = UI_EVENT::CLICKED;

						if (ElementCanBeDragged())														//If the UI Text element is draggable and is the foremost element under the mouse. 
						{
							DragElement();																//The Text element is dragged.

							CheckElementChilds();														//Checks if this Text element has any childs and updates them in consequence.

							previous_mouse_position = GetMousePos();									//Updates prevMousePos so it can be dragged again next frame.
						}
					}
				}
			}

			// --- UNCLICKED EVENT (Left Click)
			if ((IsHovered() || is_drag_target) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_UP)	//If the mouse is on the text and the left mouse button is released.
			{
				if (IsForemostElement() && ElementRemainedInPlace())								//If the UI Text element is the foremost element under the mouse and has not been dragged. 
				{
					ui_event = UI_EVENT::UNCLICKED;
				}

				if (is_drag_target)
				{
					is_drag_target = false;
					initial_position = GetScreenPos();
				}

				//currentRect = clicked;															//Button Hover sprite.
			}
		}

		if (is_interactible)
		{
			if (listener != nullptr)
			{
				listener->OnEventCall(this, ui_event);
			}
		}
	}
	/*else
	{
		App->input->ClearTextInput();
		text.DeleteCurrentStringTex();
	}*/
	
	return;
}

void UI_InputBox::CleanUp()
{
	background.CleanUp();
	cursor.CleanUp();
	text.CleanUp();
}

// --- DRAW INPUT BOX ELEMENTS
void UI_InputBox::DrawInputBoxElements()													// --------------------------------------------------------------------------
{
	background.Draw();																		//Calls Background's Draw() method.
	text.Draw();																			//Calls Text's Draw() method.
	DrawCursor();																			//Calls the DrawCursor() method. If the conditions are met, the cursor will be drawn on screen.
}

void UI_InputBox::DrawCursor()																// --------------------------------------------------------------------------
{
	if (IsFocused() && this->is_visible && cursor.is_visible)								//If input box is focused, input box is visible and the cursor is visible.
	{
		App->render->DrawQuad(cursor.GetHitbox()											//Draws a quad on screen with the parameters of the cursor's hitbox and the colour passed as argument.
			, cursor_colour.r
			, cursor_colour.g
			, cursor_colour.b
			, cursor_colour.a
			, true, false);
	}
}
// -----------------------------

// --- GETTERS / SETTERS -------
// --- This method returns the current input text length.
int UI_InputBox::TextLength()																// --------------------------------------------------------------------------
{
	return App->input->GetInputTextLength();												//Gets the current input text's length in characters.
}

int UI_InputBox::GetCurrentCursorIndex()													// --------------------------------------------------------------------------
{
	return App->input->GetCursorIndex();													//Gets the current cursor's index.
}
// -----------------------------

// --- INPUT BOX ELEMENTS STATE
void UI_InputBox::CheckInputBoxState()														// --------------------------------------------------------------------------
{
	UpdateInputBoxElementsPos();															//Calls UpdateInputBoxE...() If input box has changed positions, all its elements will be actualized.
	CheckFocus();																			//Calls CheckFocus(). If the input box has the focus and is visible, text input will be received and sent to Print().

	if (background.is_visible != this->is_visible)
	{
		background.is_visible = this->is_visible;
	}

	if (text.is_visible != this->is_visible)
	{
		text.is_visible = this->is_visible;
	}
}

void UI_InputBox::UpdateInputBoxElementsPos()												// --------------------------------------------------------------------------
{
	if (this->GetScreenPos() != this->initial_position)										//If the input box has changed positions. (Dragged, updated as a child...)
	{
		background.SetScreenPos(background.GetLocalPos() + GetScreenPos());					//The Background element's position will be actualized to match the input box.
		background.SetHitbox({ background.GetScreenPos().x									//The Background element's hitbox position will be actualized to match the input box.
						, background.GetScreenPos().y
						, background.GetHitbox().w
						, background.GetHitbox().h });

		text.SetScreenPos(text.GetLocalPos() + GetScreenPos());								//The Text element's position will be actualized to match the input box.
		text.SetHitbox({   text.GetScreenPos().x											//The Text element's hitbox position will be actualized to match the input box.
						, text.GetScreenPos().y
						, text.GetHitbox().w
						, text.GetHitbox().h });

		cursor.SetScreenPos({ cursor.GetLocalPos() + GetScreenPos() });						//The Cursor element's position will be actualized to match the input box.
		cursor.SetHitbox({ cursor.GetScreenPos().x											//The Cursor element's hitbox position will be actualized to match the input box.
						, cursor.GetScreenPos().y
						, cursor.GetHitbox().w
						, cursor.GetHitbox().h });
	}
}

void UI_InputBox::CheckFocus()																// --------------------------------------------------------------------------
{
	if (IsFocused() && is_visible)															//If the input box has the focus and is visible.
	{
		text.ui_event = UI_EVENT::FOCUSED;													//The text's ui_event will be set to FOCUSED.

		if (blink_frequency != 0.0f)														//If the cursor's blinkFrequency is different than 0;
		{
			blink_timer += App->GetDt();													//Accumulates dt on blinkTimer.
			
			if (blink_timer >= blink_frequency)												//If blink timer is larger or equal to blinkFrequency. blinkFrequency is the blink limit in seconds.
			{
				cursor.is_visible = !cursor.is_visible;										//Sets the cursor's isVisible bool to true. Shows the cursor on screen.
				blink_timer = 0.0f;															//Resets blinkTimer.
			}
		}
		else
		{
			cursor.is_visible = true;
		}
		
		CheckCursorInputs();																//Calls CheckCursorInputs(). Depending on the input the cursor's position on screen will change.
		RefreshInputText();																	//Calls RefreshInputText(). Sends the received text input to the text element and prints it on screen.
	}
	else
	{
		text.ui_event = UI_EVENT::UNFOCUSED;												//If the input box is not focused or visible, it will lose focus. (ui_event set to UNFOCUSED)
		cursor.is_visible = false;															//Sets the cursor's isVisible bool to false. (The cursor will no longer show on screen.
	}
}

void UI_InputBox::CheckCursorInputs()														// --------------------------------------------------------------------------
{	
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_STATE::KEY_DOWN)									// --------------------------------------------------------------------------
	{
		int prevIndex = GetCurrentCursorIndex();											//Gets the current cursor index. Named prevIndex to improve readability.

		if (cursor_positions[prevIndex] != NULL)											//If the cursor position for the nextIndex is not NULL.
		{
			SetCursorPosWithCursorIndex(prevIndex);											//Sets the cursor's position to the one stored in the cursorPostions[] array for the passed index.
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_STATE::KEY_DOWN)			//See Input				// --------------------------------------------------------------------------
	{
		int nextIndex = GetCurrentCursorIndex();											//Gets the current cursor index. Named nextIndex to improve readability.

		if (cursor_positions[nextIndex] != NULL)											//If the cursor position for the nextIndex is not NULL.
		{
			SetCursorPosWithCursorIndex(nextIndex);											//Sets the cursor's position to the one stored in the cursorPostions[] array for the passed index.
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_STATE::KEY_DOWN)		//See Input				// --------------------------------------------------------------------------
	{
		//currentIndex = GetCurrentCursorIndex();

		//if (GetCurrentCursorIndex() == TextLength())										//Only reset the cursor if the cursor is at the end of the text.
		//{	
		//	SetCursorPosWithTextWidth();

		//	cursorPositions[currentIndex] = cursor.GetHitbox().x;
		//}
		//else
		//{
		//	SetCursorPosWithCursorIndex(currentIndex);
		//}
	}

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_STATE::KEY_DOWN)	//See Input					// --------------------------------------------------------------------------
	{
		current_index = GetCurrentCursorIndex();											//Gets the current cursor index. In this case current index will be 0. See j1Input.
		SetCursorPosWithCursorIndex(current_index);											//Sets the cursor's poition to the one stored in the cursorPositions[] array for the passed index.
		
		for (int i = 0; i < previous_length; i++)											//Cleans Up all previously recorded positions in the cursorPositions[] array.
		{
			cursor_positions[i] = NULL;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_STATE::KEY_DOWN)								// --------------------------------------------------------------------------
	{
		current_index = GetCurrentCursorIndex();											//Gets the current cursor index. In this case current index will be 0. See j1Input.
		SetCursorPosWithCursorIndex(current_index);											//Sets the cursor's poition to the one stored in the cursorPositions[] array for the passed index.

		for (int i = 0; i < previous_length; i++)											//Cleans Up all previously recorded positions in the cursorPositions[] array..
		{
			cursor_positions[i] = NULL;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_HOME) == KEY_STATE::KEY_DOWN)		//See Input					// --------------------------------------------------------------------------
	{
		current_index = GetCurrentCursorIndex();											//Gets the current cursor index. In this case current index will be 0. See j1Input.

		SetCursorPosWithCursorIndex(current_index);											//Sets the cursor's poition to the one stored in the cursorPositions[] array for the passed index.
	}

	if (App->input->GetKey(SDL_SCANCODE_END) == KEY_STATE::KEY_DOWN)		//See Input					// --------------------------------------------------------------------------
	{
		current_index = GetCurrentCursorIndex();											//Gets the current cursor index. In this case current index will be the length of the input_string.
		
		SetCursorPosWithCursorIndex(current_index);											//Sets the cursor's poition to the one stored in the cursorPositions[] array for the passed index.
	}
}

// ------------------------------ REFRESH TEXT AND CURSOR METHODS ------------------------------
void UI_InputBox::RefreshInputText()														// --------------------------------------------------------------------------
{
	text.DeleteCurrentStringTex();															//Sets to NULL the text's currentTex.
	text.RefreshTextInput(App->input->GetInputText());										//Refreshes text's string and texture with the string received from the input.

	if (previous_length != TextLength())													//If there has been a change in the string (Addition, deletion). Takes into account the length of the string in characters and cursor_index.
	{
		RefreshCursorPos();

		previous_length = TextLength();														//As the text's length does not correspond with the cursor index, it is set to the new index.
		LOG("Current Length %d", TextLength());
	}
}

void UI_InputBox::RefreshCursorPos()														// --------------------------------------------------------------------------
{
	current_index = GetCurrentCursorIndex();												//Gets the current cursor index.

	if (current_index == TextLength())														//If cursor is at the text's end.
	{
		SetCursorPosWithTextWidth(App->input->GetInputText());								//Sets the cursor's position as the cursor's origin position + textWidth.
	}
	else
	{
		ResetCursorPositions(current_index);												//Re-sets the cursor positions taking into account the inserted/deleted input text.

		SetCursorPosWithCursorIndex(current_index);											//If cursor is within the text, set the position according to the index.
	}

	cursor_positions[current_index] = cursor.GetHitbox().x;									//The current cursor hitbox's postion in the x axis is stored (cursor.pos().x + textWidth)
}

// --- Sets the cursor's position to the position stored in the cursorPositions array for the index passed as argument.
void UI_InputBox::SetCursorPosWithCursorIndex(int index)									// --------------------------------------------------------------------------
{
	cursor.SetHitbox({ cursor_positions[index]												//Sets the cursor hitbox's  position to the one stored at the cursor's index in the positions array. 
		, cursor.GetScreenPos().y
		, cursor.GetHitbox().w
		, cursor.GetHitbox().h });
}

// --- Sets the cursor's position to the cursor's origin position + textWidth.
void UI_InputBox::SetCursorPosWithTextWidth(const char* string)								// --------------------------------------------------------------------------												
{
	App->font->CalcSize(string, text_width, text_height, font);								//CalcSize calculates the current width and height of the current string/text.
	cursor.SetHitbox({ cursor.GetScreenPos().x + text_width									//Sets the cursor's hitbox's position addind the new textWidth to it.
		, cursor.GetScreenPos().y
		, cursor.GetHitbox().w
		, cursor.GetHitbox().h });
}

void UI_InputBox::ResetCursorPositions(int index)											// --------------------------------------------------------------------------
{
	const char* inputString = App->input->GetInputText();									//Gets the current input string.
	char tmp[MAX_SIZE];																		//Buffer string that will be used to store different chunks of the string.
	
	for (int i = index; i <= TextLength(); i++)												//For i = index (cursor index), while index is less or equal to input_string length in characters.
	{
		for (int j = 0; j < i; j++)															//For j = 0, while j  is less than i.
		{
			tmp[j] = inputString[j];														//Stores in tmp the input string chunk before the current cursor's position.
		}

		tmp[i] = '\0';																		//Every element from [i] to the right (right of the cursor position) is deleted. Cleans the string.

		SetCursorPosWithTextWidth(tmp);														//Sets the new cursor position for the given input_string chunk.
		cursor_positions[i] = cursor.GetHitbox().x;											//Stores the new cursor position for index [i] in the positions array.
	}
}
// -------------------------------------------------------------------------------------------

void UI_InputBox::SetInputBoxVisibility()
{
	if (this->is_visible != background.is_visible || this->is_visible != text.is_visible)
	{
		background.is_visible = this->is_visible;
		text.is_visible = this->is_visible;
	}

	if (this->is_visible != cursor.is_visible && IsFocused())
	{
		cursor.is_visible = this->is_visible;
	}
}
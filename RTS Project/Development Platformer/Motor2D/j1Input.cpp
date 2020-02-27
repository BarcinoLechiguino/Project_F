#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Window.h"
#include "j1Gui.h"
#include "SDL/include/SDL.h"
#include "Brofiler\Brofiler.h"

#define MAX_KEYS 300

j1Input::j1Input() : j1Module()
{
	name.create("input");

	keyboard = new j1KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(j1KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(j1KeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
j1Input::~j1Input()
{
	delete[] keyboard;
}

// Called before render is available
bool j1Input::Awake(pugi::xml_node& config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called before the first frame
bool j1Input::Start()
{
	SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool j1Input::PreUpdate()
{
	BROFILER_CATEGORY("Input PreUpdate", Profiler::Color::Orange);
	static SDL_Event event;
	
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if(mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

	TextInput();

	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
				windowEvents[WE_QUIT] = true;
			break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
					windowEvents[WE_SHOW] = true;
					break;
				}
			break;

			case SDL_TEXTINPUT:

				CheckNewTextInput(event.text.text);

				//LOG("Input String %s", input_text.GetString());

				//for (int i = 0; i < MAX_KEYS; ++i)
				//{
				//	if (keys[i] == 1)
				//	{
				//		/*strcat_s(input_text, 1, event.text.text);
				//		input_text = event.text.text;
				//		LOG("Input String %s", input_text);
				//		
				//		if (keyboard[i] == KEY_DOWN)
				//		{
				//			input_text = event.text.text;
				//			LOG("Input String %s", input_text);
				//		}*/
				//	}
				//}

				break;

			case SDL_TEXTEDITING:
				//composition = event.edit.text;
				//cursor = event.edit.text;
				//selection_len = event.edit.length;
				break;

			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[event.button.button - 1] = KEY_DOWN;
				//LOG("Mouse button %d down", event.button.button-1);
			break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button - 1] = KEY_UP;
				//LOG("Mouse button %d up", event.button.button-1);
			break;

			case SDL_MOUSEWHEEL:
				//event.wheel.direction = SDL_MOUSEWHEEL_FLIPPED;

				mouse_scroll_x = event.wheel.x;			// +X SCROLL RIGHT / -X SCROLL LEFT.
				mouse_scroll_y = event.wheel.y;			// +Y SCROLL UP / -Y SCROLL DOWN.

				break;

			case SDL_MOUSEMOTION:
				int scale = App->win->GetScale();
				mouse_motion_x = event.motion.xrel / scale;
				mouse_motion_y = event.motion.yrel / scale;
				mouse_x = event.motion.x / scale;
				mouse_y = event.motion.y / scale;
				//LOG("Mouse motion x %d y %d", mouse_motion_x, mouse_motion_y);
			break;
		}
	}

	return true;
}

// Called before quitting
bool j1Input::CleanUp()
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

// ---------
bool j1Input::GetWindowEvent(j1EventWindow ev)
{
	return windowEvents[ev];
}

void j1Input::GetMousePosition(int& x, int& y)
{
	x = mouse_x;
	y = mouse_y;
}

void j1Input::GetMouseMotion(int& x, int& y)
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}

void j1Input::GetMousewheelScrolling(int&x, int& y)
{
	x = mouse_scroll_x;
	y = mouse_scroll_y;

	mouse_scroll_x = 0;
	mouse_scroll_y = 0;
}

// ---------------------------- TEXT INPUT METHODS ----------------------------
void j1Input::TextInput()																// -----------------------------------------------------------------------------
{
	if (App->gui->focusedElement != nullptr)											//If the focused element is not NULL.
	{
		if (App->gui->focusedElement->element == UI_Element::INPUTBOX && App->gui->focusedElement->isVisible)	//If the focused element is of the INPUTBOX type and is visible.
		{
			SDL_StartTextInput();														//Start to receive/record text input.
			textInputEnabled = true;													//As text input has started to be received, textInputEnabled is set to true. Manages EditTextInput().
		}
		else
		{
			SDL_StopTextInput();														//In case the focused element is not of the INPUTBOX type, stop text input.
			textInputEnabled = false;													//As text input has stopped being received, textInputEnabled is set to true. Manages EditTextInput().
		}
	}
	else
	{
		SDL_StopTextInput();															//In case the focused element is NULL. Used when right after focusing the inputbox, the focus changes to nullptr
		textInputEnabled = false;														//As text input has stopped being received, textInputEnabled is set to true. Manages EditTextInput().
	}

	if (input_string == nullptr)														// If input_string is NULL.
	{
		Allocate(1);																	// Allocate memory for string of one character.
		ClearTextInput();																// Delete everything in the string (Residual characters may remain in the string after allocation).
		cursorIndex = 0;																// The cursor's index is initialized to 0.
		prevLength = 0;																	// prevLength is initialized to 0.
	}

	if (textInputEnabled)																// If text input is enabled (SDL_StartTextInput() is activated).
	{
		EditTextInputs();																// Calls the edit text input method.
	}
}

void j1Input::EditTextInputs()															// -----------------------------------------------------------------------------													
{
	if (GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)										// -----------------------------------------------------------------------------
	{
		if (strlen(input_string) != 0)													// If the length of input_string in characters is different than 0.
		{
			if (cursorIndex == strlen(input_string))									// If the cursor's index is the same as the length of input_string in characters.
			{
				DeleteTextInput(strlen(input_string));									// Deletes the last character of input_string.
				cursorIndex--;															// As there is now a character less, the cursor's index is reduced in 1.
			}
			else
			{
				if (cursorIndex != 0)													// If the cursor's index is different than 0.
				{
					char* tmp = GetCutText(cursorIndex - 1, cursorIndex - 1, false, true);	// Stores the chunk of the string to the right of the cursor's position.

					DeleteTextInput(cursorIndex);										// Deletes the character to the left of the cursor. (Along with the chunk to the left of the cursor)
					AddTextInput(tmp);													// The stored chunk is added back to input_string.

					cursorIndex--;														// As there is now a character less in input_string, the cursor's index is reduced in 1.

					//delete[] tmp2;
				}
			}
		}
	}

	if (GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)										// -----------------------------------------------------------------------------
	{
		//App->win->SetTitle(input_string);												// Sets the title with the current string.
		//cursorIndex = 0;																// The cursor's index is set to 0 (Origin of the string).
		//ClearTextInput();																// Deletes the whole string.
	}

	if (GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)										// -----------------------------------------------------------------------------				
	{
		cursorIndex = 0;																// The cursor's index is set to 0 (Origin of the string).
		ClearTextInput();																// Deletes the whole string.
	}

	if (GetKey(SDL_SCANCODE_HOME) == KEY_DOWN)											// -----------------------------------------------------------------------------
	{
		cursorIndex = 0;																// The cursor's index is set to 0 (Origin of the string).
	}

	if (GetKey(SDL_SCANCODE_END) == KEY_DOWN)											// -----------------------------------------------------------------------------
	{
		cursorIndex = strlen(input_string);												// The cursor's index is set to the length of input_string in characters.
	}

	// --- MOVING THE CURSOR AROUND
	if (GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)											// -----------------------------------------------------------------------------
	{
		if (cursorIndex != 0)															// If the cursor's current index is different than 0 (Origin of the string).
		{
			cursorIndex--;																// cursorIndex is decreased by 1, which at the input received in UI_InputBox will be used to re-set the visual cursor's position.
		}
	}

	if (GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)											// -----------------------------------------------------------------------------
	{
		if (cursorIndex != strlen(input_string))										// If the cursor's current index is different than the length of input_string in characters.
		{
			cursorIndex++;																// cursorIndex is increased by 1, which at the input received in UI_InputBox will be used to re-set the visual cursor's position.
		}
	}
}

const char* j1Input::GetInputText()														// -----------------------------------------------------------------------------
{
	return input_string;																// Returns the current input_string.
}

int j1Input::GetInputTextLength()														// -----------------------------------------------------------------------------
{
	if (input_string != NULL)															// If input_string is not NULL
	{
		return strlen(input_string);													// Returns the current length of input_string in characters.
	}
	else
	{
		return 0;																		// If input_string is NULL, then the retuned value will be 0.
	}
}

int j1Input::GetCursorIndex()															// -----------------------------------------------------------------------------
{
	return cursorIndex;																	// Returns the current cursor index.
}

// --------------------------------- TEXT EDITING METHODS ---------------------------------
void j1Input::CheckNewTextInput(const char* newTextInput)								// -----------------------------------------------------------------------------
{
	if (cursorIndex == strlen(input_string))											// If the cursor is at the end of the string.
	{
		AddTextInput(newTextInput);														// The incoming input text is added to the end of the string.
		cursorIndex++;
	}
	else
	{
		InsertTextInput(newTextInput);													// If the cursor is anywhere else in the string, the incoming input text is added at the cursor' position.
		cursorIndex++;
	}
}

void j1Input::AddTextInput(const char* origin)											// -----------------------------------------------------------------------------
{
	if (origin != NULL)
	{
		text_size = strlen(input_string);												//Calcuates the size of the current string/text.
		unsigned int need_size = strlen(origin) + text_size + 1;						//The size needed to store both the current string and the incoming input text together is calculated.

		if (need_size > text_size)														//If the needed size is bigger than the current one.
		{
			char* tmp = input_string;													//Buffer that stores the current input_string.
			Allocate(need_size);														//The required size is allocated in memory.
			strcpy_s(input_string, need_size, tmp);										//The input string is reallocated back from the buffer to the expanded string.
			//delete[] tmp;
		}

		strcat_s(input_string, text_size, origin);										//The incoming input text is concatenated to the end of the input string.
	}
}

void j1Input::InsertTextInput(const char* origin)										// -----------------------------------------------------------------------------
{
	char tmp[MAX_SIZE] = { *GetCutText(cursorIndex, cursorIndex, false, true) };		//Gets the part of the string to the right of the cursor and stores it.

	int i = (GetInputTextLength() - (GetInputTextLength() - cursorIndex));				//Set the position from where the string will be copied taking into account the cursor's position.
	for (int j = 0; j < (GetInputTextLength() - cursorIndex); j++)						//From the previously set position until the end of the string each character will be stored in tmp.
	{
		tmp[j] = input_string[i];														//Stores input_string's character at index [i] and stores it in tmp's index [j]
		i++;
	}

	DeleteTextInput(cursorIndex + 1);													//Deletes every character that is to the right of the cursor.
	AddTextInput(origin);																//Adds the inputted text to the string.

	AddTextInput(tmp);																	//Adds the previously stored tmp string to restore input_string, only that this time the inputted text passed as argument is in between.
}

bool j1Input::CutInputText(unsigned int begin, unsigned int end)						// -----------------------------------------------------------------------------
{
	uint len = strlen(input_string);													//Stores the current length of input_string.

	if (end >= len || end == 0)															//If the end position index passed as argument is >= to the length of the string or end is == 0.
		end = len - 1;																	//End will be re-set to length - 1.

	if (begin > len || end <= begin)													//If the begin position index is larger than the length or end is less or equal to begin.
		return false;																	//The method returns false as the aforementioned condition make it impossible to  "cut" the text.

	char* p1 = input_string + begin;													//the temporal p1 string will be set with input_string + begin.
	char* p2 = input_string + end + 1;													//the temporal p2 string will be set with input_string + end + 1.

	while (*p1++ = *p2++);																//Passes p2's values to p1 as long as there are any elements left in p2's temporal string.

	return true;
}

char* j1Input::GetCutText(unsigned int begin, unsigned int end, bool returnFirstPart, bool returnLastPart)	// ----------------------------------------------------------
{
	uint len = strlen(input_string);													//Stores the current length of input_string.

	if (end >= len || end == -1)														//If the end position index passed as argument is >= to the length of the string or end is -1.
		end = len - 1;																	//End will be re-set to length - 1.

	if (begin > len || end < begin)														//If the begin position index is larger than the length or end is less than begin.
		return '\0';																	//The method returns '\0' as the aforementioned conditions make the function useless.

	char* p1 = input_string + begin;													//the temporal p1 string will be set with input_string + begin.
	char* p2 = input_string + end + 1;													//the temporal p2 string will be set with input_string + end + 1.

	if (returnFirstPart && !returnLastPart)												//If returnFirstPart is true and returnLastPart false.
	{
		return p1;																		//The method returns p1.
	}

	if (returnLastPart && !returnFirstPart)												//If returnLastPart is true and returnFirstPart false.
	{
		return p2;																		//The method returns p2.
	}

	return input_string;																//If no bool-relying condition is met, then the method returns input_string.
}

bool j1Input::CmpStr(const char* str1, const char* str2)
{
	if (strcmp(str1, str2) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void j1Input::DeleteTextInput(int positionIndex)										// -----------------------------------------------------------------------------
{
	input_string[positionIndex - 1] = '\0';												//Deletes the character to the left of the cursor (at the given index position).
}

void j1Input::ClearTextInput()															// -----------------------------------------------------------------------------
{
	input_string[0] = '\0';																//Deletes all characters of input_string.
	cursorIndex = 0;
}

void j1Input::Allocate(int required_memory)												// -----------------------------------------------------------------------------
{
	text_size = required_memory;														//Stores the required memory passed as argument.
	input_string = new char[text_size];													//Declares a new char[] with the passed size and sets input_string with it.
}
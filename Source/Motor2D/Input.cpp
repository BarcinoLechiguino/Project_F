#include "SDL/include/SDL.h"
#include "SDL/include/SDL_gamecontroller.h"
#include "Brofiler\Brofiler.h"

#include "p2Log.h"
#include "Application.h"
#include "Window.h"
#include "GuiManager.h"
#include "UI.h"

#include "Input.h"

Input::Input() : Module()
{
	name = ("input");

	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, 0, sizeof(KEY_STATE) * MAX_KEYS);										// 0 = KEY_IDLE

	memset(mouse_buttons, 0, sizeof(KEY_STATE) * NUM_MOUSE_BUTTONS);						// 0 = KEY_IDLE

	// --- INITIALIZING THE GAME CONTROLLER VARIABLES ---
	game_controller.buttons = new BUTTON_STATE[NUM_CONTROLLER_BUTTONS];
	memset(game_controller.buttons, 0, sizeof(BUTTON_STATE) * NUM_CONTROLLER_BUTTONS);		// 0 = BUTTON_IDLE

	game_controller.triggers = new BUTTON_STATE[NUM_CONTROLLER_TRIGGERS];					// Only 2 triggers are supported for a given game controller.
	memset(game_controller.triggers, 0, sizeof(BUTTON_STATE) * NUM_CONTROLLER_TRIGGERS);	// 0 = BUTTON_IDLE

	game_controller.axis = new AXIS_STATE[NUM_CONTROLLER_AXIS];								// Only 4 axis are supported for a given game controller.
	memset(game_controller.axis, 0, sizeof(AXIS_STATE) * NUM_CONTROLLER_AXIS);				// 0 = AXIS_IDLE

	game_controller.id = nullptr;
	game_controller.index = CONTROLLER_INDEX;
	game_controller.max_axis_input_threshold = 0.5f;
	game_controller.min_axis_input_threshold = 0.2f;
}

// Destructor
Input::~Input()
{
	delete[] keyboard;
}

// Called before render is available
bool Input::Awake(pugi::xml_node& config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)											//Will initialize the game controller subsystem as well as the joystick subsystem.
	{
		LOG("SDL_GAMECONTROLLER could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	SDL_GameControllerEventState(SDL_ENABLE);													//Enable/Disable events dealing with Game Controllers. SDL_QUERY and SDL_ENABLE/IGNORE.

	if (SDL_GameControllerEventState(SDL_QUERY) != SDL_ENABLE)									//Checking the state of the game controller event. QUERY will return 1 on ENABLE and 0 on IGNORE.
	{
		LOG("The Controller Event State could not be enabled! SDL_Error: %s\n", SDL_GetError());
		ret = false;		// CARE. CAN CLOSE THE APPLICATION.
	}

	return ret;
}

// Called before the first frame
bool Input::Start()
{
	SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool Input::PreUpdate()
{
	BROFILER_CATEGORY("Input PreUpdate", Profiler::Color::Orange);
	static SDL_Event event;
	
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == SDL_PRESSED)																	// If a keyboard key was pressed. 1 represents PRESSED and 0 represents RELEASED.
		{
			if (keyboard[i] == KEY_STATE::KEY_IDLE)
			{
				keyboard[i] = KEY_STATE::KEY_DOWN;
			}
			else
			{
				keyboard[i] = KEY_STATE::KEY_REPEAT;
			}
		}
		else
		{
			if (keyboard[i] == KEY_STATE::KEY_REPEAT || keyboard[i] == KEY_STATE::KEY_DOWN)
			{
				keyboard[i] = KEY_STATE::KEY_UP;
			}
			else
			{
				keyboard[i] = KEY_STATE::KEY_IDLE;
			}
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouse_buttons[i] == KEY_STATE::KEY_DOWN)
		{
			mouse_buttons[i] = KEY_STATE::KEY_REPEAT;
		}

		if(mouse_buttons[i] == KEY_STATE::KEY_UP)
		{
			mouse_buttons[i] = KEY_STATE::KEY_IDLE;
		}
	}

	/*if (game_controller.id != nullptr)
	{
		CheckGameControllerState();
	}*/

	TextInput();

	int scale = (int)App->win->GetScale();

	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
				LOG("EVEN IS SDL_QUIT!");
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

			//case SDL_TEXTINPUT:

			//	CheckNewTextInput(event.text.text);

			//	break;

			//case SDL_TEXTEDITING:
			//	//composition = event.edit.text;
			//	//cursor = event.edit.text;
			//	//selection_len = event.edit.length;
			//	break;

			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[event.button.button - 1] = KEY_STATE::KEY_DOWN;
				//LOG("Mouse button %d down", event.button.button-1);
			break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button - 1] = KEY_STATE::KEY_UP;
				//LOG("Mouse button %d up", event.button.button-1);
			break;

			case SDL_MOUSEWHEEL:
				//event.wheel.direction = SDL_MOUSEWHEEL_FLIPPED;

				mouse_scroll_x = event.wheel.x;			// +X SCROLL RIGHT / -X SCROLL LEFT.
				mouse_scroll_y = event.wheel.y;			// +Y SCROLL UP / -Y SCROLL DOWN.

				break;

			case SDL_MOUSEMOTION:
				
				mouse_motion_x = event.motion.xrel / scale;
				mouse_motion_y = event.motion.yrel / scale;
				mouse_x = event.motion.x / scale;
				mouse_y = event.motion.y / scale;
				//LOG("Mouse motion x %d y %d", mouse_motion_x, mouse_motion_y);
			break;

			case SDL_CONTROLLERDEVICEADDED:
				if (game_controller.id == nullptr)											// The game_controller has not been initialized/assigned an identifier.
				{
					if (SDL_NumJoysticks() > 0)												// > 0 as SDL_NumJoysticks will return a negative error code (number) on failure.
					{
						if (SDL_IsGameController(CONTROLLER_INDEX))							// Will return SDL_TRUE if the given joystick is supported by the game controller interface.
						{
							game_controller.id = SDL_GameControllerOpen(CONTROLLER_INDEX);	// Will open a game controller for use. Will return the game controller identifier pointer.
						}
					}
					else
					{
						LOG("No joysticks currently attached to the system. SDL_Error: %s\n", SDL_GetError());
					}
				}

			break;
		}
	}

	if (SDL_GameControllerGetAttached(game_controller.id))														// Returns true if the given game controller is open and currently connected.
	{
		// --- GAME CONTROLLER BUTTON STATES ---
		for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)														// Will iterate through all the buttons available. Same as the keyboard key loop.
		{
			if (SDL_GameControllerGetButton(game_controller.id, SDL_GameControllerButton(i)) == SDL_PRESSED)	// Gets the current state of a button in a controller. 1 = PRESSED / 0 = RELEASED.
			{
				if (game_controller.buttons[i] == BUTTON_STATE::BUTTON_IDLE)
				{
					game_controller.buttons[i] = BUTTON_STATE::BUTTON_DOWN;
				}
				else
				{
					game_controller.buttons[i] = BUTTON_STATE::BUTTON_REPEAT;
				}
			}
			else
			{
				if (game_controller.buttons[i] == BUTTON_STATE::BUTTON_DOWN || game_controller.buttons[i] == BUTTON_STATE::BUTTON_REPEAT)
				{
					game_controller.buttons[i] = BUTTON_STATE::BUTTON_UP;
				}
				else
				{
					game_controller.buttons[i] = BUTTON_STATE::BUTTON_IDLE;
				}
			}
		}

		// --- GAME CONTROLLER TRIGGER STATES ---
		for (int i = 0; i < NUM_CONTROLLER_TRIGGERS; ++i)
		{
			int trigger_value = SDL_GameControllerGetAxis(game_controller.id, SDL_GameControllerAxis(TRIGGER_INDEX + i));
			//LOG("CURRENT AXIS VALUE IS: %d", trigger_value);

			if (trigger_value > game_controller.max_axis_input_threshold * MAX_AXIS)
			{
				if (game_controller.triggers[i] == BUTTON_STATE::BUTTON_IDLE)
				{
					game_controller.triggers[i] = BUTTON_STATE::BUTTON_DOWN;
				}
				else
				{
					game_controller.triggers[i] = BUTTON_STATE::BUTTON_REPEAT;
				}
			}
			else
			{
				if (game_controller.triggers[i] == BUTTON_STATE::BUTTON_DOWN || game_controller.triggers[i] == BUTTON_STATE::BUTTON_REPEAT)
				{
					game_controller.triggers[i] = BUTTON_STATE::BUTTON_UP;
				}
				else
				{
					game_controller.triggers[i] = BUTTON_STATE::BUTTON_IDLE;
				}
			}
		}

		// --- GAME CONTROLLER AXIS STATES ---
		int max_positive_threshold = (int)(game_controller.max_axis_input_threshold * MAX_AXIS);			//Maybe make it part of the GameController struct.
		int max_negative_threshold = -(int)(game_controller.max_axis_input_threshold * MAX_AXIS);
		int min_positive_threshold = (int)(game_controller.min_axis_input_threshold * MAX_AXIS);
		int min_negative_threshold = -(int)(game_controller.min_axis_input_threshold * MAX_AXIS);

		for (int i = 0; i < NUM_CONTROLLER_AXIS; ++i)
		{
			int axis_value = SDL_GameControllerGetAxis(game_controller.id, SDL_GameControllerAxis(i));		//Getting the current axis value of the given joystick.

			if (abs(axis_value) < (int)(game_controller.min_axis_input_threshold * MAX_AXIS))
			{
				axis_value = 0;																				// Safety check to compensate for the controller's joystick dead value.
			}

			//LOG("CURRENT AXIS VALUE IS: %d", axis_value);

			if (axis_value > max_positive_threshold)														// The joystick is all the way into the positive (BOTTOM / RIGHT).
			{
				if (game_controller.axis[i] == AXIS_STATE::AXIS_IDLE)
				{
					game_controller.axis[i] = AXIS_STATE::POSITIVE_AXIS_DOWN;
				}
				else
				{
					game_controller.axis[i] = AXIS_STATE::POSITIVE_AXIS_REPEAT;
				}
			}
			else
			{
				if (axis_value < min_positive_threshold && axis_value > min_negative_threshold)			// The joystick is within the min threshold. (No negative axis input)
				{
					if (game_controller.axis[i] == AXIS_STATE::POSITIVE_AXIS_DOWN || game_controller.axis[i] == AXIS_STATE::POSITIVE_AXIS_REPEAT)
					{
						game_controller.axis[i] = AXIS_STATE::POSITIVE_AXIS_RELEASE;
					}
					else
					{
						game_controller.axis[i] = AXIS_STATE::AXIS_IDLE;
					}
				}
			}

			if (axis_value < max_negative_threshold)														// The joystick is all the way into the negative (TOP / LEFT).
			{
				if (game_controller.axis[i] == AXIS_STATE::AXIS_IDLE)
				{
					game_controller.axis[i] = AXIS_STATE::NEGATIVE_AXIS_DOWN;
				}
				else
				{
					game_controller.axis[i] = AXIS_STATE::NEGATIVE_AXIS_REPEAT;
				}
			}
			else if (axis_value > min_negative_threshold && axis_value < min_positive_threshold)													// The joystick is not past the positive threshold. (No positive axis input)
			{
				if (game_controller.axis[i] == AXIS_STATE::NEGATIVE_AXIS_DOWN || game_controller.axis[i] == AXIS_STATE::NEGATIVE_AXIS_REPEAT)
				{
					game_controller.axis[i] = AXIS_STATE::NEGATIVE_AXIS_RELEASE;
				}
				else
				{
					game_controller.axis[i] = AXIS_STATE::AXIS_IDLE;
				}
			}
		}
	}
	else																											// If the game controller has been disconnected.
	{
		if (game_controller.id != nullptr)																			// There was a controller that was open, attached and had an ID
		{
			SDL_GameControllerClose(game_controller.id);
			game_controller.id = nullptr;
		}
	}

	return true;
}

// Called before quitting
bool Input::CleanUp()
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

// ---------
bool Input::GetWindowEvent(EventWindow ev)
{
	return windowEvents[ev];
}

KEY_STATE Input::GetKey(int id) const
{
	return keyboard[id];
}

KEY_STATE Input::GetMouseButtonDown(int id) const
{
	return mouse_buttons[id - 1];
}

BUTTON_STATE Input::GetGameControllerButton(int id) const
{
	if (game_controller.id != nullptr)
	{
		return game_controller.buttons[id];
	}

	return BUTTON_STATE::UNKNOWN_BUTTON;
}

BUTTON_STATE Input::GetGameControllerTrigger(int id) const
{
	if (game_controller.id != nullptr)
	{
		return game_controller.triggers[id];
	}

	return BUTTON_STATE::UNKNOWN_BUTTON;
}

AXIS_STATE Input::GetGameControllerAxis(int id) const
{
	if (game_controller.id != nullptr)
	{
		return game_controller.axis[id];
	}

	return AXIS_STATE::UNKNOWN_AXIS;
}

// ---------------- MOUSE INPUT METHODS ----------------
void Input::GetMousePosition(int& x, int& y)
{
	x = mouse_x;
	y = mouse_y;
}

void Input::GetMouseMotion(int& x, int& y)
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}

void Input::GetMousewheelScrolling(int&x, int& y)
{
	x = mouse_scroll_x;
	y = mouse_scroll_y;

	mouse_scroll_x = 0;
	mouse_scroll_y = 0;
}

// ---------------- GAME CONTROLLER INPUT METHODS ----------------
void Input::CheckGameControllerState()
{
	if (SDL_GameControllerGetAttached(game_controller.id))														// Returns true if the given game controller is open and currently connected.
	{
		// --- GAME CONTROLLER BUTTON STATES ---
		for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)														// Will iterate through all the buttons available. Same as the keyboard key loop.
		{
			if (SDL_GameControllerGetButton(game_controller.id, SDL_GameControllerButton(i)) == SDL_PRESSED)	// Gets the current state of a button in a controller. 1 = PRESSED / 0 = RELEASED.
			{
				if (game_controller.buttons[i] == BUTTON_STATE::BUTTON_IDLE)
				{
					game_controller.buttons[i] = BUTTON_STATE::BUTTON_DOWN;
				}
				else
				{
					game_controller.buttons[i] = BUTTON_STATE::BUTTON_REPEAT;
				}
			}
			else
			{
				if (game_controller.buttons[i] == BUTTON_STATE::BUTTON_DOWN || game_controller.buttons[i] == BUTTON_STATE::BUTTON_REPEAT)
				{
					game_controller.buttons[i] = BUTTON_STATE::BUTTON_UP;
				}
				else
				{
					game_controller.buttons[i] = BUTTON_STATE::BUTTON_IDLE;
				}
			}
		}

		// --- GAME CONTROLLER TRIGGER STATES ---
		for (int i = 0; i < NUM_CONTROLLER_TRIGGERS; ++i)
		{
			int trigger_value = SDL_GameControllerGetAxis(game_controller.id, SDL_GameControllerAxis(TRIGGER_INDEX + i));
			//LOG("CURRENT AXIS VALUE IS: %d", trigger_value);

			if (trigger_value > game_controller.max_axis_input_threshold * MAX_AXIS)
			{
				if (game_controller.triggers[i] == BUTTON_STATE::BUTTON_IDLE)
				{
					game_controller.triggers[i] = BUTTON_STATE::BUTTON_DOWN;
				}
				else
				{
					game_controller.triggers[i] = BUTTON_STATE::BUTTON_REPEAT;
				}
			}
			else
			{
				if (game_controller.triggers[i] == BUTTON_STATE::BUTTON_DOWN || game_controller.triggers[i] == BUTTON_STATE::BUTTON_REPEAT)
				{
					game_controller.triggers[i] = BUTTON_STATE::BUTTON_UP;
				}
				else
				{
					game_controller.triggers[i] = BUTTON_STATE::BUTTON_IDLE;
				}
			}
		}

		// --- GAME CONTROLLER AXIS STATES ---
		int max_positive_threshold = (int)(game_controller.max_axis_input_threshold * MAX_AXIS);			//Maybe make it part of the GameController struct.
		int max_negative_threshold = -(int)(game_controller.max_axis_input_threshold * MAX_AXIS);
		int min_positive_threshold = (int)(game_controller.min_axis_input_threshold * MAX_AXIS);
		int min_negative_threshold = -(int)(game_controller.min_axis_input_threshold * MAX_AXIS);

		for (int i = 0; i < NUM_CONTROLLER_AXIS; ++i)
		{
			int axis_value = SDL_GameControllerGetAxis(game_controller.id, SDL_GameControllerAxis(i));		//Getting the current axis value of the given joystick.

			if (abs(axis_value) < game_controller.min_axis_input_threshold * MAX_AXIS)
			{
				axis_value = 0;																				// Safety check to compensate for the controller's joystick dead value.
			}

			LOG("CURRENT AXIS VALUE IS: %d", axis_value);

			if (axis_value > max_positive_threshold)														// The joystick is all the way into the positive (BOTTOM / RIGHT).
			{
				if (game_controller.axis[i] == AXIS_STATE::AXIS_IDLE)
				{
					game_controller.axis[i] = AXIS_STATE::POSITIVE_AXIS_DOWN;
				}
				else
				{
					game_controller.axis[i] = AXIS_STATE::POSITIVE_AXIS_REPEAT;
				}
			}
			else
			{
				if (axis_value < min_positive_threshold && axis_value > min_negative_threshold)			// The joystick is within the min threshold. (No negative axis input)
				{
					if (game_controller.axis[i] == AXIS_STATE::POSITIVE_AXIS_DOWN || game_controller.axis[i] == AXIS_STATE::POSITIVE_AXIS_REPEAT)
					{
						game_controller.axis[i] = AXIS_STATE::POSITIVE_AXIS_RELEASE;
					}
					else
					{
						game_controller.axis[i] = AXIS_STATE::AXIS_IDLE;
					}
				}
			}

			if (axis_value < max_negative_threshold)														// The joystick is all the way into the negative (TOP / LEFT).
			{
				if (game_controller.axis[i] == AXIS_STATE::AXIS_IDLE)
				{
					game_controller.axis[i] = AXIS_STATE::NEGATIVE_AXIS_DOWN;
				}
				else
				{
					game_controller.axis[i] = AXIS_STATE::NEGATIVE_AXIS_REPEAT;
				}
			}
			else if (axis_value > min_negative_threshold && axis_value < min_positive_threshold)													// The joystick is not past the positive threshold. (No positive axis input)
			{
				if (game_controller.axis[i] == AXIS_STATE::NEGATIVE_AXIS_DOWN || game_controller.axis[i] == AXIS_STATE::NEGATIVE_AXIS_REPEAT)
				{
					game_controller.axis[i] = AXIS_STATE::NEGATIVE_AXIS_RELEASE;
				}
				else
				{
					game_controller.axis[i] = AXIS_STATE::AXIS_IDLE;
				}
			}
		}
	}
	else																											// If the game controller has been disconnected.
	{
		if (game_controller.id != nullptr)																			// There was a controller that was open, attached and had an ID
		{
			SDL_GameControllerClose(game_controller.id);
			game_controller.id = nullptr;
		}
	}
}

// ---------------------------- TEXT INPUT METHODS ----------------------------
void Input::TextInput()
{
	/*if (App->gui->focusedElement != nullptr)											
	{
		if (App->gui->focusedElement->element == UI_ELEMENT::INPUTBOX && App->gui->focusedElement->isVisible)
		{
			SDL_StartTextInput();														
			text_input_is_enabled = true;													
		}
		else
		{
			SDL_StopTextInput();														
			text_input_is_enabled = false;													
		}
	}
	else
	{
		SDL_StopTextInput();
		text_input_is_enabled = false;
	}

	if (input_string == nullptr)														
	{
		Allocate(1);
		ClearTextInput();
		cursor_index = 0;
		prevLength = 0;
	}

	if (text_input_is_enabled)																
	{
		EditTextInputs();
	}*/
}

void Input::EditTextInputs()										
{
	if (GetKey(SDL_SCANCODE_BACKSPACE) == KEY_STATE::KEY_DOWN)										// Delete character
	{
		if (strlen(input_string) != 0)
		{
			if (cursor_index == strlen(input_string))									// Cursor in end of char
			{
				DeleteTextInput(strlen(input_string));							
				cursor_index--;														
			}
			else
			{
				if (cursor_index != 0)													// Cursor inside char
				{
					char* tmp = GetCutText(cursor_index - 1, cursor_index - 1, false, true);	// Stores the chunk of the string to the right of the cursor's position.

					DeleteTextInput(cursor_index);										
					AddTextInput(tmp);													

					cursor_index--;														

					//delete[] tmp2;
				}
			}
		}
	}

	if (GetKey(SDL_SCANCODE_RETURN) == KEY_STATE::KEY_DOWN)
	{
		//App->win->SetTitle(input_string);												// Sets the title with the current string.
		//cursor_index = 0;																// The cursor's index is set to 0 (Origin of the string).
		//ClearTextInput();																// Deletes the whole string.
	}

	if (GetKey(SDL_SCANCODE_DELETE) == KEY_STATE::KEY_DOWN)										//Erase all text			
	{
		cursor_index = 0;
		ClearTextInput();																
	}

	if (GetKey(SDL_SCANCODE_HOME) == KEY_STATE::KEY_DOWN)											// Return to start
	{
		cursor_index = 0;																
	}

	if (GetKey(SDL_SCANCODE_END) == KEY_STATE::KEY_DOWN)											// Cursor to end
	{
		cursor_index = strlen(input_string);												
	}

	// --- MOVING THE CURSOR AROUND
	if (GetKey(SDL_SCANCODE_LEFT) == KEY_STATE::KEY_DOWN)											// Move cursor left
	{
		if (cursor_index != 0)															
		{
			cursor_index--;																
		}
	}

	if (GetKey(SDL_SCANCODE_RIGHT) == KEY_STATE::KEY_DOWN)											// Move cursor right
	{
		if (cursor_index != strlen(input_string))										
		{
			cursor_index++;																
		}
	}
}

const char* Input::GetInputText()														
{
	return input_string;																
}

int Input::GetInputTextLength()														
{
	if (input_string != nullptr)															
	{
		return strlen(input_string);													
	}
	else
	{
		return 0;																		
	}
}

int Input::GetCursorIndex()
{
	return cursor_index;
}

// TEXT EDITING METHODS
void Input::CheckNewTextInput(const char* newTextInput)								// -----------------------------------------------------------------------------
{
	if (cursor_index == strlen(input_string))											// If the cursor is at the end of the string.
	{
		AddTextInput(newTextInput);														// The incoming input text is added to the end of the string.
		cursor_index++;
	}
	else
	{
		InsertTextInput(newTextInput);													// If the cursor is anywhere else in the string, the incoming input text is added at the cursor' position.
		cursor_index++;
	}
}

void Input::AddTextInput(const char* origin)											// -----------------------------------------------------------------------------
{
	if (origin != NULL)
	{
		text_size = strlen(input_string);												//Calcuates the size of the current string/text.
		unsigned int need_size = strlen(origin) + text_size + 1;						//The size needed to store both the current string and the incoming input text together is calculated.

		if ((int)need_size > text_size)														//If the needed size is bigger than the current one.
		{
			char* tmp = input_string;													//Buffer that stores the current input_string.
			Allocate(need_size);														//The required size is allocated in memory.
			strcpy_s(input_string, need_size, tmp);										//The input string is reallocated back from the buffer to the expanded string.
			//delete[] tmp;
		}

		strcat_s(input_string, text_size, origin);										//The incoming input text is concatenated to the end of the input string.
	}
}

void Input::InsertTextInput(const char* origin)										// -----------------------------------------------------------------------------
{
	char tmp[MAX_SIZE] = { *GetCutText(cursor_index, cursor_index, false, true) };		//Gets the part of the string to the right of the cursor and stores it.

	int i = (GetInputTextLength() - (GetInputTextLength() - cursor_index));				//Set the position from where the string will be copied taking into account the cursor's position.
	for (int j = 0; j < (GetInputTextLength() - cursor_index); j++)						//From the previously set position until the end of the string each character will be stored in tmp.
	{
		tmp[j] = input_string[i];														//Stores input_string's character at index [i] and stores it in tmp's index [j]
		i++;
	}

	DeleteTextInput(cursor_index + 1);													//Deletes every character that is to the right of the cursor.
	AddTextInput(origin);																//Adds the inputted text to the string.

	AddTextInput(tmp);																	//Adds the previously stored tmp string to restore input_string, only that this time the inputted text passed as argument is in between.
}

bool Input::CutInputText(unsigned int begin, unsigned int end)						// -----------------------------------------------------------------------------
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

char* Input::GetCutText(unsigned int begin, unsigned int end, bool returnFirstPart, bool returnLastPart)	// ----------------------------------------------------------
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

bool Input::CmpStr(const char* str1, const char* str2)
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

void Input::DeleteTextInput(int positionIndex)										// -----------------------------------------------------------------------------
{
	input_string[positionIndex - 1] = '\0';												//Deletes the character to the left of the cursor (at the given index position).
}

void Input::ClearTextInput()															// -----------------------------------------------------------------------------
{
	input_string[0] = '\0';																//Deletes all characters of input_string.
	cursor_index = 0;
}

void Input::Allocate(int required_memory)												// -----------------------------------------------------------------------------
{
	text_size = required_memory;														//Stores the required memory passed as argument.
	input_string = new char[text_size];													//Declares a new char[] with the passed size and sets input_string with it.
}
#ifndef __INPUT_H__
#define __INPUT_H__

#include "Module.h"

//#define NUM_KEYS 352
//#define LAST_KEYS_PRESSED_BUFFER 50

#define MAX_KEYS 300

#define NUM_MOUSE_BUTTONS 5
#define NUM_CONTROLLER_BUTTONS 15
#define NUM_CONTROLLER_AXIS 4
#define NUM_CONTROLLER_TRIGGERS 2

#define MAX_AXIS 32767								// Absolute maximum axis value that can be returned by a given controller axis.
#define CONTROLLER_INDEX 0							// Added to increase readability. As only one controller will be supported the controller index will always be 0.
#define TRIGGER_INDEX 4								// The trigger axis are the 5th and 6th element of the SDL_GameControllerAxis, so their index starts at 4.

#define LEFT_TRIGGER 0								// Own macros to support the implementation of the triggers as if they were buttons.
#define RIGHT_TRIGGER 1								// Triggers have 0 and 1 as their game_controller.trigger index.

#define MAX_SIZE 1000

struct SDL_Rect;
struct _SDL_GameController;

enum EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum class KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

enum class BUTTON_STATE								// Struct implemented to improve the readability of the code and separate keys from buttons. Same as KEY_STATE.
{
	BUTTON_IDLE,
	BUTTON_DOWN,
	BUTTON_REPEAT,
	BUTTON_UP,
	UNKNOWN_BUTTON									// State that will be used in case the GetGameControllerButton/Trigger is called and there is no controller currently connected.
};

enum class AXIS_STATE								// Struct implemented to add a "button" state to a given axis value.
{
	AXIS_IDLE,
	POSITIVE_AXIS_DOWN,
	POSITIVE_AXIS_REPEAT,
	POSITIVE_AXIS_RELEASE,
	NEGATIVE_AXIS_DOWN,
	NEGATIVE_AXIS_REPEAT,
	NEGATIVE_AXIS_RELEASE,
	UNKNOWN_AXIS									// State that will be used in case the GetGameControllerAxis is called and there is no controller currently connected.
};

enum class BUTTON_BINDINGS
{
	SELECT_GATHERERS,
	SELECT_SCOUTS,
	SELECT_INFANTRIES,
	SELECT_HEAVYS,
	SELECT_TOWNHALL,
	SELECT_BARRACKS
};

struct GameController
{
	_SDL_GameController*	id;							// The controller id pointer. Will designate which controller is sending the event and so on.
	int						index;						// The index will always be 0 as only one controller will be supported. (Singleplayer game)
														
	BUTTON_STATE*			buttons;					
	BUTTON_STATE*			triggers;					// Although the controller triggers are inside the SDL_GameControllerAxis structure, they will be treated as if they were buttons.
	AXIS_STATE*				axis;					
													
	float					max_axis_input_threshold;	// Limit before any given axis input is detected. Goes from 0.0f to 1.0f.
	float					min_axis_input_threshold;	// Limit before the release/idle axis input is detected. Goes from 0.0f to 1.0f.
};

class Input : public Module
{
public:

	Input();
	virtual ~Input();											// Destructor

	bool Awake(pugi::xml_node&);								// Called before render is available
	bool Start();												// Called before the first frame
	bool PreUpdate();											// Called each loop iteration
	bool CleanUp();												// Called before quitting

public:
	bool GetWindowEvent(EventWindow ev);						// Gather relevant win events
	bool GetWindowEvent(int code);								// Check if a certain window event happened

	// Check key states (includes mouse and joy).
	KEY_STATE GetKey(int id) const;								// Get Keyboard Key State
	KEY_STATE GetMouseButtonDown(int id) const;					// Get Mouse Button State
	BUTTON_STATE GetGameControllerButton(int id) const;			// Get Game Controller Button State
	BUTTON_STATE GetGameControllerTrigger(int id) const;		// Get Game Controller Trigger State
	AXIS_STATE GetGameControllerAxis(int id) const;				// Get Game Controller Axis State.

	// Mouse (position & axis)
	void GetMousePosition(int &x, int &y);						// Returns the current mouse position.
	void GetMouseMotion(int& x, int& y);						// Returns the current mouse motion value.
	void GetMousewheelScrolling(int&x, int& y);					// Returns the current mousewheel scroll value.

	// Game Controller
	void CheckGameControllerState();							// Will check all the states of all buttons and axis of a game controller. Implemented for readability.


public:
	// Text Input
	void TextInput();
	void EditTextInputs();

	const char* GetInputText();
	int GetInputTextLength();
	int GetCursorIndex();

	void CheckNewTextInput(const char* newTextInput);
	void AddTextInput(const char* origin);
	void InsertTextInput(const char* origin);

	bool CutInputText(unsigned int begin, unsigned int end = 0);
	char* GetCutText(unsigned int begin, unsigned int end = -1, bool returnFirstPart = false, bool returnLastPart = false);

	bool CmpStr(const char* str1, const char* str2);

	void ClearTextInput();
	void DeleteTextInput(int positionIndex);

private:
	void Allocate(int required_memory);

private:
	bool			windowEvents[WE_COUNT];
	
	KEY_STATE*		keyboard;
	KEY_STATE		mouse_buttons[NUM_MOUSE_BUTTONS];

	GameController game_controller;
	
	// --- MOUSE INPUT VARS ---
	int				mouse_motion_x;
	int				mouse_motion_y;
	int				mouse_x;
	int				mouse_y;
	int				mouse_scroll_x;
	int				mouse_scroll_y;

	// --- TEXT INPUT VARS ---
	bool			text_input_is_enabled;
	char*			input_string;
	int				text_size;
	int				previous_length;
	int				cursor_index;
};

#endif // __INPUT_H__
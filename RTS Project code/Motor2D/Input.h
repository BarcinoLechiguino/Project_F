#ifndef __INPUT_H__
#define __INPUT_H__

#include "Module.h"

//#define NUM_KEYS 352
#define NUM_MOUSE_BUTTONS 5
#define NUM_CONTROLLER_BUTTONS 15
#define MAX_AXIS 32767
#define MAX_SIZE 1000
//#define LAST_KEYS_PRESSED_BUFFER 50

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

enum class BUTTON_STATE							// Controller input states. Added to improve readability. Could also be applied to the mouse.
{
	BUTTON_IDLE,
	BUTTON_DOWN,
	BUTTON_REPEAT,
	BUTTON_UP
};

enum class AXIS_STATE							// Controller joystick states. 
{
	AXIS_IDLE,
	AXIS_POSITIVE_DOWN,
	AXIS_POSITIVE_REPEAT,
	AXIS_POSITIVE_UP,
	AXIS_NEGATIVE_DOWN,
	AXIS_NEGATIVE_REPEAT,
	AXIS_NEGATIVE_UP
};

enum class BUTTON_BINDING						// Controller bindings. Implemented to be able to use the selection shortcuts.
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
	_SDL_GameController* controller_id;
	BUTTON_STATE		buttons[NUM_CONTROLLER_BUTTONS];

	//int					axis;
	//AXIS_STATE			axis_state;
};

class Input : public Module
{
public:

	Input();

	// Destructor
	virtual ~Input();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called before quitting
	bool CleanUp();

	// Gather relevant win events
	bool GetWindowEvent(EventWindow ev);

	// Check key states (includes mouse and joy buttons)
	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id - 1];
	}

	KEY_STATE GetControllerButtonDown(int id) const
	{
		return controller_buttons[id - 1];
	}

	// Check if a certain window event happened
	bool GetWindowEvent(int code);

	// Get mouse / axis position
	void GetMousePosition(int &x, int &y);
	void GetMouseMotion(int& x, int& y);
	void GetMousewheelScrolling(int&x, int& y);

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
	bool		windowEvents[WE_COUNT];
	
	KEY_STATE*	keyboard;
	KEY_STATE	mouse_buttons[NUM_MOUSE_BUTTONS];
	KEY_STATE	controller_buttons[NUM_CONTROLLER_BUTTONS];

	GameController game_controller;
	
	// MOUSE INPUT VARS
	int			mouse_motion_x;
	int			mouse_motion_y;
	int			mouse_x;
	int			mouse_y;
	int			mouse_scroll_x;
	int			mouse_scroll_y;

	// TEXT INPUT VARS
	bool		text_input_is_enabled;
	char*		input_string;
	int			text_size;
	int			previous_length;
	int			cursor_index;
};

#endif // __INPUT_H__
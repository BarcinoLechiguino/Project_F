#ifndef __UI_INPUTBOX_H__
#define __UI_INPUTBOX_H__

#include "UI.h"

struct _TTF_Font;

class UI_Image;
class UI_Text;

class UI_InputBox : public UI
{
public:
	UI_InputBox();
	UI_InputBox(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, SDL_Rect cursorSize, SDL_Color cursorColor, iPoint text_offset,
		float blink_frequency = 0.0f, bool is_visible = true, bool is_interactible = true, bool is_draggable = false, Module* listener = nullptr, UI* parent = nullptr,
		std::string* defaultString = nullptr, bool emptyElements = false);

	bool Draw();

	void CheckInput();

	void CleanUp();

public:
	void DrawInputBoxElements();
	void DrawCursor();
	
	void CheckInputBoxState();
	void CheckFocus();

	void UpdateInputBoxElementsPos();
	void SetInputBoxVisibility();

	void CheckCursorInputs();
	void RefreshInputText();
	void RefreshCursorPos();

	void SetCursorPosWithCursorIndex(int index);
	void SetCursorPosWithTextWidth(const char* string);		//
	void ResetCursorPositions(int index);					//If an inputted text is inserted in between 2 characters of the string, this method recalculates the cursor positions.

	int TextLength();										//Gets the current text length.
	int GetCurrentCursorIndex();							//Gets the current cursor index.

private:
	UI_Image		background;								//Background element of the Input Box.
	UI_Text			text;									//Text element of the Input Box.
	UI_Image		cursor;									//Cursor element of the Input Box.

	_TTF_Font*		font;									//Loaded font.
	int				text_width;								//Current width of the text.
	int				text_height;								//Current height of the text.
	iPoint			text_offset;								//Difference in origin position between the background's and the text's.

	SDL_Color		cursor_colour;							//Colour of the cursor.
	float			blink_timer;								//Keeps track of the time (dt). Applied to the blinking of the cursor.
	float			blink_frequency;							//Amount of time that the cursor will spend in one state (visible/not visible). Set to 0 to make it always visible.
	int				previous_length;								//Amount of letters currently in the text.
	int				current_index;							//Each number of the cursor_index corresponds with a letter. First letter: cursor_index = 1...
	int				cursor_positions[MAX_SIZE];				//Array that will keep track of all the positions that the cursor has been in. Each postion correspond with a cursor index.
};

#endif // !__UI_INPUTBOX_H__

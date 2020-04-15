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
	UI_InputBox(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, SDL_Rect cursorSize, SDL_Color cursorColor, iPoint textOffset,
		float blinkFrequency = 0.0f, bool isVisible = true, bool isInteractible = true, bool isDraggable = false, Module* listener = nullptr, UI* parent = nullptr,
		std::string* defaultString = nullptr, bool emptyElements = false);

	bool Draw();

	void CheckInput();

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
	//SDL_Texture*	tex;
	UI_Image		background;								//Background element of the Input Box.
	UI_Text			text;									//Text element of the Input Box.
	UI_Image		cursor;									//Cursor element of the Input Box.

	_TTF_Font*		font;									//Loaded font.
	int				textWidth;								//Current width of the text.
	int				textHeight;								//Current height of the text.
	iPoint			textOffset;								//Difference in origin position between the background's and the text's.

	SDL_Color		cursorColour;							//Colour of the cursor.
	float			blinkTimer;								//Keeps track of the time (dt). Applied to the blinking of the cursor.
	float			blinkFrequency;							//Amount of time that the cursor will spend in one state (visible/not visible). Set to 0 to make it always visible.
	int				prevLength;								//Amount of letters currently in the text.
	int				currentIndex;							//Each number of the cursorIndex corresponds with a letter. First letter: cursorIndex = 1...
	int				cursorPositions[MAX_SIZE];				//Array that will keep track of all the positions that the cursor has been in. Each postion correspond with a cursor index.
};

#endif // !__UI_INPUTBOX_H__

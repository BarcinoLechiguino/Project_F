#ifndef __UI_TEXT_H__
#define __UI_TEXT_H__

#include "p2SString.h"
#include "j1Fonts.h"
#include "UI.h"

class UI_Text : public UI
{
public:
	UI_Text();
	UI_Text(UI_Element element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, bool isVisible = true, bool isInteractible = false, bool isDraggable = false,
		UI* parent = nullptr, p2SString* string = nullptr, p2SString* hoverString = nullptr, p2SString* leftClickString = nullptr, p2SString* rightClickString = nullptr);

	bool Draw();

	void CheckInput();

	void CleanUp();

public:
	SDL_Texture* GetCurrentStringTex();
	void DeleteCurrentStringTex();

	void RefreshTextInput(const char* newString);

private:
	p2SString*		string;							//String of the UI Text element
	_TTF_Font*		font;							//Loaded font.
	SDL_Color		font_colour;					//Colour of the font.
	SDL_Texture*	idleTex;						//Texture for the idle state of the text.
	SDL_Texture*	hoverTex;						//Texture for the hover state of the text.
	SDL_Texture*	leftClickTex;					//Texture for the left-Clicked state of the text.
	SDL_Texture*	rightClickTex;					//Texture for the right-Clicked state of the text.

	SDL_Texture*	inputTextTex;					//Texture that is constantly being refreshed. If text is inputted, this texture will show the text input on screen.

	SDL_Texture*	currentTex;						//Current texture to be blitted. Depends on the input the UI Text element receives.

	SDL_Rect		textRect;
};

#endif // !__UI_TEXT_H__
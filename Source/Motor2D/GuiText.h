#ifndef __GUI_TEXT_H__
#define __GUI_TEXT_H__

#include "GuiElement.h"

struct _TTF_Font;

class GuiText : public GuiElement
{
public:
	GuiText();
	GuiText(GUI_ELEMENT_TYPE type, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, bool is_visible = true, bool is_interactible = false, bool is_draggable = false,
		Module* listener = nullptr, GuiElement* parent = nullptr, std::string* string = nullptr, std::string* hoverString = nullptr, std::string* leftClickString = nullptr, std::string* rightClickString = nullptr);

	bool Draw();
	void CheckInput();
	void CleanUp();

	SDL_Texture* GetTexture() const;

public:
	SDL_Texture* GetCurrentStringTex();				//Overlaps with GetTexture(), erase later (?)
	void DeleteCurrentStringTex();
	std::string* GetString();

	void RefreshTextInput(const char* newString);

private:
	std::string*	string;							//String of the UI Text element
	_TTF_Font*		font;							//Loaded font.
	SDL_Color		font_colour;					//Colour of the font.
	SDL_Texture*	idle_texture;					//Texture for the idle state of the text.
	SDL_Texture*	hover_texture;					//Texture for the hover state of the text.
	SDL_Texture*	left_click_texture;				//Texture for the left-Clicked state of the text.
	SDL_Texture*	right_click_texture;			//Texture for the right-Clicked state of the text.

	SDL_Texture*	input_text_texture;				//Texture that is constantly being refreshed. If text is inputted, this texture will show the text input on screen.

	SDL_Texture*	current_texture;				//Current texture to be blitted. Depends on the input the UI Text element receives.

	SDL_Rect		text_rect;
};

#endif // !__GUI_TEXT_H__
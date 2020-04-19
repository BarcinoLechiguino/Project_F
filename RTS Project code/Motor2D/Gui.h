#ifndef __GUI_H__
#define __GUI_H__

#include "Module.h"
#include "Point.h"

#define CURSOR_WIDTH 2
#define DRAG_LIMIT_OFFSET 1

/*#include "UI.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_InputBox.h"
#include "UI_Scrollbar.h"
#include "UI_Healthbar.h"*/

struct SDL_Color;
struct SDL_Texture;
struct _TTF_Font;

class Entity;

class UI;
enum class UI_ELEMENT;
//enum class UI_EVENT;

class UI_Image;
class UI_Text;
class UI_Button;
class UI_InputBox;
class UI_Scrollbar;
class UI_Healthbar;

class Gui : public Module
{
public:

	Gui();
	virtual ~Gui();											// Destructor

	bool Awake(pugi::xml_node&);							// Called when before render is available
	bool Start();											// Call before first frame
	bool PreUpdate();										// Called before all Updates
	bool PostUpdate();										// Called after all Updates
	bool CleanUp();											// Called before quitting

public:
	/*const*/ SDL_Texture* GetAtlas() const;

	UI* CreateImage(UI_ELEMENT element, int x, int y, SDL_Rect rect, bool isVisible = false, bool isInteractible = false, bool isDraggable = false
				, Module* listener = nullptr, UI* parent = nullptr);

	UI* CreateText(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, bool isVisible = true, bool isInteractible = false, bool isDraggable = false
				, Module* listener = nullptr, UI* parent = nullptr, std::string* string = nullptr
				, std::string* hoverString = nullptr, std::string* leftClickString = nullptr, std::string* rightClickString = nullptr);

	UI* CreateButton(UI_ELEMENT element, int x, int y, bool isVisible = true, bool isInteractible = true, bool isDraggable = false, Module* listener = nullptr, UI* parent = nullptr
				, SDL_Rect* idle = nullptr, SDL_Rect* hover = nullptr, SDL_Rect* clicked = nullptr);

	UI* CreateUI_Window(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, bool isVisible = true, bool isInteractible = false, bool isDraggable = false, UI* parent = nullptr);

	UI* CreateInputBox(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, SDL_Rect cursor, SDL_Color cursorColour, iPoint textOffset
					, float blinkFrequency = 0.0f, bool isVisible = true, bool isInteractible = true, bool isDraggable = false, Module* listener = nullptr, UI* parent = nullptr
					, std::string* defaultString = nullptr, bool emptyElements = false);
	
	UI* CreateScrollbar(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, SDL_Rect thumbSize, iPoint thumbOffset, SDL_Rect dragArea, float dragFactor, bool dragXAxis = false
					, bool dragYAxis = true, bool invertedScrolling = false, bool isVisible = true, bool isInteractible = true, bool isDraggable = false, Module* listener = nullptr
					, UI* parent = nullptr, SDL_Rect* scrollMask = nullptr, iPoint maskOffset = iPoint(0, 0), bool emptyElements = false);

	UI* CreateHealthbar(UI_ELEMENT element, int x, int y, bool is_visible = true, SDL_Rect* healthbar = nullptr, SDL_Rect* background = nullptr
					, Entity* attached_unit = nullptr, Module* listener = nullptr, UI* parent = nullptr);

	
	void DeleteGuiElement(UI* element_to_delete);

																			// ------------------ GUI ELEMENTS MANAGEMENT ------------------
	void OnEventCall(UI* element, UI_EVENT ui_event);						//This function is called whenever an new event happens, it receives the pointer of the element that caused the event and the kind of event it is.
	void PassFocus();														//Method that passes the focus from an interactible and able to focused element to another with the same conditions.
	bool ElementCanBeFocused(UI* focusElement) const;						//If an element fulfills all requirements (is a button or a scrollbar), then this method returns true. Used to filter which UI elements can or cannot have focus.
	
	UI* FirstElementUnderMouse() const;										//Returs the first element under the mouse.
	bool ElementCanBeClicked(UI* clickedElement) const;

	bool ElementHasChilds(UI* parentElement) const;							//Returns true if the element passed as argument has at least one child.
	void UpdateChilds(UI* parentElement);									//Updates all UI Elements that have the element passed as argument as a parent.
	void SetElementsVisibility(UI* parentElement, bool state);				//Enables/Disables the isVisible bool of a UI Element and its childs according to the passed arguments.

	void Debug_UI();														//Shows on screen the different rects that compose the UI Display.

	void CreateGuiCommands();												//Creates all Gui relevant commands.
	void OnCommand(const char* command, const char* subCommand = nullptr);	//Gui OnCommand() method overload.

public:
	UI*							focusedElement;								//Change to list item
	std::vector<UI*>::iterator	iteratedElement;

	bool						escape;										//When this bool is true the game is exited.
	bool						ui_debug;									//When this bool is true, debug mode is activated.
	
	//Command Strings
	const char*					quitCommand;
	const char*					enable_ui_debug;
	const char*					disable_ui_debug;

public:
	uint						new_game_fx;
	uint						options_fx;
	uint						back_fx;
	uint						appear_menu_fx;
	uint						exit_fx;
	uint						standard_fx;
	uint						upgrade_fx;
	uint						recruit_fx;

private:
	SDL_Texture*				atlas;										//Texture of the atlas (UI Spritesheet)
	std::string					atlas_file_name;							//Name of the atlas in the xml file.

	std::vector<UI*>			elements;									//List where all the UI elements in a scene will be stored at.

public:
	bool						game_started = false;						//To know if the user has enter the game
	bool						audioAlreadyLoaded;

};

#endif // __GUI_H__
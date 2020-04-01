#ifndef __GUI_H__
#define __GUI_H__

#include "Module.h"
#include "Textures.h"
#include "UI.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_InputBox.h"
#include "UI_Scrollbar.h"

//class UI;

#define CURSOR_WIDTH 2
#define DRAG_LIMIT_OFFSET 1

class Gui : public Module
{
public:

	Gui();

	// Destructor
	virtual ~Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:
	/*const*/ SDL_Texture* GetAtlas() const;

	UI* CreateImage(UI_Element element, int x, int y, SDL_Rect rect, bool isVisible = false, bool isInteractible = false, bool isDraggable = false
		, Module* listener = nullptr, UI* parent = nullptr);

	UI* CreateText(UI_Element element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, bool isVisible = true, bool isInteractible = false, bool isDraggable = false
		, Module* listener = nullptr, UI* parent = nullptr, std::string* string = nullptr, std::string* hoverString = nullptr, std::string* leftClickString = nullptr, std::string* rightClickString = nullptr);

	UI* CreateButton(UI_Element element, int x, int y, bool isVisible = true, bool isInteractible = true, bool isDraggable = false, Module* listener = nullptr, UI* parent = nullptr
		, SDL_Rect* idle = nullptr, SDL_Rect* hover = nullptr, SDL_Rect* clicked = nullptr);

	UI* CreateUI_Window(UI_Element element, int x, int y, SDL_Rect hitbox, bool isVisible = true, bool isInteractible = false, bool isDraggable = false, UI* parent = nullptr);

	UI* CreateInputBox(UI_Element element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, SDL_Rect cursor, SDL_Color cursorColour, iPoint textOffset
		, float blinkFrequency = 0.0f, bool isVisible = true, bool isInteractible = true, bool isDraggable = false, Module* listener = nullptr, UI* parent = nullptr
		, std::string* defaultString = nullptr, bool emptyElements = false);

	UI* CreateScrollbar(UI_Element element, int x, int y, SDL_Rect hitbox, SDL_Rect thumbSize, iPoint thumbOffset, SDL_Rect dragArea, float dragFactor, bool dragXAxis = false
		, bool dragYAxis = true, bool invertedScrolling = false, bool isVisible = true, bool isInteractible = true, bool isDraggable = false, Module* listener = nullptr, UI* parent = nullptr
		, SDL_Rect* scrollMask = nullptr, iPoint maskOffset = iPoint(0, 0), bool emptyElements = false);

	
	void OnEventCall(UI* element, UI_Event ui_event);						//This function is called whenever an new event happens, it receives the pointer of the element that caused the event and the kind of event it is.
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
	std::list<UI*>::iterator	iteratedElement;

	bool						escape;										//When this bool is true the game is exited.
	bool						ui_debug;									//When this bool is true, debug mode is activated.
	
	//Command Strings
	const char*					quitCommand;
	const char*					enable_ui_debug;
	const char*					disable_ui_debug;

public:
	uint						tab_fx;
	uint						play_fx;
	uint						save_fx;
	uint						exit_fx;
	uint						nav_fx;

private:
	SDL_Texture*				atlas;										//Texture of the atlas (UI Spritesheet)
	std::string					atlas_file_name;							//Name of the atlas in the xml file.

	std::list<UI*>				elements;									//List where all the UI elements in a scene will be stored at.

public:
	bool						game_started = false;						//To know if the user has enter the game
	bool						audioAlreadyLoaded;
};

#endif // __GUI_H__
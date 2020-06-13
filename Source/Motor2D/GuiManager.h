#ifndef __GUI_H__
#define __GUI_H__

#include "Module.h"
#include "Point.h"

#define CURSOR_WIDTH 2
#define DRAG_LIMIT_OFFSET 1

struct SDL_Color;
struct SDL_Texture;
struct _TTF_Font;

class Entity;

class GuiElement;
enum class GUI_ELEMENT_TYPE;

class GuiAnimation;
enum class GUI_ANIMATION_TYPE;

class GuiManager : public Module
{
public:

	GuiManager();
	~GuiManager();															// Destructor

	bool Awake(pugi::xml_node&);											// Called when before render is available
	bool Start();															// Call before first frame
	bool PreUpdate();														// Called before all Updates
	bool PostUpdate();														// Called after all Updates
	bool CleanUp();															// Called before quitting

public:																		//  ------------------ GUI ELEMENT CREATION/DESTRUCTION METHODS ------------------ 
	/*const*/ SDL_Texture* GetAtlas() const;

	GuiElement* CreateImage(GUI_ELEMENT_TYPE type, int x, int y, SDL_Rect rect, bool is_visible = false, bool is_interactible = false, bool is_draggable = false
				, Module* listener = nullptr, GuiElement* parent = nullptr);

	GuiElement* CreateText(GUI_ELEMENT_TYPE type, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color font_colour, bool is_visible = true, bool is_interactible = false, bool is_draggable = false
				, Module* listener = nullptr, GuiElement* parent = nullptr, std::string* string = nullptr
				, std::string* hover_string = nullptr, std::string* left_click_string = nullptr, std::string* right_click_string = nullptr);

	GuiElement* CreateButton(GUI_ELEMENT_TYPE type, int x, int y, bool is_visible = true, bool is_interactible = true, bool is_draggable = false, Module* listener = nullptr, GuiElement* parent = nullptr
				, SDL_Rect* idle = nullptr, SDL_Rect* hover = nullptr, SDL_Rect* clicked = nullptr);

	GuiElement* CreateInputBox(GUI_ELEMENT_TYPE type, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color font_colour, SDL_Rect cursor, SDL_Color cursor_colour, iPoint text_offset
					, float blink_frequency = 0.0f, bool is_visible = true, bool is_interactible = false, bool is_draggable = false, Module* listener = nullptr, GuiElement* parent = nullptr
					, std::string* default_string = nullptr, bool empty_elements = false);
	
	GuiElement* CreateScrollbar(GUI_ELEMENT_TYPE type, int x, int y, SDL_Rect hitbox, SDL_Rect thumb_size, iPoint thumb_offset, SDL_Rect drag_area, float drag_factor, bool drag_x_axis = false
					, bool drag_y_axis = true, bool inverted_scrolling = false, bool is_visible = true, bool is_interactible = true, bool is_draggable = false, Module* listener = nullptr
					, GuiElement* parent = nullptr, SDL_Rect* scroll_mask = nullptr, iPoint mask_offset = iPoint(0, 0), bool empty_elements = false);

	GuiElement* CreateHealthbar(GUI_ELEMENT_TYPE type, int x, int y, bool is_visible = true, SDL_Rect* healthbar = nullptr, SDL_Rect* background = nullptr, Entity* attached_entity = nullptr);

	GuiElement* CreateCreationBar(GUI_ELEMENT_TYPE type, int x, int y, bool is_visible = true, SDL_Rect* creation_bar = nullptr, SDL_Rect* background = nullptr, Entity* attached_entity = nullptr);

	GuiElement* CreateCursor(GUI_ELEMENT_TYPE type, int x, int y, bool is_visible = true, SDL_Rect* idle = nullptr, SDL_Rect* clicked_idle = nullptr
					, SDL_Rect* hover_ally = nullptr, SDL_Rect* hover_enemy = nullptr, SDL_Rect* hover_resource = nullptr, SDL_Rect* hover_UI = nullptr
					, SDL_Rect* clicked_ally = nullptr, SDL_Rect* clicked_enemy = nullptr, SDL_Rect* clicked_resource = nullptr, SDL_Rect* clicked_UI = nullptr);

	
	void DestroyGuiElements();
	void DeleteGuiElement(GuiElement* element_to_delete);

	void LoadGuiFonts();													//  Fonts do not need to be UnLoaded, they are all cleaned up at the Fonts module CleanUp().

	void LoadGuiElementsAudio();
	void UnLoadGuiElementsAudio();

public:																		// -------- GUI ELEMENT ANIMATION CREATION/DESTRUCTION METHODS --------
	GuiAnimation* CreateFadeAnimation(GuiElement* element, float animation_duration, bool hide_on_completion, float start_alpha, float end_alpha);
	GuiAnimation* CreateSlideAnimation(GuiElement* element, float animation_duration, bool hide_on_completion, iPoint initial_position, iPoint final_position);

	void DeleteUIAnimation(GuiAnimation* ui_animation_to_delete);							// Will iterate ui_animations searching for the given GuiAnimation*.
	void DeleteUIAnimation(std::vector<GuiAnimation*>::iterator animation_item);				// Will delete a ui_animation directly without needing to iterate the ui_animations vector.
	void CancelUIAnimation(GuiElement* element_being_animated, GUI_ANIMATION_TYPE animation_type);	// Will cancel an animation of the specified type. Only one of each type can be active.
	void DestroyUIAnimations();

public:																		// ------------------ GUI ELEMENT MANAGEMENT METHODS ------------------
	void OnEventCall(GuiElement* element, GUI_EVENT ui_event);						//This function is called whenever an new event happens, it receives the pointer of the element that caused the event and the kind of event it is.
	void PassFocus();														//Method that passes the focus from an interactible and able to focused element to another with the same conditions.
	bool ElementCanBeFocused(GuiElement* focus_element) const;						//If an element fulfills all requirements (is a button or a scrollbar), then this method returns true. Used to filter which UI elements can or cannot have focus.
	
	//bool VisibleElementIsUnderMouse() const;								//Returns true if the mouse is over a visible UI_Element.
	bool VisibleElementIsUnderCursor() const;								//Returns true if the mouse is over a visible UI_Element.		//TMP CONTROLLER

	//UI* FirstInteractibleElementUnderMouse() const;						//Returs the first interactible element under the mouse.
	GuiElement* FirstInteractibleElementUnderCursor() const;						//Returs the first interactible element under the mouse.		//TMP CONTROLLER
	bool ElementCanBeClicked(GuiElement* clicked_element) const;

	bool ElementHasChilds(GuiElement* parent_element) const;						//Returns true if the element passed as argument has at least one child.
	void UpdateChilds(GuiElement* parent_element);									//Updates all UI Elements that have the element passed as argument as a parent.
	void SetElementsVisibility(GuiElement* parent_element, bool state);				//Enables/Disables the isVisible bool of a UI Element and its childs according to the passed arguments.
	std::vector<GuiElement*> GetElementChilds(GuiElement* parent_element);

	void Debug_UI();														//Shows on screen the different rects that compose the UI Display.

public:																		// --- GUI PUBLIC VARIABLES ---
	GuiElement*							focused_element;							//Change to list item
	std::vector<GuiElement*>::iterator	iterated_element;

	bool						escape;										//When this bool is true the game is exited.
	bool						ui_debug;									//When this bool is true, debug mode is activated.

public:																		// --- GUI AUDIO VARIABLES ---
	bool						audio_already_loaded;
	
	uint						new_game_button_clicked_fx;
	uint						options_button_clicked_fx;
	uint						back_button_clicked_fx;
	uint						main_menu_button_clicked_fx;
	uint						pause_menu_button_clicked_fx;
	uint						standard_button_clicked_fx;
	uint						upgrade_button_clicked_fx;
	uint						recruit_unit_button_clicked_fx;
	uint						no_resources_fx;

public:																		// --- GUI FONT VARIABLES ---
	bool						fonts_already_loaded;
	
	_TTF_Font*					borgsquadcond_12;
	_TTF_Font*					borgsquadcond_15;
	_TTF_Font*					borgsquadcond_20;
	_TTF_Font*					borgsquadcond_25;
	_TTF_Font*					borgsquadcond_30;
	_TTF_Font*					borgsquadcond_40;
	_TTF_Font*					borgsquadcond_50;

private:																	// --- GUI PRIVATE VARIABLES ---
	SDL_Texture*				atlas;										//Texture of the atlas (UI Spritesheet)
	std::string					atlas_file_name;							//Name of the atlas in the xml file.

	std::vector<GuiElement*>	elements;									//List where all the UI elements in a scene will be stored at.
	std::vector<GuiAnimation*>	ui_animations;
};

#endif // __GUI_H__
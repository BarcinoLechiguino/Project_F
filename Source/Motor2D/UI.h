#ifndef __UI_H__
#define __UI_H__

#include "SDL/include/SDL.h"
#include "Module.h"
#include "Point.h"

class Entity;

enum class UI_ELEMENT 
{
	EMPTY,
	IMAGE,
	TEXT,
	BUTTON,
	SCROLLBAR,
	INPUTBOX,
	HEALTHBAR,
	CREATIONBAR,
	CURSOR
};

enum class UI_EVENT
{
	IDLE,
	HOVER,
	UNHOVER,
	CLICKED,
	UNCLICKED,
	FOCUSED,
	UNFOCUSED,
	TEXT_INPUT,
	TEXT_EXECUTION
};

class UI
{
public:
	UI(UI_ELEMENT element, int x, int y, SDL_Rect rect, Module* listener = nullptr, UI* parent = nullptr);
	UI();
	virtual ~UI();

	virtual bool Draw();

	virtual void CheckInput();

	virtual void CleanUp();

	void BlitElement(SDL_Texture* texture, int x, int y, SDL_Rect* rect, float speed, float render_scale);

public:
	void SetScreenPos(iPoint position);					// Sets the position of a UI_Element with the Screen as point of reference.
	iPoint GetScreenPos() const;						// Gets the position of a UI_Element with the Screen as point of reference.
														   
	void SetScreenRect(SDL_Rect rect);					// Sets the data members of a UI_Element's rect with the Screen as point of reference.
	SDL_Rect GetScreenRect() const;						// Gets the data members of a UI_Element's rect with the Screen as point of reference.
														   
	void SetHitbox(SDL_Rect hitbox);					// Sets the data members of a UI_Element's hitbox with the Screen as point of reference.
	SDL_Rect GetHitbox() const;							// Gets the data members of a UI_Element's hitbox with the Screen as point of reference.
														   
	void SetLocalPos(iPoint local_position);			// Sets the position of a UI Element with it's Parent as point of reference.
	iPoint GetLocalPos() const;							// Gets the position of a UI Element with it's Parent as point of reference.
														   
	void SetLocalRect(SDL_Rect local_rect);				// Sets the rect of a UI Element with the parent element as point of reference.
	SDL_Rect GetLocalRect() const;						// Gets the rect of a UI Element with the parent element as point of reference.
														   
	void SetLocalHitbox(SDL_Rect local_hitbox);			// Sets the hitbox rect of a UI Element with the parent element as the point of reference.
	SDL_Rect GetLocalHitbox() const;
	
	void SetElementPosition(const iPoint& position);	// Will set the element's screen position and hitbox position to the one passed as argument.
	iPoint GetElementPosition() const;					// Gets the current position of the UI Element.

	iPoint GetMousePos() /*const*/;						// Gets the mouse's position in pixels. (World Position)
	iPoint GetCursorPos();								// Gets the UI_Cursor's position in pixels. (World Position)		//TMP CONTROLLER

	iPoint GetMouseTilePosition();						// Gets the mouse's position in tiles. (Map Position)
	iPoint GetMouseMotion() /*const*/;					// Gets the mouse's motion.

	bool CheckMousePos() const;							// Checks the position of the mouse.
	bool CheckCursorPos() const;						// Checks if the cursor is inside a UI_Element.						//TMP CONTROLLER

	bool IsHovered() const;								   
	bool IsFocused() const;								// Centralizing common functionalities.

	bool IsForemostElement() const;						// Returns true if its the first element in inverse order of draw and has the mouse on it.
	bool ElementCanBeDragged() const;					// Returns true if all dragging conditions are met.
	bool ElementRemainedInPlace() const;				// If a UI Element was clicked but not dragged, this will return true.

	void DragElement();											// Drags a draggable UI Element to the mouse's position.
	void AxisRestrictedDragElement(bool X_Axis, bool Y_Axis);	// If the UI element is a scrollbar element, then dragging will be limitied to the Y axis.
	void CheckElementChilds();									// Checks if a UI Element has childs and updates them in case the parent element had changed its position (dragged)

public:
	bool		is_visible;							// Keeps track of whether or not a UI Element is visible or not. Can be overlapped with isInteractible.
	bool		is_interactible;					// Keeps track of whether a UI Element is interactible or not.
	bool		is_draggable;						// Keeps track of whether a UI Element is draggable or not.
	iPoint		previous_mouse_position;			// Keeps track of the previous position of the mouse in the screen before starting to drag anything.

	bool		is_drag_target;						// Keeps track whether or not an element susceptible to be dragged is the element wanted to be dragged. Set on KEY_DOWN / KEY_UP.
													// Used to avoid dragging the window after moving the mouse outside an element while keeping the mouse button clicked (button to window...).
	bool		drag_x_axis;						// Keeps track of which axis an element can be dragged in, in this case the X axis. If both bools are true or false, drag will be free.
	bool		drag_y_axis;						// Keeps track of which axis an element can be dragged in, in this case the Y axis. If both bools are true or false, drag will be free.

	UI_EVENT	ui_event;							// Defines which events will the UI_Elements send when interacted with.
	UI_ELEMENT	element;							// Enum that defines which kind of element a UI element is.

	Module*		listener;							// Callback to Module, maybe need to make a virtual event detection function. Whenever an event is triggered, this calls the right module for the event.

	UI*			parent;								// Keeps track of the dependencies between UI elements.
	
	iPoint		initial_position;					// Keeps track of the initial position of a UI Element. Create Get/Set Methods?
	
	bool		is_filled;							// Determines if an empty UI Element will have its rect drawn or not.

	bool		is_transitioning;					// 

private:

	iPoint		position;							// Position of the UI element in the world.
	iPoint		local_position;						// Position of the UI element relative to its parent's position in the world.

	SDL_Rect	rect;								// Rectangle that represents the UI element in the world. Used for textures.
	SDL_Rect	local_rect;							// Rectangle coordinates and size of the UI Element taking the parent element as point of reference.

	SDL_Rect	hitbox;								// Rectangle that represents the UI element's hitbox. Used for interactions.
	SDL_Rect	local_hitbox;						// Data members of hitbox (position) according to the parent's world position.

	iPoint		mouse_position;						// Position of the mouse in pixels.
	iPoint		mouse_tile_position;				// Position of the mouse in tiles.
	iPoint		mouse_motion;						// Motion of the mouse. Used to move a dragged element around.
};

#endif // !__UI_IMAGE_H__
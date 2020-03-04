#ifndef __UI_H__
#define __UI_H__

#include "p2Log.h"
#include "p2Point.h"
#include "j1Input.h"
#include "SDL/include/SDL.h"

//class iPoint;			//If declared as a forward declaration instead of an include, 
//class SDL_Rect;		//then iPoint and SDL_Rect need to be pointers.

enum class UI_Element 
{
	EMPTY,
	IMAGE,
	TEXT,
	BUTTON,
	SCROLLBAR,
	INPUTBOX,
};

enum class UI_Event
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
	UI(UI_Element element, int x, int y, SDL_Rect rect, UI* parent = nullptr);

	UI();

	virtual ~UI();

	virtual bool Draw();

	virtual void CheckInput();

	virtual void CleanUp();

	void BlitElement(SDL_Texture* texture, int x, int y, SDL_Rect* rect);

public:
	void SetScreenPos(iPoint position);				//Sets the position of a UI_Element with the Screen as point of reference.
	iPoint GetScreenPos() const;					//Gets the position of a UI_Element with the Screen as point of reference.
	
	void SetScreenRect(SDL_Rect rect);				//Sets the data members of a UI_Element's rect with the Screen as point of reference.
	SDL_Rect GetScreenRect() const;					//Gets the data members of a UI_Element's rect with the Screen as point of reference.
	
	void SetHitbox(SDL_Rect hitbox);				//Sets the data members of a UI_Element's hitbox with the Screen as point of reference.
	SDL_Rect GetHitbox() const;						//Gets the data members of a UI_Element's hitbox with the Screen as point of reference.
	
	void SetLocalPos(iPoint localPosition);			//Sets the position of a UI Element with it's Parent as point of reference.
	iPoint GetLocalPos() const;						//Gets the position of a UI Element with it's Parent as point of reference.

	void SetLocalRect(SDL_Rect localRect);			//Sets the rect of a UI Element with the parent element as point of reference.
	SDL_Rect GetLocalRect() const;					//Gets the rect of a UI Element with the parent element as point of reference.

	void SetLocalHitbox(SDL_Rect localHitbox);		//Sets the hitbox rect of a UI Element with the parent element as the point of reference.
	SDL_Rect GetLocalHitbox();

	iPoint GetMousePos() /*const*/;					//Gets the mouse's position.
	iPoint GetMouseMotion() /*const*/;				//Gets the mouse's motion.
	bool CheckMousePos() const;						//Checks the position of the mouse.

	bool IsHovered() const;
	bool IsFocused() const;							//Centralizing common functionalities.

	bool IsForemostElement() const;					//Returns true if its the first element in inverse order of draw and has the mouse on it.
	bool ElementCanBeDragged() const;				//Returns true if all dragging conditions are met.
	bool ElementRemainedInPlace() const;			//If a UI Element was clicked but not dragged, this will return true.

	void DragElement();											//Drags a draggable UI Element to the mouse's position.
	void AxisRestrictedDragElement(bool X_Axis, bool Y_Axis);	//If the UI element is a scrollbar element, then dragging will be limitied to the Y axis.
	void CheckElementChilds();									//Checks if a UI Element has childs and updates them in case the parent element had changed its position (dragged)

public:
	bool		isVisible;							//Keeps track of whether or not a UI Element is visible or not. Can be overlapped with isInteractible.
	bool		isInteractible;						//Keeps track of whether a UI Element is interactible or not.
	bool		isDraggable;						//Keeps track of whether a UI Element is draggable or not.
	iPoint		prevMousePos;						//Keeps track of the previous position of the mouse in the screen before starting to drag anything.

	bool		isDragTarget;						//Keeps track whether or not an element susceptible to be dragged is the element wanted to be dragged. Set on KEY_DOWN / KEY_UP.
													//Used to avoid dragging the window after moving the mouse outside an element while keeping the mouse button clicked (button to window...).
	bool		dragXAxis;							//Keeps track of which axis an element can be dragged in, in this case the X axis. If both bools are true or false, drag will be free.
	bool		dragYAxis;							//Keeps track of which axis an element can be dragged in, in this case the Y axis. If both bools are true or false, drag will be free.

	UI_Event	ui_event;							//Defines which events will the UI_Elements send when interacted with.
	UI_Element	element;							//Enum that defines which kind of element a UI element is.

	j1Module*	listener;							//Callback to j1Module, maybe need to make a virtual event detection function. Whenever an event is triggered, this calls the right module for the event.
	UI*			parent;								//Keeps track of the dependencies between UI elements.

	iPoint		initialPosition;					//Keeps track of the initial position of a UI Element. Create Get/Set Methods?
	
	bool		isFilled;							//Determines if an empty UI Element will have its rect drawn or not.

private:
	iPoint		position;							//Position of the UI element in the world.
	iPoint		localPosition;						//Position of the UI element relative to its parent's position in the world.

	SDL_Rect	rect;								//Rectangle that represents the UI element in the world. Used for textures.
	SDL_Rect	localRect;							//Rectangle coordinates and size of the UI Element taking the parent element as point of reference.

	SDL_Rect	hitbox;								//Rectangle that represents the UI element's hitbox. Used for interactions.
	SDL_Rect	localHitbox;						//Data members of hitbox (position) according to the parent's world position.

	iPoint		mousePos;							//Position of the mouse.
	iPoint		mouseMotion;						//Motion of the mouse. Used to move a dragged element around.
};

#endif // !__UI_IMAGE_H__
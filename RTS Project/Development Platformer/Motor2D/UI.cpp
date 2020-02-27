#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Scene.h"
#include "UI.h"
#include "UI_Image.h"
#include "UI_Text.h"

UI::UI(UI_Element element, int x, int y, SDL_Rect rect, UI* parent) :
	element(element),										//The variables of the UI element are initialized at the constructor.
	position(x, y),
	rect(rect),
	parent(parent)
{
	//hitbox = {GetPosition().x, GetPosition().y, GetRect().w, GetRect().h };
	hitbox = {position.x, position.y, this->rect.w, this->rect.h };
}

UI::UI()		//Default Constructor
{

}

UI::~UI()
{

}

bool UI::Draw()
{	
	return true;
}

void UI::CheckInput()
{
	return;
}

void UI::CleanUp()
{
	return;
}

void UI::BlitElement(SDL_Texture* texture, int x, int y, SDL_Rect* rect)
{
	App->render->Blit(texture, x, y, rect, false, 0.0f);		//Setting the blit's speed argument to 0 will make the UI Element remain static in relation to the camera.
}

// ----------------------------------------- SET/GET METHODS -----------------------------------------
void UI::SetScreenPos(iPoint position)
{
	this->position = position;							//this-> Allows to overload the position variable name as it declares that the "position" we are setting is the one declared in UI.h.
}

iPoint UI::GetScreenPos() const
{
	return position;									//Returns the position of a UI element.
}

void UI::SetScreenRect(SDL_Rect rect)
{
	this->rect = rect;									//this-> Sets the pointer of a UI_Element so only that UI element's rect is changed. Moreover it allows to overload the rect variable name.
}

SDL_Rect UI::GetScreenRect() const
{
	return rect;										//Returns the rect of a UI element.
}

void UI::SetHitbox(SDL_Rect hitbox)
{
	this->hitbox = hitbox;								//this-> Sets the pointer of a UI_Element so only that UI element's hitbox is changed. Moreover it allows to overload the hitbox variable name.
}

SDL_Rect UI::GetHitbox() const
{
	return hitbox;										//Returns the hitbox of a UI element.
}

void UI::SetLocalPos(iPoint localPosition)
{
	this->localPosition = localPosition;
}

iPoint UI::GetLocalPos() const
{
	return localPosition;
}

void UI::SetLocalRect(SDL_Rect localRect)
{
	int localPosX = localRect.x - this->parent->rect.x;
	int localPosY = localRect.y - this->parent->rect.y;
	
	SDL_Rect newLocalRect = { localPosX, localPosY, localRect.w, localRect.h };

	this->localRect = newLocalRect;

	//SetScreenRect(newLocalRect);
}

SDL_Rect UI::GetLocalRect() const
{
	if (parent != NULL)
	{
		//SDL_Rect localRect = { rect.x - parent->rect.x, rect.y - parent->rect.y, rect.w, rect.h };
		//SDL_Rect localRect = { localPosition.x, localPosition.y, rect.w, rect.h };

		return localRect;
	}
}

void UI::SetLocalHitbox(SDL_Rect localHitbox)
{
	int localPosX = localRect.x - this->parent->rect.x;
	int localPosY = localRect.y - this->parent->rect.y;
	
	SDL_Rect newLocalHitbox = { localPosX, localPosY, localHitbox.w, localHitbox.h };

	this->localHitbox = newLocalHitbox;
}

// -------------------------------- UI ELEMENT INTERACTIONS --------------------------------
iPoint UI::GetMousePos() /*const*/
{
	App->input->GetMousePosition(mousePos.x, mousePos.y);

	return mousePos;
}

bool UI::CheckMousePos() const
{
	return(mousePos.x > hitbox.x && mousePos.x < hitbox.x + hitbox.w
		&& mousePos.y > hitbox.y && mousePos.y < hitbox.y + hitbox.h);
}

iPoint UI::GetMouseMotion() /*const*/
{
	App->input->GetMouseMotion(mouseMotion.x, mouseMotion.y);

	return mouseMotion;
}

// --- This method checks whether a UI Element is being hovered (The mouse is inside it's hitbox).
bool UI::IsHovered() const
{
	return CheckMousePos();
}

// --- This method checks whether the focused element is the same as the element that called the method.
bool UI::IsFocused() const
{
	return App->gui->focusedElement == this;
}

// --- This method checks whether the element that called the method is the foremost element under the mouse.
bool UI::IsForemostElement() const
{
	return App->gui->FirstElementUnderMouse() == this;
}

// --- This method checks whether or not the element that called the method fulfills the conditions to be dragged.
bool UI::ElementCanBeDragged() const
{
	return ((isDraggable && isDragTarget && App->gui->FirstElementUnderMouse() == this) || isDragTarget);
}

// --- This method checks whetheror not the element that called the method has been clicked but not dragged anywhere.
bool UI::ElementRemainedInPlace() const
{
	return (GetScreenPos() == initialPosition);
}

// --- Drags an element around taking into account where the mouse was and where it currently is.
void UI::DragElement()
{	
																			// --- Updating the UI Element's position when it is being dragged.
	iPoint origin(0, 0);													//This prevents sending undragged elements to undesired places when passing from dragging one element to another as prevMousePos in undragged elements is (0,0).

	if (prevMousePos != GetMousePos() && prevMousePos != origin)			//If the recorded prevMousePos is different than the current mousePos and prevMouse pos is not origin.
	{
		position += GetMousePos() - prevMousePos;							//Check this, mouse is not at a fixed point, the UI Element moves too slow.
		SetScreenPos(position);
	}

																			// --- Updating the UI Element's hitbox rect when it is being dragged.
	SDL_Rect newPosRect = { position.x, position.y, rect.w, rect.h };

	this->SetHitbox(newPosRect);

	//iPoint draggingPos = position + GetMouseMotion();
}

void UI::AxisRestrictedDragElement(bool X_Axis, bool Y_Axis)
{
	if (X_Axis == Y_Axis)
	{
		return;
	}
	
	if (X_Axis)
	{
		if (parent == NULL)
		{
			position.x += GetMousePos().x - prevMousePos.x;
			SetScreenPos(position);
		}
		else
		{
			if (position.x != parent->position.x && position.x + GetHitbox().w != parent->position.x + parent->GetHitbox().w)
			{
				position.x += GetMousePos().x - prevMousePos.x;
				SetScreenPos(position);
			}

			if (position.x <= parent->position.x)
			{
				position.x = parent->position.x + DRAG_LIMIT_OFFSET;
			}

			if (position.x + GetHitbox().w >= parent->position.x + parent->GetHitbox().w)
			{
				position.x = (parent->position.x + parent->GetHitbox().w) - (GetHitbox().w + DRAG_LIMIT_OFFSET);
			}
		}
	}
	
	if (Y_Axis)
	{
		if (parent == NULL)
		{
			position.y += GetMousePos().y - prevMousePos.y;
			SetScreenPos(position);
		}
		else
		{
			if (position.y != parent->position.y && position.y + GetHitbox().h != parent->position.y + parent->GetHitbox().h)
			{
				position.y += GetMousePos().y - prevMousePos.y;
				SetScreenPos(position);
			}

			if (position.y <= parent->position.y)
			{
				position.y = parent->position.y + DRAG_LIMIT_OFFSET;
			}

			if (position.y + GetHitbox().h >= parent->position.y + parent->GetHitbox().h)
			{
				position.y = (parent->position.y + parent->GetHitbox().h) - (GetHitbox().h + DRAG_LIMIT_OFFSET);
			}
		}
	}

	SDL_Rect newPosRect = { position.x, position.y, rect.w, rect.h };

	this->SetHitbox(newPosRect);
}

// --- This method Checks if a UI Element has childs and updates them in case the UI Element (parent) has been moved/dragged.
void UI::CheckElementChilds()
{
	if (App->gui->ElementHasChilds(this))
	{
		App->gui->UpdateChilds(this);
	}
}
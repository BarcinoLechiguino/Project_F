#include "Brofiler\Brofiler.h"

#include "Application.h"
#include "Input.h"
#include "GuiManager.h"
#include "UI_Image.h"

#include "UI_Scrollbar.h"


UI_Scrollbar::UI_Scrollbar(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, SDL_Rect thumbSize, iPoint thumbOffset, SDL_Rect drag_area, float drag_factor, bool drag_x_axis, bool drag_y_axis,
				bool inverted_scrolling, bool is_visible, bool is_interactible, bool is_draggable, Module* listener, UI* parent , SDL_Rect* scroll_mask, iPoint maskOffset,
				bool emptyElements) : UI(element, x, y, hitbox, listener, parent)
{
	//tex = App->gui->GetAtlas();

	if (is_interactible)															//If the Scrollbar element is interactible.
	{
		this->listener = listener;													//This scrollbar's listener is set to the App->gui module (For OnCallEvent()).
	}

	if (parent != NULL)																//If a parent is passed as argument.
	{
		int localPosX = x - parent->GetScreenPos().x;								//Gets the local position of the Button element in the X Axis.
		int localPosY = y - parent->GetScreenPos().y;								//Gets the local position of the Button element in the Y Axis.

		iPoint localPos = { localPosX,localPosY };									//Buffer iPoint to pass it as argument to SetLocalPos().

		SetLocalPos(localPos);														//Sets the local poisition of this Button element to the given localPos.
	}

	// -------------------------- SCROLLBAR VARIABLES --------------------------
	// --- Scrollbar Flags
	// --- Setting this element's flags to the ones passed as argument.
	this->is_visible			= is_visible;										//Sets the isVisible flag to the one passed as argument.
	this->is_interactible		= is_interactible;									//Sets the isInteractible flag to the one passed as argument. 
	this->is_draggable			= is_draggable;										//Sets the isDraggable flag to the one passed as argument.
	this->drag_x_axis			= drag_x_axis;										//Sets the dragXaxis flag to the one passed as argument.
	this->drag_y_axis			= drag_y_axis;										//Sets the dragYaxis flag to the one passed as argument.
	previous_mouse_position		= iPoint(0, 0);										//Initializes prevMousePos for this UI Element. Safety measure to avoid weird dragging behaviour.
	initial_position			= GetScreenPos();									//Records the initial position where the element is at at app execution start.											//Records the initial position where the input box is at app execution start.

	// --- Scrollbar Elements
	/*bar = UI_Image(UI_Element::IMAGE, x, y, hitbox, true, false, false, this);
	thumb = UI_Image(UI_Element::IMAGE, x + thumbOffset.x, y + thumbOffset.y, thumbSize, true, true, true, this);*/

	if (!emptyElements)
	{
		bar = (UI_Image*)App->gui_manager->CreateImage(UI_ELEMENT::IMAGE, x, y, hitbox, is_visible, false, false, nullptr, this);
		thumb = (UI_Image*)App->gui_manager->CreateImage(UI_ELEMENT::IMAGE, x + thumbOffset.x, y + thumbOffset.y, thumbSize, is_visible, true, true, nullptr, this);
	}
	else
	{
		bar = (UI_Image*)App->gui_manager->CreateImage(UI_ELEMENT::EMPTY, x, y, hitbox, is_visible, false, false, nullptr, this);
		thumb = (UI_Image*)App->gui_manager->CreateImage(UI_ELEMENT::IMAGE, x + thumbOffset.x, y + thumbOffset.y, thumbSize, is_visible, true, true, nullptr, this);
	}

	if (scroll_mask != nullptr)
	{
		this->scroll_mask = (UI_Image*)App->gui_manager->CreateImage(UI_ELEMENT::EMPTY, x + maskOffset.x, y + maskOffset.y, *scroll_mask, false, false, false, nullptr, this);
	}

	// --- Other Scrollbar Variables
	this->scrollbar_width			= this->GetHitbox().w;
	this->scrollbar_height			= this->GetHitbox().h;
	this->drag_area					= {x, y, drag_area.w, drag_area.h};
	//this->dragArea				= {x, y, scrollbarWidth, scrollbarHeight };
	this->drag_factor				= drag_factor;
	this->drag_displacement			= iPoint(0, 0);
	this->inverted_scrolling		= inverted_scrolling;
	this->arrow_position_factor		= drag_factor;
	this->drag_x_axis				= drag_x_axis;
	this->drag_y_axis				= drag_y_axis;

	if (this->drag_x_axis == this->drag_y_axis)
	{
		this->drag_y_axis = true;
		this->drag_x_axis = false;
	}
	// -----------------------------------------------------------------------------------
}

bool UI_Scrollbar::Draw()
{
	CheckInput();

	UpdateLinkedElements();
	
	//DrawScrollbarElements();

	//BlitElement()

	return true;
}

void UI_Scrollbar::CheckInput()
{
	BROFILER_CATEGORY("Scrollbar_CheckInput", Profiler::Color::LightGoldenRodYellow);

	if (is_visible)																				//If the image element is visible
	{
		GetMousePos();																			//Gets the mouse's position on the screen.

		if (!IsHovered())																		//If the mouse is not on the image.
		{
			ui_event = UI_EVENT::IDLE;
		}
		
		if (IsHovered() || LinkedElementsBeingHovered() || MouseWithinDragArea())
		{
			DragThumbWithMousewheel();
		}

		if (IsFocused())
		{
			CheckKeyboardInputs();
			DragThumbWithMousewheel();
		}

		if (IsHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_DOWN)		//If the mouse is on the image and the left mouse button is pressed.
		{
			if (IsForemostElement() || !IsFocused())
			{
				previous_mouse_position = GetMousePos();									//Sets the initial position where the mouse was before starting to drag the element.
				initial_position = GetScreenPos();											//Sets initialPosition with the current position at mouse KEY_DOWN.
				is_drag_target = true;														//Sets the element as the drag target.
				App->gui_manager->focused_element = this;											//Set the focus on the Input Box element when it is clicked.
			}

			PlaceThumbOnMousePos();															//TMP FIX, FIX FOCUS LATER.

			/*if (IsFocused())
			{
				PlaceThumbOnMousePos();
			}*/
		}

		if (CheckCursorPos() && App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_A) == BUTTON_STATE::BUTTON_DOWN)		
		{
			PlaceThumbOnCursorPos();
		}

		if (!IsHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_DOWN)		//Unfocus when the mouse clicks outside the input box.
		{
			if (App->gui_manager->focused_element == this)
			{
				App->gui_manager->focused_element = nullptr;
			}
		}

		if ((IsHovered() || is_drag_target) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_REPEAT)	//If the mouse is on the image and the left mouse button is continuously pressed.
		{
			if (IsForemostElement() || is_drag_target)															//If it is the first element under the mouse (in inverse order of draw)
			{
				ui_event = UI_EVENT::CLICKED;

				if (ElementCanBeDragged() && is_draggable)									//If the UI Image element is draggable and is the foremost element under the mouse.
				{
					DragElement();															//The element is dragged around.

					CheckElementChilds();													//Checks if this image element has any childs and updates them in case the image element (parent) has had any change in position.

					previous_mouse_position = GetMousePos();								//prevMousePos is set with the new position where the mouse is after dragging for a frame.
				}
			}
		}

		if (IsHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_UP)
		{
			if (is_drag_target)
			{
				is_drag_target = false;
				initial_position = GetScreenPos();
			}
		}

		if (is_interactible)																//If the image element is interactible.
		{
			if (listener != nullptr)
			{
				listener->OnEventCall(this, ui_event);										//The listener call the OnEventCall() method passing this UI_Image and it's event as arguments.
			}
		}
	}
}

void UI_Scrollbar::DrawScrollbarElements()
{
	//bar.Draw();
	//thumb.Draw();
	//scrollMask->isVisible = false;
}

void UI_Scrollbar::LinkScroll(UI* element)
{
	linked_elements.push_back(element);
}

void UI_Scrollbar::UpdateLinkedElements()
{
	BROFILER_CATEGORY("Scrollbar_UpdateLinkedElements", Profiler::Color::LightGoldenRodYellow);

	for (std::vector<UI*>::iterator element = linked_elements.begin(); element != linked_elements.end(); element++)
	{
		UI* elem = (*element);

		if (thumb->GetScreenPos() != thumb->initial_position)
		{
			if (drag_x_axis)
			{
				drag_factor = GetDragFactor(elem);
				float x_PositionOffset = (thumb->GetScreenPos().x - thumb->initial_position.x) * drag_factor;

				if (inverted_scrolling)
				{
					drag_displacement = iPoint(elem->GetScreenPos().x - (int)x_PositionOffset, elem->GetLocalPos().y);
				}
				else
				{
					drag_displacement = iPoint(elem->GetScreenPos().x + (int)x_PositionOffset, elem->GetLocalPos().y);
				}
			}
			if (drag_y_axis)
			{
				drag_factor = GetDragFactor(elem);
				float y_PositionOffset = (thumb->GetScreenPos().y - thumb->initial_position.y) * drag_factor;

				if (inverted_scrolling)
				{
					drag_displacement = iPoint(elem->GetScreenPos().x, elem->GetLocalPos().y - (int)y_PositionOffset);
				}
				else
				{
					drag_displacement = iPoint(elem->GetScreenPos().x, elem->GetLocalPos().y + (int)y_PositionOffset);
				}
			}
			
			elem->SetScreenPos(drag_displacement);
			elem->SetHitbox({ elem->GetScreenPos().x
							, elem->GetScreenPos().y
							, elem->GetHitbox().w
							, elem->GetHitbox().h });
		}
	}
}

bool UI_Scrollbar::LinkedElementsBeingHovered()
{	
	for (std::vector<UI*>::iterator element = linked_elements.begin(); element != linked_elements.end(); element++)
	{
		if ((*element)->IsHovered())
		{
			return true;
		}
	}

	return false;
}

bool UI_Scrollbar::MouseWithinDragArea()
{
	iPoint scrollMousePos = GetMousePos();
	
	return (scrollMousePos.x > drag_area.x && scrollMousePos.x < drag_area.x + drag_area.w
		&& scrollMousePos.y > drag_area.y && scrollMousePos.y < drag_area.y + drag_area.h);
}

float UI_Scrollbar::GetDragFactor(UI* element)
{
	if (drag_x_axis == drag_y_axis)
	{
		return 0.0f;
	}
	
	if (drag_x_axis)
	{
		float elem_width = (float)element->GetHitbox().w;
		float drag_area_width = (float)drag_area.w;
		drag_factor = (elem_width / drag_area_width);
	}
	if (drag_y_axis)
	{
		float elem_height = (float)element->GetHitbox().h;
		float drag_area_height = (float)drag_area.h;
		drag_factor = (elem_height / drag_area_height);
	}

	return drag_factor;
}

bool UI_Scrollbar::GetDragXAxis() const
{
	return drag_x_axis;
}

bool UI_Scrollbar::GetDragYAxis() const
{
	return drag_y_axis;
}

iPoint UI_Scrollbar::GetThumbLocalPos()
{
	return thumb->GetLocalPos();
}

SDL_Rect UI_Scrollbar::GetThumbHitbox()
{
	return thumb->GetHitbox();
}

void UI_Scrollbar::SetThumbHitbox(SDL_Rect hitbox)
{
	thumb->SetHitbox(hitbox);
}

void UI_Scrollbar::PlaceThumbOnMousePos()
{
	if (drag_x_axis /*&& !drag_y_axis*/)
	{
		int half_thumb_width = (int)(thumb->GetHitbox().w * 0.5f);

		new_thumb_position = { GetMousePos().x - half_thumb_width, thumb->GetScreenPos().y };

		thumb->SetScreenPos(new_thumb_position);
		thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
	}

	if (drag_y_axis /*&& !drag_x_axis*/)
	{
		int half_thumb_height = (int)(thumb->GetHitbox().h * 0.5f);

		new_thumb_position = { thumb->GetScreenPos().x, GetMousePos().y + half_thumb_height };

		thumb->SetScreenPos(new_thumb_position);
		thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
	}
}

void UI_Scrollbar::PlaceThumbOnCursorPos()
{
	if (drag_x_axis /*&& !drag_y_axis*/)
	{
		int half_thumb_width = (int)(thumb->GetHitbox().w * 0.5f);

		new_thumb_position = { GetCursorPos().x - half_thumb_width, thumb->GetScreenPos().y };

		thumb->SetScreenPos(new_thumb_position);
		thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
	}

	if (drag_y_axis /*&& !drag_x_axis*/)
	{
		int half_thumb_height = (int)(thumb->GetHitbox().h * 0.5f);

		new_thumb_position = { thumb->GetScreenPos().x, GetCursorPos().y + half_thumb_height };

		thumb->SetScreenPos(new_thumb_position);
		thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
	}
}

void UI_Scrollbar::CheckKeyboardInputs()
{
	if (drag_x_axis)
	{
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_STATE::KEY_DOWN)
		{
			if (!ThumbIsAtLeftBound())
			{
				int arrow_factor_left = thumb->GetScreenPos().x - (scrollbar_height * (int)arrow_position_factor);
				new_thumb_position = { arrow_factor_left, thumb->GetScreenPos().y };

				thumb->SetScreenPos(new_thumb_position);
				thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_STATE::KEY_DOWN)
		{
			if (!ThumbIsAtRightBound())
			{
				int arrow_factor_right = thumb->GetScreenPos().x + (scrollbar_height * (int)arrow_position_factor);
				new_thumb_position = { arrow_factor_right, thumb->GetScreenPos().y };

				thumb->SetScreenPos(new_thumb_position);
				thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_HOME) == KEY_STATE::KEY_DOWN)
		{
			new_thumb_position = { this->GetScreenPos().x + DRAG_LIMIT_OFFSET,  thumb->GetScreenPos().y };

			thumb->SetScreenPos(new_thumb_position);
			thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
		}

		if (App->input->GetKey(SDL_SCANCODE_END) == KEY_STATE::KEY_DOWN)
		{
			new_thumb_position = { ((this->GetScreenPos().x + this->GetHitbox().w) - (thumb->GetHitbox().w + DRAG_LIMIT_OFFSET)), thumb->GetScreenPos().y };

			thumb->SetScreenPos(new_thumb_position);
			thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
		}
	}

	if (drag_y_axis /*&& !drag_x_axis*/)
	{
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_STATE::KEY_DOWN)
		{
			if (!ThumbIsAtUpperBound())
			{
				int arrowFactorUp = thumb->GetScreenPos().y - (scrollbar_height * (int)arrow_position_factor);
				new_thumb_position = { thumb->GetScreenPos().x, arrowFactorUp };

				thumb->SetScreenPos(new_thumb_position);
				thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
			}
		}

		if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_STATE::KEY_DOWN)
		{
			if (!ThumbIsAtLowerBound())
			{
				int arrowFactorDown = thumb->GetScreenPos().y + (scrollbar_height * (int)arrow_position_factor);
				new_thumb_position = { thumb->GetScreenPos().x, arrowFactorDown };

				thumb->SetScreenPos(new_thumb_position);
				thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_HOME) == KEY_STATE::KEY_DOWN)
		{
			new_thumb_position = { thumb->GetScreenPos().x,  this->GetScreenPos().y + DRAG_LIMIT_OFFSET };

			thumb->SetScreenPos(new_thumb_position);
			thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
		}

		if (App->input->GetKey(SDL_SCANCODE_END) == KEY_STATE::KEY_DOWN)
		{
			new_thumb_position = { thumb->GetScreenPos().x,  ((this->GetScreenPos().y + this->GetHitbox().h) - (thumb->GetHitbox().h + DRAG_LIMIT_OFFSET)) };

			thumb->SetScreenPos(new_thumb_position);
			thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
		}
	}
	
	CheckScrollbarBounds();
}

void UI_Scrollbar::DragThumbWithMousewheel()																							// ----------------------------------------------
{
	App->input->GetMousewheelScrolling(mouse_wheel_scroll.x, mouse_wheel_scroll.y);

	mouse_wheel_scroll = { mouse_wheel_scroll.x * 3, mouse_wheel_scroll.y * 3 };															//It is multiplied by 3 to increase scroll speed.

	if (drag_x_axis)
	{
		if (inverted_scrolling)
		{
			if (ThumbIsWithinHorizontalScrollbarBounds())
			{
				new_thumb_position = { thumb->GetScreenPos().x - mouse_wheel_scroll.y, thumb->GetScreenPos().y };							// Done for readability.

				thumb->SetScreenPos(new_thumb_position);
				thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
			}
		}
		else
		{
			if (ThumbIsWithinHorizontalScrollbarBounds())
			{
				new_thumb_position = { thumb->GetScreenPos().x + mouse_wheel_scroll.y, thumb->GetScreenPos().y };

				thumb->SetScreenPos(new_thumb_position);
				thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
			}
		}

		CheckScrollbarBounds();
	}

	if (drag_y_axis)
	{
		if (inverted_scrolling)
		{
			if (ThumbIsWithinVerticalScrollbarBounds())
			{
				new_thumb_position = { thumb->GetScreenPos().x, thumb->GetScreenPos().y - mouse_wheel_scroll.y };

				thumb->SetScreenPos(new_thumb_position);
				thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
			}
		}
		else
		{
			if (ThumbIsWithinVerticalScrollbarBounds())
			{
				new_thumb_position = { thumb->GetScreenPos().x, thumb->GetScreenPos().y + mouse_wheel_scroll.y };

				thumb->SetScreenPos(new_thumb_position);
				thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
			}
		}

		CheckScrollbarBounds();
	}
}

bool UI_Scrollbar::ThumbIsWithinVerticalScrollbarBounds()
{
	return (thumb->GetScreenPos().y != this->GetScreenPos().y 
			&& thumb->GetScreenPos().y + thumb->GetHitbox().h != this->GetScreenPos().y + this->GetHitbox().h);
}

bool UI_Scrollbar::ThumbIsAtUpperBound()
{
	return (thumb->GetScreenPos().y <= this->GetScreenPos().y);
}

bool UI_Scrollbar::ThumbIsAtLowerBound()
{
	return (thumb->GetScreenPos().y + thumb->GetHitbox().h >= this->GetScreenPos().y + this->GetHitbox().h);
}

bool UI_Scrollbar::ThumbIsWithinHorizontalScrollbarBounds()
{
	return (thumb->GetScreenPos().x != this->GetScreenPos().x
		&& thumb->GetScreenPos().x + thumb->GetHitbox().w != this->GetScreenPos().x + this->GetHitbox().w);
}

bool UI_Scrollbar::ThumbIsAtLeftBound()
{
	return (thumb->GetScreenPos().x <= this->GetScreenPos().x);
}

bool UI_Scrollbar::ThumbIsAtRightBound()
{
	return (thumb->GetScreenPos().x + thumb->GetHitbox().w >= this->GetScreenPos().x + this->GetHitbox().w);
}

void UI_Scrollbar::CheckScrollbarBounds()
{
	if (drag_x_axis)
	{
		if (ThumbIsAtLeftBound())
		{
			new_thumb_position = { this->GetScreenPos().x + DRAG_LIMIT_OFFSET,  thumb->GetScreenPos().y };

			thumb->SetScreenPos(new_thumb_position);
			thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
		}

		if (ThumbIsAtRightBound())
		{
			new_thumb_position = { ((this->GetScreenPos().x + this->GetHitbox().w) - (thumb->GetHitbox().w + DRAG_LIMIT_OFFSET)), thumb->GetScreenPos().y };

			thumb->SetScreenPos(new_thumb_position);
			thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
		}
	}

	if (drag_y_axis)
	{
		if (ThumbIsAtUpperBound())
		{
			new_thumb_position = { thumb->GetScreenPos().x,  this->GetScreenPos().y + DRAG_LIMIT_OFFSET };

			thumb->SetScreenPos(new_thumb_position);
			thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
		}

		if (ThumbIsAtLowerBound())
		{
			new_thumb_position = { thumb->GetScreenPos().x,  ((this->GetScreenPos().y + this->GetHitbox().h) - (thumb->GetHitbox().h + DRAG_LIMIT_OFFSET)) };

			thumb->SetScreenPos(new_thumb_position);
			thumb->SetHitbox({ thumb->GetScreenPos().x, thumb->GetScreenPos().y, thumb->GetHitbox().w, thumb->GetHitbox().h });
		}
	}
}

void UI_Scrollbar::CleanUp()
{
	bar = nullptr;
	thumb = nullptr;
	scroll_mask = nullptr;
	
	// Only use this if bar, thumb and scroll_mask are not pointers.
	/*if (bar != nullptr)
	{
		bar->CleanUp();
	}

	if (thumb != nullptr)
	{
		thumb->CleanUp();
	}

	if (scroll_mask != nullptr)
	{
		scroll_mask->CleanUp();
	}*/
}
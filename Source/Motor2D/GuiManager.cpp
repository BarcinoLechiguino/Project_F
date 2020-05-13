#include "p2Log.h"

#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Fonts.h"
#include "Console.h"
#include "Entity.h"
#include "Player.h"									// TMP CONTROLLER

#include "GuiManager.h"
#include "UI.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_InputBox.h"
#include "UI_Scrollbar.h"
#include "UI_Healthbar.h"
#include "UI_CreationBar.h"
#include "UI_Cursor.h"

#include "Brofiler\Brofiler.h"

GuiManager::GuiManager() : Module()
{
	name = ("gui");
	audio_already_loaded = false;
}

// Destructor
GuiManager::~GuiManager()
{}

// Called before render is available
bool GuiManager::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool GuiManager::Start()
{
	atlas = App->tex->Load(atlas_file_name.c_str());

	ui_debug = false;
	escape = true;

	
	if (!audio_already_loaded)
	{
		LoadGuiElementsAudio();

		audio_already_loaded = true;
	}

	//CreateGuiCommands();

	return true;
}

// Update all guis
bool GuiManager::PreUpdate()
{
	// THIS
	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_STATE::KEY_DOWN)
	{
		if (!App->player->cursor.game_controller_mode)																				// TMP CONTROLLER
		{
			PassFocus();
		}

		//App->audio->PlayFx(new_game_fx, 0);
	}

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_STATE::KEY_DOWN 
		|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_A) == BUTTON_STATE::BUTTON_DOWN)								// TMP CONTROLLER
	{
		if (focused_element != nullptr && focused_element->is_interactible)
		{
			focused_element->ui_event = UI_EVENT::UNCLICKED;
			
			if (focused_element->listener != nullptr)
			{
				focused_element->listener->OnEventCall(focused_element, focused_element->ui_event);
			}
		}
	}
	

	// CONSOLE MANAGING
	/*if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN)			//  Bring this to console.cpp?
	{
		SetElementsVisibility(App->console->console_background, !App->console->console_background->is_visible);
		//SetElementsVisibility(App->scene->upper_bar, !App->scene->upper_bar->is_visible);							//As it does not currently exist it CTDs on a nullptr

		
		App->input->ClearTextInput();

		if (App->console->console_background->is_visible)
		{
			focused_element = App->console->console_input;
		}
	}*/
	

	return true;
}

// Called after all Updates
bool GuiManager::PostUpdate()
{	
	BROFILER_CATEGORY("GUI_PostUpdate", Profiler::Color::NavajoWhite);
	
	//App->console->DrawBackgroundElement();		//THIS HERE CONSOLE

	for (int i = 0; i < (int)elements.size(); ++i)
	{
		if (elements[i]->is_visible)
		{
			elements[i]->Draw();
		}
	}

	Debug_UI();

	return escape;
}

// Called before quitting
bool GuiManager::CleanUp()
{
	LOG("Freeing GUI");
	
	DestroyGuiElements();

	App->tex->UnLoad(atlas);
	atlas = nullptr;

	UnLoadGuiElementsAudio();

	return true;
}

// const getter for atlas
/*const*/ SDL_Texture* GuiManager::GetAtlas() const
{
	return atlas;
}

// ----------------------------------- UI ELEMENT CREATION METHODS -----------------------------------
UI* GuiManager::CreateImage(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, bool is_visible, bool is_interactible, bool is_draggable, Module* listener, UI* parent)
{
	BROFILER_CATEGORY("GUI_Image", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_Image(element, x, y, hitbox, is_visible, is_interactible, is_draggable, listener, parent);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

UI* GuiManager::CreateText(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color font_colour, bool is_visible, bool is_interactible, bool is_draggable
	, Module* listener, UI* parent, std::string* string, std::string* hover_string, std::string* left_click_string, std::string* right_click_string)
{
	BROFILER_CATEGORY("GUI_Text", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_Text(element, x, y, hitbox, font, font_colour, is_visible, is_interactible, is_draggable, listener, parent, string, hover_string, left_click_string, right_click_string);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

UI* GuiManager::CreateButton(UI_ELEMENT element, int x, int y, bool is_visible, bool is_interactible, bool is_draggable, Module* listener, UI* parent, SDL_Rect* idle, SDL_Rect* hover, SDL_Rect* clicked)
{
	BROFILER_CATEGORY("GUI_Button", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_Button(element, x, y, is_visible, is_interactible, is_draggable, listener, parent, idle, hover, clicked);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

UI* GuiManager::CreateInputBox(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color font_colour, SDL_Rect cursor, SDL_Color cursor_colour, iPoint text_offset, 
					float blink_frequency, bool is_visible, bool is_interactible, bool is_draggable, Module* listener, UI* parent, std::string* default_string, bool empty_elements)
{
	BROFILER_CATEGORY("GUI_InputBox", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_InputBox(element, x, y, hitbox, font, font_colour, cursor, cursor_colour, text_offset, blink_frequency, is_visible, is_interactible, is_draggable, listener, parent,
					default_string, empty_elements);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

UI* GuiManager::CreateScrollbar(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, SDL_Rect thumb_size, iPoint thumb_offset , SDL_Rect drag_area, float drag_factor
								, bool drag_x_axis, bool drag_y_axis, bool inverted_scrolling, bool is_visible, bool is_interactible, bool is_draggable, Module* listener, UI* parent
								, SDL_Rect* scroll_mask, iPoint mask_offset, bool empty_elements)
{
	BROFILER_CATEGORY("GUI_ScrollBar", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_Scrollbar(element, x, y, hitbox, thumb_size, thumb_offset, drag_area, drag_factor, drag_x_axis, drag_y_axis, inverted_scrolling,
					is_visible, is_interactible, is_draggable, listener, parent, scroll_mask, mask_offset, empty_elements);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

UI* GuiManager::CreateHealthbar(UI_ELEMENT element, int x, int y, bool is_visible, SDL_Rect* healthbar, SDL_Rect* background, Entity* attached_entity)
{
	UI* elem = nullptr;

	elem = new UI_Healthbar(element, x, y, is_visible, healthbar, background, attached_entity);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

UI* GuiManager::CreateCreationBar(UI_ELEMENT element, int x, int y, bool is_visible, SDL_Rect* creation_bar, SDL_Rect* background, Entity* attached_entity)
{
	UI* elem = nullptr;

	elem = new UI_CreationBar(element, x, y, is_visible, creation_bar, background, attached_entity);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

UI* GuiManager::CreateCursor(UI_ELEMENT element, int x, int y, bool is_visible, SDL_Rect* idle, SDL_Rect* clicked_idle
							, SDL_Rect* hover_ally, SDL_Rect* hover_enemy, SDL_Rect* hover_resource, SDL_Rect* hover_UI
							, SDL_Rect* clicked_ally, SDL_Rect* clicked_enemy, SDL_Rect* clicked_resource, SDL_Rect* clicked_UI)
{
	UI* elem = nullptr;

	elem = new UI_Cursor(element, x, y, is_visible, idle, clicked_idle, hover_ally, hover_enemy, hover_resource, clicked_ally, clicked_enemy, clicked_resource);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}


// ----------------------------------- UI ELEMENT DELETION METHODS -----------------------------------
void GuiManager::DestroyGuiElements()
{
	if (focused_element != nullptr)
	{
		focused_element = nullptr;
	}
	
	for (int i = 0; i < (int)elements.size(); ++i)
	{
		elements[i]->CleanUp();
		RELEASE(elements[i]);
	}

	elements.clear();
}

void GuiManager::DeleteGuiElement(UI* element_to_delete)
{
	std::vector<UI*>::iterator item = elements.begin();
	
	for (; item != elements.end(); ++item)
	{
		if ((*item) == element_to_delete)
		{
			(*item)->CleanUp();
			RELEASE((*item));

			elements.erase(item);

			return;
		}
	}
}

void GuiManager::LoadGuiElementsAudio()
{
	new_game_fx		= App->audio->LoadFx("audio/fx/UI/New Game_Continue.wav");
	options_fx		= App->audio->LoadFx("audio/fx/UI/Options.wav");
	back_fx			= App->audio->LoadFx("audio/fx/UI/Back.wav");
	exit_fx			= App->audio->LoadFx("audio/fx/UI/Back to menu.wav");
	appear_menu_fx	= App->audio->LoadFx("audio/fx/UI/Appear Pause Menu.wav");
	standard_fx		= App->audio->LoadFx("audio/fx/UI/Standard.wav");
	upgrade_fx		= App->audio->LoadFx("audio/fx/UI/Upgrade Button.wav");
	recruit_fx		= App->audio->LoadFx("audio/fx/UI/Recruit_Unit.wav");
}

void GuiManager::UnLoadGuiElementsAudio()
{

}

//--------------------------------- INPUT PROCESSING METHODS ---------------------------------
void GuiManager::OnEventCall(UI* element, UI_EVENT ui_event)
{
	BROFILER_CATEGORY("GUI_OnEventCall", Profiler::Color::NavajoWhite);


} 

// --- Method that will return true of there is a visible element under the mouse.
bool GuiManager::VisibleElementIsUnderCursor() const
{
	bool ret = false;

	for (int i = 0; i < elements.size(); ++i)
	{
		if (elements[i]->is_visible)
		{
			if (elements[i]->CheckMousePos() || elements[i]->CheckCursorPos())					// If the mouse is on the iterated UI_Element. // TMP CONTROLLER
			{
				ret = true;
				break;
			}
		}
	}

	return ret;
}

// --- Method that will return the foremost element of the UI under the Mouse. (First in inverse order of draw)
UI* GuiManager::FirstInteractibleElementUnderCursor() const
{
	UI* first_element = nullptr;

	/*std::vector<UI*>::const_iterator item = elements.cbegin();

	for (; item != elements.cend(); item++)
	{
		if (ElementCanBeClicked((*item)))														//Checks that the element being iterated has the mouse on it.
		{
			first_element = (*item);															//The element being iterated is assigned to first_element.
		}
	}*/

	std::vector<UI*>::const_reverse_iterator item = elements.crbegin();							// As what is looked for is the first element in inverse order of draw,
																								// we use a reverse_iterator to start from the end of the elements vector.
	for (; item != elements.crend(); ++item)
	{
		if (ElementCanBeClicked((*item)))
		{
			first_element = (*item);															// Since the first element that meets the criteria is the first element in
			break;																				// inverse order of draw, the loop can be stopped here.
		}
	}

	return first_element;
}

bool GuiManager::ElementCanBeClicked(UI* clickedElement) const
{
	bool ret = false;

	if ((clickedElement->CheckMousePos() || clickedElement->CheckCursorPos()) &&									//TMP CONTROLLER
			clickedElement->is_visible && (clickedElement->is_interactible || clickedElement->is_draggable))
	{
		ret = true;
	}
	
	return ret;
}

//----------------------------------- FOCUS MANAGEMENT METHODS -----------------------------------
// --- Method that will pass the focus from focuseable UI Element to the next.
void GuiManager::PassFocus()
{
	if (elements.size() != 0)
	{
		if (iterated_element == elements.end() || focused_element == nullptr)
		{
			iterated_element = elements.begin();

			for (; iterated_element != elements.end(); ++iterated_element)
			{
				if (ElementCanBeFocused((*iterated_element)))
				{
					focused_element = (*iterated_element);
					break;
				}
			}

			return;
		}

		for (; iterated_element != elements.end(); ++iterated_element)							//Loop that is used to find the next interactible element of the elments list.
		{
 			if (*next(iterated_element) != nullptr)												//If the next element of the list is not NULL.
			{
				if (ElementCanBeFocused(*next(iterated_element)))								//If the next element of the list fulfills all focus conditions.
				{
					focused_element = *next(iterated_element);									//UI* focusedElement is set with the UI* of the element next to the element being iterated. 
					++iterated_element;															//The element being iterated is set to the next element in the list.
					break;																		//The loop is stopped so the focus remains in the interactible element that now has the focus.
				}
			}
			else																				//If the next element of the list is NULL.
			{
				iterated_element = elements.end();												//The list_item is set to nullptr.
				focused_element = nullptr;			//Change for focusing first element again.	//The UI* focused element is set to nullptr, which efectively disables the focus.
				break;																			//The loop is stopped so no element regains the focus.

				/*iterated_element = elements.begin();

				for (; iterated_element != elements.end(); ++iterated_element)
				{
					if (ElementCanBeFocused((*iterated_element)))
					{
						focused_element = (*iterated_element);
						break;
					}
				}

				break;*/
			}
		}
	}
}

// --- Method that returns true if the passed element is a visible BUTTON, INPUTBOX or a SCROLLBAR
bool GuiManager::ElementCanBeFocused(UI* focusElement) const
{
	bool ret = false;

	if (focusElement->is_visible
		/*&& focusElement->isInteractible */
			&& (focusElement->element == UI_ELEMENT::BUTTON
			|| focusElement->element == UI_ELEMENT::SCROLLBAR
			|| focusElement->element == UI_ELEMENT::INPUTBOX))
	{
		ret = true;
	}

	return ret;
}

// --------------------------- PARENT/CHILD UI ELEMENTS METHODS --------------------------
// --- 
bool GuiManager::ElementHasChilds(UI* parentElement) const
{
	bool ret = false;

	std::vector<UI*>::const_iterator elem = elements.cbegin();

	for (; elem != elements.cend(); ++elem)
	{
		if ((*elem)->parent == parentElement)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

void GuiManager::UpdateChilds(UI* parentElement)
{
	std::vector<UI*>::iterator child = elements.begin();

	for (; child != elements.end(); ++child)
	{
		if ((*child)->parent == parentElement)
		{
			(*child)->previous_mouse_position = (*child)->parent->previous_mouse_position;				//The prevMousePos of the element being iterated is set with the parent's prev mouse pos.
			(*child)->DragElement();												//The child is also dragged, just as the parent.

			if (ElementHasChilds((*child)))											//If the first child also has child elements, then they are updated the same way.
			{
				UpdateChilds((*child));												//Recursive function, maybe avoid?
			}
		}
	}
}

void GuiManager::SetElementsVisibility(UI* parentElement, bool state)
{
	std::vector<UI*>::iterator child = elements.begin();

	for (; child != elements.end(); ++child)
	{
		if ((*child)->parent == parentElement)										//If the parent of the iterated element is parentElement.
		{
			(*child)->is_visible = state;											//Enables/Disables the iterated child's visibility. Changes isVisible from true to false and viceversa.

			if (ElementHasChilds((*child)))											//If the first child also has child elements, then they are updated the same way.
			{
				SetElementsVisibility((*child), state);								//Recursive function, maybe avoid?
			}
		}
	}

	parentElement->is_visible = state;												//Enables/Disables the parent element's visibility. Changes isVisible from true to false and viceversa.	
}

//----------------------------------- UI DEBUG METHOD -----------------------------------
void GuiManager::Debug_UI()
{
	if (ui_debug == true)
	{
		for (std::vector<UI*>::iterator elem = elements.begin(); elem != elements.end(); ++elem)
		{
			if ((*elem)->is_visible)
			{
				switch ((*elem)->element)
				{
				case UI_ELEMENT::EMPTY:
					App->render->DrawQuad((*elem)->GetHitbox(), 255, 255, 255, 255, false, false);		//UI_Empty will be WHITE.
					break;

				case UI_ELEMENT::IMAGE:
					App->render->DrawQuad((*elem)->GetHitbox(), 0, 0, 255, 255, false, false);			//UI_Image will be BLUE.

					break;

				case UI_ELEMENT::TEXT:
					App->render->DrawQuad((*elem)->GetHitbox(), 0, 255, 0, 255, false, false);			//UI_Text will be GREEN.

					break;

				case UI_ELEMENT::BUTTON:
					App->render->DrawQuad((*elem)->GetHitbox(), 255, 0, 0, 255, false, false);			//UI_Button will be RED.

					break;

				case UI_ELEMENT::INPUTBOX:
					App->render->DrawQuad((*elem)->GetHitbox(), 255, 0, 255, 255, false, false);		//UI_Input Box will be PURPLE.
					break;

				case UI_ELEMENT::SCROLLBAR:
					App->render->DrawQuad((*elem)->GetHitbox(), 255, 255, 0, 255, false, false);		//UI_Scrollbar will be YELLOW.
					break;

				case UI_ELEMENT::HEALTHBAR:
					App->render->DrawQuad((*elem)->GetHitbox(), 0, 255, 255, 255, false, true);			//UI_Healthbar will be CYAN.
					break;
				}
			}
		}
	}
}

void GuiManager::CreateGuiCommands()
{
	quit_command			= "quit";
	enable_ui_debug		= "enable_ui_debug";
	disable_ui_debug	= "disable_ui_debug";

	App->console->CreateCommand(quit_command, this, 1, 1);
	App->console->CreateCommand(enable_ui_debug, this, 1, 1);
	App->console->CreateCommand(disable_ui_debug, this, 1, 1);
}

void GuiManager::OnCommand(const char* command, const char* subCommand)
{	
	if (App->input->CmpStr(command, quit_command))
	{
		escape = false;
	}

	if (App->input->CmpStr(command, enable_ui_debug))
	{
		ui_debug = true;
	}

	if (App->input->CmpStr(command, disable_ui_debug))
	{
		ui_debug = false;
	}
}

// class Gui ---------------------------------------------------


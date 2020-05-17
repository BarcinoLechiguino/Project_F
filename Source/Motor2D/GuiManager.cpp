#include "Dependencies\Brofiler\Brofiler.h"

#include "Log.h"

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
#include "GuiElement.h"
#include "GuiImage.h"
#include "GuiText.h"
#include "GuiButton.h"
#include "GuiInputBox.h"
#include "GuiScrollbar.h"
#include "GuiHealthbar.h"
#include "GuiCreationBar.h"
#include "GuiCursor.h"

#include "GuiAnimation.h"
#include "GuiAnimationFade.h"
#include "GuiAnimationSlide.h"

GuiManager::GuiManager() : Module()
{
	name = ("gui");

	fonts_already_loaded = false;
	audio_already_loaded = false;
}

// Destructor
GuiManager::~GuiManager()
{}

// Called before render is available
bool GuiManager::Awake(pugi::xml_node& config)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = config.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool GuiManager::Start()
{
	atlas = App->tex->Load(atlas_file_name.c_str());

	ui_debug = false;
	escape = true;

	if (!fonts_already_loaded)
	{
		LoadGuiFonts();

		fonts_already_loaded = true;
	}
	
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

		//App->audio->PlayFx(new_game_button_clicked_fx, 0);
	}

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_STATE::KEY_DOWN 
		|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_A) == BUTTON_STATE::BUTTON_DOWN)								// TMP CONTROLLER
	{
		if (focused_element != nullptr && focused_element->is_interactible)
		{
			focused_element->ui_event = GUI_EVENT::UNCLICKED;
			
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

	for (int i = 0; i < (int)ui_animations.size(); ++i)
	{
		if (!ui_animations[i]->to_delete)
		{
			ui_animations[i]->StepAnimation();
		}
		else
		{
			DeleteUIAnimation(ui_animations[i]);
		}
	}

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
GuiElement* GuiManager::CreateImage(GUI_ELEMENT_TYPE type, int x, int y, SDL_Rect hitbox, bool is_visible, bool is_interactible, bool is_draggable, Module* listener, GuiElement* parent)
{
	BROFILER_CATEGORY("GUI_Image", Profiler::Color::NavajoWhite);
	GuiElement* elem = nullptr;

	elem = new GuiImage(type, x, y, hitbox, is_visible, is_interactible, is_draggable, listener, parent);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

GuiElement* GuiManager::CreateText(GUI_ELEMENT_TYPE type, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color font_colour, bool is_visible, bool is_interactible, bool is_draggable
	, Module* listener, GuiElement* parent, std::string* string, std::string* hover_string, std::string* left_click_string, std::string* right_click_string)
{
	BROFILER_CATEGORY("GUI_Text", Profiler::Color::NavajoWhite);
	GuiElement* elem = nullptr;

	elem = new GuiText(type, x, y, hitbox, font, font_colour, is_visible, is_interactible, is_draggable, listener, parent, string, hover_string, left_click_string, right_click_string);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

GuiElement* GuiManager::CreateButton(GUI_ELEMENT_TYPE type, int x, int y, bool is_visible, bool is_interactible, bool is_draggable, Module* listener, GuiElement* parent, SDL_Rect* idle, SDL_Rect* hover, SDL_Rect* clicked)
{
	BROFILER_CATEGORY("GUI_Button", Profiler::Color::NavajoWhite);
	GuiElement* elem = nullptr;

	elem = new GuiButton(type, x, y, is_visible, is_interactible, is_draggable, listener, parent, idle, hover, clicked);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

GuiElement* GuiManager::CreateInputBox(GUI_ELEMENT_TYPE type, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color font_colour, SDL_Rect cursor, SDL_Color cursor_colour, iPoint text_offset, 
					float blink_frequency, bool is_visible, bool is_interactible, bool is_draggable, Module* listener, GuiElement* parent, std::string* default_string, bool empty_elements)
{
	BROFILER_CATEGORY("GUI_InputBox", Profiler::Color::NavajoWhite);
	GuiElement* elem = nullptr;

	elem = new GuiInputBox(type, x, y, hitbox, font, font_colour, cursor, cursor_colour, text_offset, blink_frequency, is_visible, is_interactible, is_draggable, listener, parent,
					default_string, empty_elements);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

GuiElement* GuiManager::CreateScrollbar(GUI_ELEMENT_TYPE type, int x, int y, SDL_Rect hitbox, SDL_Rect thumb_size, iPoint thumb_offset , SDL_Rect drag_area, float drag_factor
								, bool drag_x_axis, bool drag_y_axis, bool inverted_scrolling, bool is_visible, bool is_interactible, bool is_draggable, Module* listener, GuiElement* parent
								, SDL_Rect* scroll_mask, iPoint mask_offset, bool empty_elements)
{
	BROFILER_CATEGORY("GUI_ScrollBar", Profiler::Color::NavajoWhite);
	GuiElement* elem = nullptr;

	elem = new GuiScrollbar(type, x, y, hitbox, thumb_size, thumb_offset, drag_area, drag_factor, drag_x_axis, drag_y_axis, inverted_scrolling,
					is_visible, is_interactible, is_draggable, listener, parent, scroll_mask, mask_offset, empty_elements);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

GuiElement* GuiManager::CreateHealthbar(GUI_ELEMENT_TYPE type, int x, int y, bool is_visible, SDL_Rect* healthbar, SDL_Rect* background, Entity* attached_entity)
{
	GuiElement* elem = nullptr;

	elem = new GuiHealthbar(type, x, y, is_visible, healthbar, background, attached_entity);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

GuiElement* GuiManager::CreateCreationBar(GUI_ELEMENT_TYPE type, int x, int y, bool is_visible, SDL_Rect* creation_bar, SDL_Rect* background, Entity* attached_entity)
{
	GuiElement* elem = nullptr;

	elem = new GuiCreationBar(type, x, y, is_visible, creation_bar, background, attached_entity);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

GuiElement* GuiManager::CreateCursor(GUI_ELEMENT_TYPE type, int x, int y, bool is_visible, SDL_Rect* idle, SDL_Rect* clicked_idle
							, SDL_Rect* hover_ally, SDL_Rect* hover_enemy, SDL_Rect* hover_resource, SDL_Rect* hover_UI
							, SDL_Rect* clicked_ally, SDL_Rect* clicked_enemy, SDL_Rect* clicked_resource, SDL_Rect* clicked_UI)
{
	GuiElement* elem = nullptr;

	elem = new GuiCursor(type, x, y, is_visible, idle, clicked_idle, hover_ally, hover_enemy, hover_resource, clicked_ally, clicked_enemy, clicked_resource);

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

void GuiManager::DeleteGuiElement(GuiElement* element_to_delete)
{
	std::vector<GuiElement*>::iterator item = elements.begin();
	
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

void GuiManager::LoadGuiFonts()
{
	std::string borgsquadcond = App->config_file.child("config").child("gui").child("fonts").child("borgsquadcond").attribute("path").as_string();

	borgsquadcond_12 = App->font->Load(borgsquadcond.c_str(), 12);
	borgsquadcond_15 = App->font->Load(borgsquadcond.c_str(), 15);
	borgsquadcond_20 = App->font->Load(borgsquadcond.c_str(), 20);
	borgsquadcond_25 = App->font->Load(borgsquadcond.c_str(), 25);
	borgsquadcond_30 = App->font->Load(borgsquadcond.c_str(), 30);
	borgsquadcond_40 = App->font->Load(borgsquadcond.c_str(), 40);
	borgsquadcond_50 = App->font->Load(borgsquadcond.c_str(), 50);
}

void GuiManager::LoadGuiElementsAudio()
{
	pugi::xml_node gui_fx = App->config_file.child("config").child("gui").child("fx");
	
	new_game_button_clicked_fx		= App->audio->LoadFx(gui_fx.child("new_game_button_clicked").attribute("path").as_string());
	options_button_clicked_fx		= App->audio->LoadFx(gui_fx.child("options_button_clicked").attribute("path").as_string());
	back_button_clicked_fx			= App->audio->LoadFx(gui_fx.child("back_button_clicked").attribute("path").as_string());
	main_menu_button_clicked_fx		= App->audio->LoadFx(gui_fx.child("main_menu_button_clicked").attribute("path").as_string());
	pause_menu_button_clicked_fx	= App->audio->LoadFx(gui_fx.child("pause_menu_button_clicked").attribute("path").as_string());
	standard_button_clicked_fx		= App->audio->LoadFx(gui_fx.child("standard_button_clicked").attribute("path").as_string());
	upgrade_button_clicked_fx		= App->audio->LoadFx(gui_fx.child("upgrade_button_clicked").attribute("path").as_string());
	recruit_unit_button_clicked_fx	= App->audio->LoadFx(gui_fx.child("recruit_unit_button_clicked").attribute("path").as_string());
}

void GuiManager::UnLoadGuiElementsAudio()
{

}

// --------------------------------- UI ANIMATION METHODS ---------------------------------
GuiAnimation* GuiManager::CreateFadeAnimation(GuiElement* element, float animation_duration, bool hide_on_completion, float start_alpha, float end_alpha)
{
	GuiAnimation* ui_animation = nullptr;
	
	if (!element->is_transitioning)
	{
		ui_animation = new GuiAnimationFade(element, animation_duration, hide_on_completion, start_alpha, end_alpha);

		if (ui_animation != nullptr)
		{
			ui_animations.push_back(ui_animation);
		}
	}
	else
	{
		CancelUIAnimation(element, GUI_ANIMATION_TYPE::FADE);
	}

	return ui_animation;
}

GuiAnimation* GuiManager::CreateSlideAnimation(GuiElement* element, float animation_duration, bool hide_on_completion, iPoint initial_position, iPoint final_position)
{
	GuiAnimation* ui_animation = nullptr;
	
	if (!element->is_transitioning)
	{
		ui_animation = new GuiAnimationSlide(element, animation_duration, hide_on_completion, initial_position, final_position);

		if (ui_animation != nullptr)
		{
			ui_animations.push_back(ui_animation);
		}
	}
	else
	{
		CancelUIAnimation(element, GUI_ANIMATION_TYPE::SLIDE);
	}

	return ui_animation;
}

void GuiManager::DeleteUIAnimation(GuiAnimation* ui_animation_to_delete)
{
	std::vector<GuiAnimation*>::iterator ui_animation = ui_animations.begin();

	for (; ui_animation != ui_animations.end(); ++ui_animation)
	{
		if ((*ui_animation) == ui_animation_to_delete)
		{
			(*ui_animation)->CleanUp();
			RELEASE((*ui_animation));

			ui_animations.erase(ui_animation);

			break;
		}
	}
}

void GuiManager::DeleteUIAnimation(std::vector<GuiAnimation*>::iterator animation_item)
{
	(*animation_item)->CleanUp();
	RELEASE((*animation_item));

	ui_animations.erase(animation_item);
}

void GuiManager::CancelUIAnimation(GuiElement* element_being_animated, GUI_ANIMATION_TYPE animation_type)
{
	std::vector<GuiAnimation*>::iterator ui_animation = ui_animations.begin();

	for (; ui_animation != ui_animations.end(); ++ui_animation)
	{
		if ((*ui_animation)->element == element_being_animated && (*ui_animation)->type == animation_type)	// If the element of the GuiAnimation* is the element passed as argument.
		{
			DeleteUIAnimation(ui_animation);
			break;
		}
	}
}

void GuiManager::DestroyUIAnimations()
{
	std::vector<GuiAnimation*>::iterator ui_animation = ui_animations.begin();

	for (; ui_animation != ui_animations.end(); ++ui_animation)
	{
		(*ui_animation)->CleanUp();
		RELEASE((*ui_animation));
	}

	ui_animations.clear();
}

// --------------------------------- INPUT PROCESSING METHODS ---------------------------------
void GuiManager::OnEventCall(GuiElement* element, GUI_EVENT ui_event)
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
GuiElement* GuiManager::FirstInteractibleElementUnderCursor() const
{
	GuiElement* first_element = nullptr;

	/*std::vector<UI*>::const_iterator item = elements.cbegin();

	for (; item != elements.cend(); item++)
	{
		if (ElementCanBeClicked((*item)))														//Checks that the element being iterated has the mouse on it.
		{
			first_element = (*item);															//The element being iterated is assigned to first_element.
		}
	}*/

	std::vector<GuiElement*>::const_reverse_iterator item = elements.crbegin();							// As what is looked for is the first element in inverse order of draw,
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

bool GuiManager::ElementCanBeClicked(GuiElement* clickedElement) const
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
bool GuiManager::ElementCanBeFocused(GuiElement* focus_element) const
{
	bool ret = false;

	if (focus_element->is_visible
		/*&& focus_element->isInteractible */
			&& (focus_element->element == GUI_ELEMENT_TYPE::BUTTON
			|| focus_element->element == GUI_ELEMENT_TYPE::SCROLLBAR
			|| focus_element->element == GUI_ELEMENT_TYPE::INPUTBOX))
	{
		ret = true;
	}

	return ret;
}

// --------------------------- PARENT/CHILD UI ELEMENTS METHODS --------------------------
// --- 
bool GuiManager::ElementHasChilds(GuiElement* parent_element) const
{
	bool ret = false;

	std::vector<GuiElement*>::const_iterator elem = elements.cbegin();

	for (; elem != elements.cend(); ++elem)
	{
		if ((*elem)->parent == parent_element)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

void GuiManager::UpdateChilds(GuiElement* parent_element)
{
	std::vector<GuiElement*>::iterator child = elements.begin();

	for (; child != elements.end(); ++child)
	{
		if ((*child)->parent == parent_element)
		{
			(*child)->previous_mouse_position = (*child)->parent->previous_mouse_position;		//The prevMousePos of the element being iterated is set with the parent's prev mouse pos.
			(*child)->DragElement();															//The child is also dragged, just as the parent.

			if (ElementHasChilds((*child)))														//If the first child also has child elements, then they are updated the same way.
			{
				UpdateChilds((*child));															//Recursive function, maybe avoid?
			}
		}
	}
}

void GuiManager::SetElementsVisibility(GuiElement* parent_element, bool state)
{
	std::vector<GuiElement*>::iterator child = elements.begin();

	for (; child != elements.end(); ++child)
	{
		if ((*child)->parent == parent_element)										// If the parent of the iterated element is parent_element.
		{																			   
			(*child)->is_visible = state;											// Enables/Disables the iterated child's visibility. Changes isVisible from true to false and viceversa.
																					   
			if (ElementHasChilds((*child)))											// If the first child also has child elements, then they are updated the same way.
			{																		   
				SetElementsVisibility((*child), state);								// Recursive function, maybe avoid?
			}
		}
	}

	parent_element->is_visible = state;												// Enables/Disables the parent element's visibility. Changes isVisible from true to false and viceversa.	
}																					   
																					   
std::vector<GuiElement*> GuiManager::GetElementChilds(GuiElement* parent_element)					   
{																					   
	std::vector<GuiElement*> childs;														   
																					   
	std::vector<GuiElement*>::iterator element = elements.begin();							   
																					   
	for (; element != elements.end(); ++element)									   
	{																				   
		if ((*element)->parent == parent_element)									   
		{																			   
			childs.push_back((*element));											   
																					   
			if (ElementHasChilds((*element)))										// If the first child also has child elements, then they are updated the same way.
			{																		   
				GetElementChilds((*element));										// Recursive function, maybe avoid?
			}
		}
	}

	return childs;
}

//----------------------------------- UI DEBUG METHOD -----------------------------------
void GuiManager::Debug_UI()
{
	if (ui_debug == true)
	{
		for (std::vector<GuiElement*>::iterator elem = elements.begin(); elem != elements.end(); ++elem)
		{
			if ((*elem)->is_visible)
			{
				switch ((*elem)->element)
				{
				case GUI_ELEMENT_TYPE::EMPTY:
					App->render->DrawQuad((*elem)->GetHitbox(), 255, 255, 255, 255, false, false);		//UI_Empty will be WHITE.
					break;

				case GUI_ELEMENT_TYPE::IMAGE:
					App->render->DrawQuad((*elem)->GetHitbox(), 0, 0, 255, 255, false, false);			//GuiImage will be BLUE.

					break;

				case GUI_ELEMENT_TYPE::TEXT:
					App->render->DrawQuad((*elem)->GetHitbox(), 0, 255, 0, 255, false, false);			//GuiText will be GREEN.

					break;

				case GUI_ELEMENT_TYPE::BUTTON:
					App->render->DrawQuad((*elem)->GetHitbox(), 255, 0, 0, 255, false, false);			//GuiButton will be RED.

					break;

				case GUI_ELEMENT_TYPE::INPUTBOX:
					App->render->DrawQuad((*elem)->GetHitbox(), 255, 0, 255, 255, false, false);		//UI_Input Box will be PURPLE.
					break;

				case GUI_ELEMENT_TYPE::SCROLLBAR:
					App->render->DrawQuad((*elem)->GetHitbox(), 255, 255, 0, 255, false, false);		//GuiScrollbar will be YELLOW.
					break;

				case GUI_ELEMENT_TYPE::HEALTHBAR:
					App->render->DrawQuad((*elem)->GetHitbox(), 0, 255, 255, 255, false, true);			//GuiHealthbar will be CYAN.
					break;

				case GUI_ELEMENT_TYPE::CREATIONBAR:
					App->render->DrawQuad((*elem)->GetHitbox(), 0, 0, 0, 255, false, true);
					break;

				case GUI_ELEMENT_TYPE::CURSOR:
					App->render->DrawQuad((*elem)->GetHitbox(), 255, 102, 0, false, false);
					break;
				}
			}
		}
	}
}

void GuiManager::CreateGuiCommands()
{
	quit_command		= "quit";
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


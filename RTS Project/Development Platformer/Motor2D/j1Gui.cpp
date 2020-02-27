#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Audio.h"
#include "j1FadeScene.h"
#include "j1Scene.h"
#include "j1Console.h"
#include "j1Gui.h"

#include "Brofiler\Brofiler.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
	audioAlreadyLoaded = false;
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.GetString());

	if (atlas == NULL)
	{
		LOG("Texture could not be loaded");
	}

	ui_debug = false;
	escape = true;

	
	if (!audioAlreadyLoaded)
	{
		tab_fx = App->audio->LoadFx("audio/fx/tab_ui.wav");
		play_fx = App->audio->LoadFx("audio/fx/play_ui.wav");
		save_fx = App->audio->LoadFx("audio/fx/save_fx.wav");
		exit_fx = App->audio->LoadFx("audio/fx/exit_ui.wav");
		nav_fx = App->audio->LoadFx("audio/fx/navegate_ui.wav");

		audioAlreadyLoaded = true;
	}

	CreateGuiCommands();

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
	{
		PassFocus();
		App->audio->PlayFx(tab_fx, 0);
	}

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		if (focusedElement != nullptr && focusedElement->isInteractible)
		{
			focusedElement->ui_event = UI_Event::UNCLICKED;
			OnEventCall(focusedElement, focusedElement->ui_event);
		}
	}


	if (App->scene->background_image->isVisible)
	{
		App->pause = true;
	}
	
	if(App->scene->main_in_menu->isVisible)
	{
		App->pause = true;
	}

	if (game_started) 
	{
		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		{
			Mix_HaltMusic();
			SetElementsVisibility(App->scene->main_in_menu, !App->scene->main_in_menu->isVisible);
			App->audio->PlayMusic(App->scene->music_path3.GetString());
			if (!App->scene->main_in_menu->isVisible)
			{
				App->pause = false;
				
			}
		}
	}
	

	if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN)
	{
		SetElementsVisibility(App->console->console_background, !App->console->console_background->isVisible);
		SetElementsVisibility(App->scene->upper_bar, !App->scene->upper_bar->isVisible);

		
		App->input->ClearTextInput();

		if (App->console->console_background->isVisible)
		{
			focusedElement = App->console->console_input;
		}
	}
	
	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{	

	BROFILER_CATEGORY("GUI_Update", Profiler::Color::NavajoWhite);
	//escape = true;
	App->console->DrawBackgroundElement();		//THIS HERE


	for (p2List_item<UI*>* element_iterator = elements.start; element_iterator != NULL; element_iterator = element_iterator->next)
	{
		switch (element_iterator->data->element)
		{
		case UI_Element::IMAGE:
			if (element_iterator->data->isVisible)
			{
				element_iterator->data->Draw();
			}

			break;

		case UI_Element::TEXT:
			if (element_iterator->data->isVisible)
			{
				element_iterator->data->Draw();
			}

			break;

		case UI_Element::BUTTON:
			if (element_iterator->data->isVisible)
			{
				element_iterator->data->Draw();
			}

			break;

		case UI_Element::SCROLLBAR:
			if (element_iterator->data->isVisible)
			{
				element_iterator->data->Draw();
			}

			break;

		case UI_Element::INPUTBOX:
			if (element_iterator->data->isVisible)
			{
				element_iterator->data->Draw();
			}
			
			break;
		}
	}

	Debug_UI();

	return escape;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	//Iterate the elements list and frees all allocated memory.
	for (p2List_item<UI*>* element_iterator = elements.start; element_iterator != NULL; element_iterator = element_iterator->next)
	{
		element_iterator->data->CleanUp();
		RELEASE(element_iterator->data);
	}

	elements.clear();

	return true;
}

// const getter for atlas
/*const*/ SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

//----------------------------------- UI ELEMENT CREATION METHODS -----------------------------------
UI* j1Gui::CreateImage(UI_Element element, int x, int y, SDL_Rect hitbox, bool isVisible, bool isInteractible, bool isDraggable, UI* parent)
{

	BROFILER_CATEGORY("GUI_Image", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_Image(element, x, y, hitbox, isVisible, isInteractible, isDraggable, parent);

	if (elem != nullptr)
	{
		elements.add(elem);
	}

	return elem;
}

UI* j1Gui::CreateText(UI_Element element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, bool isVisible, bool isInteractible, bool isDraggable,
					UI* parent, p2SString* string, p2SString* hoverString, p2SString* leftClickString, p2SString* rightClickString)
{
	BROFILER_CATEGORY("GUI_Text", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_Text(element, x, y, hitbox, font, fontColour, isVisible, isInteractible, isDraggable, parent, string, hoverString, leftClickString, rightClickString);

	if (elem != nullptr)
	{
		elements.add(elem);
	}

	return elem;
}

UI* j1Gui::CreateButton(UI_Element element, int x, int y, bool isVisible, bool isInteractible, bool isDraggable, UI* parent, SDL_Rect* idle, SDL_Rect* hover, SDL_Rect* clicked)
{
	BROFILER_CATEGORY("GUI_Button", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_Button(element, x, y, isVisible, isInteractible, isDraggable, parent, idle, hover, clicked);

	if (elem != nullptr)
	{
		elements.add(elem);
	}

	return elem;
}

UI* j1Gui::CreateInputBox(UI_Element element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, SDL_Rect cursor, SDL_Color cursorColour, iPoint textOffset, 
					float blinkFrequency, bool isVisible, bool isInteractible, bool isDraggable, UI* parent, p2SString* defaultString, bool emptyElements)
{
	BROFILER_CATEGORY("GUI_InputBox", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_InputBox(element, x, y, hitbox, font, fontColour, cursor, cursorColour, textOffset, blinkFrequency, isVisible, isInteractible, isDraggable, parent,
					defaultString, emptyElements);

	if (elem != nullptr)
	{
		elements.add(elem);
	}

	return elem;
}

UI* j1Gui::CreateScrollbar(UI_Element element, int x, int y, SDL_Rect hitbox, SDL_Rect thumbSize, iPoint thumbOffset, SDL_Rect dragArea, float dragFactor, bool dragXAxis, bool dragYAxis,
					bool invertedScrolling, bool isVisible, bool isInteractible, bool isDraggable, UI* parent, SDL_Rect* scrollMask, iPoint maskOffset, bool emptyElements)
{
	BROFILER_CATEGORY("GUI_ScrollBar", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_Scrollbar(element, x, y, hitbox, thumbSize, thumbOffset, dragArea, dragFactor, dragXAxis, dragYAxis, invertedScrolling,
					isVisible, isInteractible, isDraggable, parent, scrollMask, maskOffset, emptyElements);

	if (elem != nullptr)
	{
		elements.add(elem);
	}

	return elem;
}

//--------------------------------- INPUT PROCESSING METHODS ---------------------------------
void j1Gui::OnEventCall(UI* element, UI_Event ui_event)
{
	BROFILER_CATEGORY("GUI_OnEventCall", Profiler::Color::NavajoWhite);
	// Main Menu
	if (element == App->scene->main_button_play && ui_event == UI_Event::UNCLICKED)
	{
		Mix_HaltMusic();
		SetElementsVisibility(App->scene->main_window, !App->scene->main_window->isVisible);
		SetElementsVisibility(App->scene->background_image, !App->scene->background_image->isVisible);
		SetElementsVisibility(App->scene->upper_bar, !App->scene->upper_bar->isVisible);

		if (!App->scene->background_image->isVisible)
		{
			App->pause = false;
		}

		game_started = true;
		App->audio->PlayFx(play_fx, 0);
		App->audio->PlayMusic(App->scene->music_path.GetString());
	}

	if (element == App->scene->main_button_continue && ui_event == UI_Event::UNCLICKED)
	{
		Mix_HaltMusic();
		App->LoadGame("save_game.xml");
		SetElementsVisibility(App->scene->main_window, !App->scene->main_window->isVisible);
		SetElementsVisibility(App->scene->background_image, !App->scene->background_image->isVisible);
		SetElementsVisibility(App->scene->upper_bar, !App->scene->upper_bar->isVisible);
		game_started = true;

		if (!App->scene->background_image->isVisible)
		{
			App->pause = false;
		}

		App->audio->PlayFx(play_fx, 0);
		App->audio->PlayMusic(App->scene->music_path.GetString());
	}

	if (element == App->scene->main_button_settings && ui_event == UI_Event::UNCLICKED)
	{
		SetElementsVisibility(App->scene->main_settings_menu, !App->scene->main_settings_menu->isVisible);
		SetElementsVisibility(App->scene->main_window, !App->scene->main_window->isVisible);
		App->audio->PlayFx(nav_fx, 0);
	}

	if (element == App->scene->main_button_exit && ui_event == UI_Event::UNCLICKED)
	{
		escape = false;
		App->audio->PlayFx(exit_fx, 0);
	}
	//-------------------------------------------------------------------------------------------------------------

	// Settings menu
	if (element == App->scene->back && ui_event == UI_Event::UNCLICKED)
	{
		SetElementsVisibility(App->scene->main_settings_menu, !App->scene->main_settings_menu->isVisible);
		SetElementsVisibility(App->scene->main_window, !App->scene->main_window->isVisible);
		App->audio->PlayFx(nav_fx, 0);
	}

	if (element == App->scene->unmute && ui_event == UI_Event::UNCLICKED)
	{
		App->audio->volume = 100;
		App->audio->PlayFx(nav_fx, 0);
	}

	if (element == App->scene->mute && ui_event == UI_Event::UNCLICKED)
	{
		App->audio->volume = 0;
		App->audio->PlayFx(nav_fx, 0);
	}

	if (element == App->scene->check && ui_event == UI_Event::UNCLICKED)
	{
		if (App->framesAreCapped == true)
		{
			App->framesAreCapped = false;
		}
		else
		{
			App->framesAreCapped = true;
		}

		App->audio->PlayFx(nav_fx, 0);
	}

	if (element == App->scene->credits && ui_event == UI_Event::UNCLICKED)
	{
		SetElementsVisibility(App->scene->main_settings_menu, !App->scene->main_settings_menu->isVisible);
		SetElementsVisibility(App->scene->main_credits_menu, !App->scene->main_credits_menu->isVisible);
		App->audio->PlayFx(nav_fx, 0);
	}

	if (element == App->scene->github && ui_event == UI_Event::UNCLICKED)
	{
		ShellExecuteA(NULL, "open", "https://gromeu2000.wixsite.com/mutualcooperation", NULL, NULL, SW_SHOWNORMAL);
		App->audio->PlayFx(nav_fx, 0);
	}
	//---------------------------------------------------------------------------------------------------------------------

	// Credits menu
	if (element == App->scene->back_credits && ui_event == UI_Event::UNCLICKED)
	{
		SetElementsVisibility(App->scene->main_settings_menu, !App->scene->main_settings_menu->isVisible);
		SetElementsVisibility(App->scene->main_credits_menu, !App->scene->main_credits_menu->isVisible);
		App->audio->PlayFx(nav_fx, 0);
	}
	//----------------------------------------------------------------------------------------------------------------------

	// In-game menu
	if (element == App->scene->in_buttons_resume && ui_event == UI_Event::UNCLICKED)
	{
		Mix_HaltMusic();
		SetElementsVisibility(App->scene->main_in_menu, !App->scene->main_in_menu->isVisible);

		if (!App->scene->main_in_menu->isVisible)
		{
			App->pause = false;
		}

		App->audio->PlayFx(play_fx, 0);
		App->audio->PlayMusic(App->scene->music_path.GetString());
	}

	if (element == App->scene->in_buttons_save && ui_event == UI_Event::UNCLICKED)
	{
		App->SaveGame("save_game.xml");
		App->audio->PlayFx(save_fx, 0);
	}

	if (element == App->scene->in_buttons_load && ui_event == UI_Event::UNCLICKED)
	{
		Mix_HaltMusic();
		SetElementsVisibility(App->scene->main_in_menu, !App->scene->main_in_menu->isVisible);
		
		if (!App->scene->main_in_menu->isVisible)
		{
			App->pause = false;
		}
		
		App->LoadGame("save_game.xml");
		App->audio->PlayFx(play_fx, 0);
		App->audio->PlayMusic(App->scene->music_path.GetString());
	}

	if (element == App->scene->in_buttons_exit && ui_event == UI_Event::UNCLICKED)
	{
		Mix_HaltMusic();
		SetElementsVisibility(App->scene->main_in_menu, !App->scene->main_in_menu->isVisible);
		SetElementsVisibility(App->scene->main_window, !App->scene->main_window->isVisible);
		SetElementsVisibility(App->scene->background_image, !App->scene->background_image->isVisible);
		SetElementsVisibility(App->scene->upper_bar, !App->scene->upper_bar->isVisible);
		game_started = false;
		
		if (!App->scene->main_in_menu->isVisible)
		{
			App->pause = false;
		}
		
		App->audio->PlayFx(exit_fx, 0);
		App->audio->PlayMusic(App->scene->music_path2.GetString());
	}

	if (element == App->scene->unmute_in && ui_event == UI_Event::UNCLICKED)
	{
		App->audio->volume = 100;
		App->audio->PlayFx(nav_fx, 0);
	}

	if (element == App->scene->mute_in && ui_event == UI_Event::UNCLICKED)
	{
		App->audio->volume = 0;
		App->audio->PlayFx(nav_fx, 0);
	}
	//----------------------------------------------------------------------------------------------------------------------
} 

// --- Method to return the foremost element of the UI. (First in inverse order of draw)
UI* j1Gui::FirstElementUnderMouse() const
{
	UI* firstElement = nullptr;

	for (p2List_item<UI*>* iterator = elements.start; iterator != NULL; iterator = iterator->next)
	{
		if (ElementCanBeClicked(iterator->data))													//Checks that the element being iterated has the mouse on it.
		{
			firstElement = iterator->data;															//The element being iterated is assigned to firstElement.
		}
	}

	if (firstElement != nullptr)
	{
		return firstElement;																		//The last element that was checked to have the mouse on it will be returned.
	}
}

bool j1Gui::ElementCanBeClicked(UI* clickedElement) const
{
	bool ret = false;

	if (clickedElement->CheckMousePos()
		&& clickedElement->isVisible
		&& (clickedElement->isInteractible
			|| clickedElement->isDraggable))
	{
		ret = true;
	}
	
	return ret;
}

//----------------------------------- FOCUS MANAGEMENT METHODS -----------------------------------
// --- Method that will pass the focus from focuseable UI Element to the next.
void j1Gui::PassFocus()
{
	if (iteratedElement == nullptr)
	{
		iteratedElement = elements.start;
	}
	
	if (focusedElement == nullptr)
	{
		for (iteratedElement; iteratedElement != NULL; iteratedElement = iteratedElement->next)		//Loop that is used to find the first interactible element of the elments list.
		{
			if (ElementCanBeFocused(iteratedElement->data))											//If the element being iterated fulfills all focus conditions.
			{
				focusedElement = iteratedElement->data;												//UI* focusedElement is set with the UI* of the element being iterated.
				break;																				//The loop is stopped.
			}
		}

		return;																						//Stops the function here so the focus remains in the first interactible element.
	}

	for (iteratedElement; iteratedElement != NULL; iteratedElement = iteratedElement->next)			//Loop that is used to find the next interactible element of the elments list.
	{
		if (iteratedElement->next != NULL)															//If the next element of the list is not NULL.
		{
			if (ElementCanBeFocused(iteratedElement->next->data))									//If the next element of the list fulfills all focus conditions.
			{
				focusedElement = iteratedElement->next->data;										//UI* focusedElement is set with the UI* of the element next to the element being iterated. 
				iteratedElement = iteratedElement->next;											//The element being iterated is set to the next element in the list.
				break;																				//The loop is stopped so the focus remains in the interactible element that now has the focus.
			}
		}
		else																						//If the next element of the list is NULL.
		{
			iteratedElement = nullptr;																//The list_item is set to nullptr.
			focusedElement = nullptr;																//The UI* focused element is set to nullptr, which efectively disables the focus.
			break;																					//The loop is stopped so no element regains the focus.
		}
	}
}

// --- Method that returns true if the passed element is a visible BUTTON, INPUTBOX or a SCROLLBAR
bool j1Gui::ElementCanBeFocused(UI* focusElement) const
{
	bool ret = false;

	if (focusElement->isVisible
		/*&& focusElement->isInteractible */
			&& (focusElement->element == UI_Element::BUTTON
			|| focusElement->element == UI_Element::SCROLLBAR
			|| focusElement->element == UI_Element::INPUTBOX))
	{
		ret = true;
	}

	return ret;
}

// --------------------------- PARENT/CHILD UI ELEMENTS METHODS --------------------------
// --- 
bool j1Gui::ElementHasChilds(UI* parentElement) const
{
	bool ret = false;
	
	for (p2List_item<UI*>* iterator = elements.start; iterator != NULL; iterator = iterator->next)
	{
		if (iterator->data->parent == parentElement)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

void j1Gui::UpdateChilds(UI* parentElement)
{
	//Check ElementHasChilds() here instead than in the elements?
	for (p2List_item<UI*>* child = elements.start; child != NULL; child = child->next)
	{
		if (child->data->parent == parentElement)
		{
			child->data->prevMousePos = child->data->parent->prevMousePos;			//The prevMousePos of the element being iterated is set with the parent's prev mouse pos.
			child->data->DragElement();												//The child is also dragged, just as the parent.

			if (ElementHasChilds(child->data))										//If the first child also has child elements, then they are updated the same way.
			{
				UpdateChilds(child->data);											//Recursive function, maybe avoid?
			}

			/*child->data->SetScreenPos(child->data->GetLocalPos() + child->data->parent->GetScreenPos());
			child->data->SetHitbox({ child->data->GetScreenPos().x, child->data->GetScreenPos().y , child->data->GetHitbox().w, child->data->GetHitbox().h});*/
		}
	}
}

void j1Gui::SetElementsVisibility(UI* parentElement, bool state)
{	
	for (p2List_item<UI*>* child = elements.start; child != NULL; child = child->next)
	{
		if (child->data->parent == parentElement)									//If the parent of the iterated element is parentElement.
		{
			child->data->isVisible = state;											//Enables/Disables the iterated child's visibility. Changes isVisible from true to false and viceversa.

			if (ElementHasChilds(child->data))										//If the first child also has child elements, then they are updated the same way.
			{
				SetElementsVisibility(child->data, state);							//Recursive function, maybe avoid?
			}
		}
	}

	//parentElement->isVisible = !parentElement->isVisible;							//Enables/Disables the parent element's visibility. Changes isVisible from true to false and viceversa.	
	parentElement->isVisible = state;												//Enables/Disables the parent element's visibility. Changes isVisible from true to false and viceversa.	
}

//----------------------------------- UI DEBUG METHOD -----------------------------------
void j1Gui::Debug_UI()
{
	if (ui_debug == true)
	{
		for (p2List_item<UI*>* element_iterator = elements.start; element_iterator != NULL; element_iterator = element_iterator->next)
		{	
			if (element_iterator->data->isVisible)
			{
				switch (element_iterator->data->element)
				{
				case UI_Element::EMPTY:
					App->render->DrawQuad(element_iterator->data->GetHitbox(), 255, 255, 255, 255, false, false);		//UI_Empty will be WHITE.
						break;
				
				case UI_Element::IMAGE:
					App->render->DrawQuad(element_iterator->data->GetHitbox(), 0, 0, 255, 255, false, false);			//UI_Image will be BLUE.

					break;

				case UI_Element::TEXT:
					App->render->DrawQuad(element_iterator->data->GetHitbox(), 0, 255, 0, 255, false, false);			//UI_Text will be GREEN.

					break;

				case UI_Element::BUTTON:
					App->render->DrawQuad(element_iterator->data->GetHitbox(), 255, 0, 0, 255, false, false);			//UI_Button will be RED.

					break;

				case UI_Element::SCROLLBAR:
					App->render->DrawQuad(element_iterator->data->GetHitbox(), 255, 255, 0, 255, false, false);			//UI_Scrollbar will be YELLOW.
					break;

				case UI_Element::INPUTBOX:
					App->render->DrawQuad(element_iterator->data->GetHitbox(), 255, 0, 255, 255, false, false);			//UI_Input Box will be PURPLE.
					break;
				}
			}
		}
	}
}

void j1Gui::CreateGuiCommands()
{
	quitCommand			= "quit";
	enable_ui_debug		= "enable_ui_debug";
	disable_ui_debug	= "disable_ui_debug";

	App->console->CreateCommand(quitCommand, this, 1, 1);
	App->console->CreateCommand(enable_ui_debug, this, 1, 1);
	App->console->CreateCommand(disable_ui_debug, this, 1, 1);
}

void j1Gui::OnCommand(const char* command, const char* subCommand)
{	
	if (App->input->CmpStr(command, quitCommand))
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


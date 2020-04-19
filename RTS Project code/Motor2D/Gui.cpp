#include "p2Log.h"

#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Fonts.h"
#include "Console.h"
#include "Entity.h"

#include "Gui.h"
#include "UI.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_InputBox.h"
#include "UI_Scrollbar.h"
#include "UI_Healthbar.h"

#include "Brofiler\Brofiler.h"

Gui::Gui() : Module()
{
	name = ("gui");
	audioAlreadyLoaded = false;
}

// Destructor
Gui::~Gui()
{}

// Called before render is available
bool Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool Gui::Start()
{
	if (atlas == nullptr)
	{
		atlas = App->tex->Load(atlas_file_name.c_str());
	}

	ui_debug = false;
	escape = true;

	
	if (!audioAlreadyLoaded)
	{
		new_game_fx = App->audio->LoadFx("audio/fx/UI/New Game_Continue.wav");
		options_fx = App->audio->LoadFx("audio/fx/UI/Options.wav");
		back_fx = App->audio->LoadFx("audio/fx/UI/Back.wav");
		exit_fx = App->audio->LoadFx("audio/fx/UI/Back to menu.wav");
		appear_menu_fx = App->audio->LoadFx("audio/fx/UI/Appear Pause Menu.wav");
		standard_fx = App->audio->LoadFx("audio/fx/UI/Standard.wav");
		upgrade_fx = App->audio->LoadFx("audio/fx/UI/Upgrade Button.wav");
		recruit_fx = App->audio->LoadFx("audio/fx/UI/Recruit_Unit.wav");

		audioAlreadyLoaded = true;
	}

	//CreateGuiCommands();

	return true;
}

// Update all guis
bool Gui::PreUpdate()
{
	/*if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)						// Does not work, for now.
	{
		PassFocus();
		//App->audio->PlayFx(new_game_fx, 0);
	}

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)					// Does not work, for now.
	{
		if (focusedElement != nullptr && focusedElement->isInteractible)
		{
			focusedElement->ui_event = UI_EVENT::UNCLICKED;
			focusedElement->listener->OnEventCall(focusedElement, focusedElement->ui_event);
		}
	}*/
	
	/*if(App->scene->main_in_menu->isVisible)
	{
		App->pause = true;
	}*/

	if (game_started) 
	{
		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		{
			Mix_HaltMusic();
			//SetElementsVisibility(App->scene1->main_in_menu, !App->scene->main_in_menu->isVisible);
			////App->audio->PlayMusic(App->scene->music_path3.c_str());
			//if (!App->scene1->main_in_menu->isVisible)
			//{
			//	App->pause = false;
			//	
			//}
		}
	}
	

	//if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN)			//  Bring this to console.cpp?
	//{
	//	SetElementsVisibility(App->console->console_background, !App->console->console_background->isVisible);
	//	//SetElementsVisibility(App->scene->upper_bar, !App->scene->upper_bar->isVisible);							//As it does not currently exist it CTDs on a nullptr

	//	
	//	App->input->ClearTextInput();

	//	if (App->console->console_background->isVisible)
	//	{
	//		focusedElement = App->console->console_input;
	//	}
	//}
	//
	return true;
}

// Called after all Updates
bool Gui::PostUpdate()
{	

	BROFILER_CATEGORY("GUI_Update", Profiler::Color::NavajoWhite);
	//escape = true;
	
	//App->console->DrawBackgroundElement();		//THIS HERE CONSOLE

	for (int i=0; i < elements.size(); i++)
	{
		if (elements[i]->isVisible)
		{
			elements[i]->Draw();
		}
	}

	Debug_UI();

	return escape;
}

// Called before quitting
bool Gui::CleanUp()
{
	LOG("Freeing GUI");
	
	//Iterate the elements list and frees all allocated memory.
	for (std::vector<UI*>::iterator element_iterator = elements.begin(); element_iterator != elements.end(); element_iterator++)
	{
		(*element_iterator)->CleanUp();
		RELEASE((*element_iterator));
	}

	elements.clear();

	App->tex->UnLoad(atlas);
	atlas = nullptr;

	return true;
}

// const getter for atlas
/*const*/ SDL_Texture* Gui::GetAtlas() const
{
	return atlas;
}

//----------------------------------- UI ELEMENT CREATION METHODS -----------------------------------
UI* Gui::CreateImage(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, bool isVisible, bool isInteractible, bool isDraggable, Module* listener, UI* parent)
{
	BROFILER_CATEGORY("GUI_Image", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_Image(element, x, y, hitbox, isVisible, isInteractible, isDraggable, listener, parent);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

UI* Gui::CreateText(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, bool isVisible, bool isInteractible, bool isDraggable
	, Module* listener, UI* parent, std::string* string, std::string* hoverString, std::string* leftClickString, std::string* rightClickString)
{
	BROFILER_CATEGORY("GUI_Text", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_Text(element, x, y, hitbox, font, fontColour, isVisible, isInteractible, isDraggable, listener, parent, string, hoverString, leftClickString, rightClickString);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

UI* Gui::CreateButton(UI_ELEMENT element, int x, int y, bool isVisible, bool isInteractible, bool isDraggable, Module* listener, UI* parent, SDL_Rect* idle, SDL_Rect* hover, SDL_Rect* clicked)
{
	BROFILER_CATEGORY("GUI_Button", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_Button(element, x, y, isVisible, isInteractible, isDraggable, listener, parent, idle, hover, clicked);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

UI* Gui::CreateInputBox(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, _TTF_Font* font, SDL_Color fontColour, SDL_Rect cursor, SDL_Color cursorColour, iPoint textOffset, 
					float blinkFrequency, bool isVisible, bool isInteractible, bool isDraggable, Module* listener, UI* parent, std::string* defaultString, bool emptyElements)
{
	BROFILER_CATEGORY("GUI_InputBox", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_InputBox(element, x, y, hitbox, font, fontColour, cursor, cursorColour, textOffset, blinkFrequency, isVisible, isInteractible, isDraggable, listener, parent,
					defaultString, emptyElements);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

UI* Gui::CreateScrollbar(UI_ELEMENT element, int x, int y, SDL_Rect hitbox, SDL_Rect thumbSize, iPoint thumbOffset, SDL_Rect dragArea, float dragFactor, bool dragXAxis, bool dragYAxis,
					bool invertedScrolling, bool isVisible, bool isInteractible, bool isDraggable, Module* listener, UI* parent, SDL_Rect* scrollMask, iPoint maskOffset, bool emptyElements)
{
	BROFILER_CATEGORY("GUI_ScrollBar", Profiler::Color::NavajoWhite);
	UI* elem = nullptr;

	elem = new UI_Scrollbar(element, x, y, hitbox, thumbSize, thumbOffset, dragArea, dragFactor, dragXAxis, dragYAxis, invertedScrolling,
					isVisible, isInteractible, isDraggable, listener, parent, scrollMask, maskOffset, emptyElements);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

UI* Gui::CreateHealthbar(UI_ELEMENT element, int x, int y, bool is_visible, SDL_Rect* healthbar, SDL_Rect* background, Entity* attached_unit, Module* listener, UI* parent)
{
	UI* elem = nullptr;

	elem = new UI_Healthbar(element, x, y, is_visible, healthbar, background, attached_unit, listener, parent);

	if (elem != nullptr)
	{
		elements.push_back(elem);
	}

	return elem;
}

void Gui::DeleteGuiElement(UI* element_to_delete)
{
	for (std::vector<UI*>::iterator element_iterator = elements.begin(); element_iterator != elements.end(); element_iterator++)
	{
		if ((*element_iterator) == element_to_delete)
		{
			(*element_iterator)->CleanUp();
			RELEASE((*element_iterator));

			elements.erase(element_iterator);

			return;
		}
	}
}

//--------------------------------- INPUT PROCESSING METHODS ---------------------------------
void Gui::OnEventCall(UI* element, UI_EVENT ui_event)
{
	BROFILER_CATEGORY("GUI_OnEventCall", Profiler::Color::NavajoWhite);


} 

// --- Method to return the foremost element of the UI. (First in inverse order of draw)
UI* Gui::FirstElementUnderMouse() const
{
	UI* firstElement = nullptr;

	std::vector<UI*>::const_iterator iterator = elements.cbegin();

	for (; iterator != elements.cend(); iterator++)
	{
		if (ElementCanBeClicked((*iterator)))													//Checks that the element being iterated has the mouse on it.
		{
			firstElement = (*iterator);															//The element being iterated is assigned to firstElement.
		}
	}

	if (firstElement != nullptr)
	{
		return firstElement;																	//The last element that was checked to have the mouse on it will be returned.
	}
}

bool Gui::ElementCanBeClicked(UI* clickedElement) const
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
void Gui::PassFocus()
{
	if (elements.size() != 0)
	{
		if (iteratedElement == elements.end())
		{
			iteratedElement = elements.begin();
		}

		if (focusedElement == nullptr)
		{
			for (iteratedElement; iteratedElement != elements.end(); ++iteratedElement)			//Loop that is used to find the first interactible element of the elments list.
			{
				if (ElementCanBeFocused((*iteratedElement)))									//If the element being iterated fulfills all focus conditions.
				{
					focusedElement = (*iteratedElement);										//UI* focusedElement is set with the UI* of the element being iterated.
					break;																		//The loop is stopped.
				}
			}

			return;																				//Stops the function here so the focus remains in the first interactible element.
		}

		for (iteratedElement; iteratedElement != elements.end(); ++iteratedElement)				//Loop that is used to find the next interactible element of the elments list.
		{
			if (*next(iteratedElement) != nullptr)												//If the next element of the list is not NULL.
			{
				if (ElementCanBeFocused(*next(iteratedElement)))								//If the next element of the list fulfills all focus conditions.
				{
					focusedElement = *next(iteratedElement);									//UI* focusedElement is set with the UI* of the element next to the element being iterated. 
					++iteratedElement;															//The element being iterated is set to the next element in the list.
					break;																		//The loop is stopped so the focus remains in the interactible element that now has the focus.
				}
			}
			else																				//If the next element of the list is NULL.
			{
				iteratedElement = elements.end();												//The list_item is set to nullptr.
				focusedElement = nullptr;														//The UI* focused element is set to nullptr, which efectively disables the focus.
				break;																			//The loop is stopped so no element regains the focus.
			}
		}
	}
}

// --- Method that returns true if the passed element is a visible BUTTON, INPUTBOX or a SCROLLBAR
bool Gui::ElementCanBeFocused(UI* focusElement) const
{
	bool ret = false;

	if (focusElement->isVisible
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
bool Gui::ElementHasChilds(UI* parentElement) const
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

void Gui::UpdateChilds(UI* parentElement)
{
	std::vector<UI*>::iterator child = elements.begin();

	for (; child != elements.end(); ++child)
	{
		if ((*child)->parent == parentElement)
		{
			(*child)->prevMousePos = (*child)->parent->prevMousePos;				//The prevMousePos of the element being iterated is set with the parent's prev mouse pos.
			(*child)->DragElement();												//The child is also dragged, just as the parent.

			if (ElementHasChilds((*child)))											//If the first child also has child elements, then they are updated the same way.
			{
				UpdateChilds((*child));												//Recursive function, maybe avoid?
			}
		}
	}
}

void Gui::SetElementsVisibility(UI* parentElement, bool state)
{
	std::vector<UI*>::iterator child = elements.begin();

	for (; child != elements.end(); ++child)
	{
		if ((*child)->parent == parentElement)										//If the parent of the iterated element is parentElement.
		{
			(*child)->isVisible = state;											//Enables/Disables the iterated child's visibility. Changes isVisible from true to false and viceversa.

			if (ElementHasChilds((*child)))											//If the first child also has child elements, then they are updated the same way.
			{
				SetElementsVisibility((*child), state);								//Recursive function, maybe avoid?
			}
		}
	}

	parentElement->isVisible = state;												//Enables/Disables the parent element's visibility. Changes isVisible from true to false and viceversa.	
}

//----------------------------------- UI DEBUG METHOD -----------------------------------
void Gui::Debug_UI()
{
	if (ui_debug == true)
	{
		for (std::vector<UI*>::iterator elem = elements.begin(); elem != elements.end(); ++elem)
		{
			if ((*elem)->isVisible)
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

void Gui::CreateGuiCommands()
{
	quitCommand			= "quit";
	enable_ui_debug		= "enable_ui_debug";
	disable_ui_debug	= "disable_ui_debug";

	App->console->CreateCommand(quitCommand, this, 1, 1);
	App->console->CreateCommand(enable_ui_debug, this, 1, 1);
	App->console->CreateCommand(disable_ui_debug, this, 1, 1);
}

void Gui::OnCommand(const char* command, const char* subCommand)
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


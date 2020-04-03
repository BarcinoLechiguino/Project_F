#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "Main_Menu.h"
#include "Gui.h"

Main_Menu::Main_Menu() : Scene(SCENES::MAIN_MENU)
{
	name_tag = ("mainmenu");
}

// Destructor
Main_Menu::~Main_Menu()
{

}

// Called before render is available
bool Main_Menu::Awake(pugi::xml_node&)
{
	return true;
}

// Called before the first frame
bool Main_Menu::Start()
{
	LoadGuiElements();

	//App->gui->SetElementsVisibility(main_in_menu, !main_in_menu->isVisible);

	return true;
}

// Called before all Updates
bool Main_Menu::PreUpdate()
{
	//play_button = App->gui->CreateButton(UI_Element::BUTTON,100,100,true,true,false,nullptr,)

	return true;
}

// Called each loop iteration
bool Main_Menu::Update(float dt)
{
	return true;
}

// Called before all Updates
bool Main_Menu::PostUpdate()
{
	//Load Scene unload Main Menu
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
	{
		ExecuteTransition();
	}

	
	return true;
}

// Called before quitting
bool Main_Menu::CleanUp()
{
	App->gui->CleanUp();
	
	return true;
}

void Main_Menu::LoadGuiElements()
{
	// Temporal transition button
	int win_width;
	int win_height;
	
	iPoint window_center;

	App->win->GetWindowSize(win_width, win_height);

	window_center = iPoint(win_width * 0.5f, win_height * 0.5f);

	SDL_Rect button_size				= { 0, 0, 230, 18 };
	SDL_Rect transition_button_idle		= { 0, 202, 230, 18 };
	SDL_Rect transition_button_hover	= { 0, 224, 230, 18 };
	SDL_Rect transition_button_clicked	= { 0, 158, 230, 18 };
	
	iPoint center_button = iPoint(window_center.x - button_size.w * 0.5, window_center.y - button_size.h * 0.5f);

	transition_button = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, center_button.x, center_button.y, true, true, false, this, nullptr
													, &transition_button_idle, &transition_button_hover, &transition_button_clicked);

	SDL_Rect text_rect = { 0, 0, 40, 9 };
	_TTF_Font* font = App->font->Load("fonts/Minecraftia-Regular.ttf", 12);
	std::string button_string = "Transition";

	button_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, center_button.x + text_rect.w, center_button.y + 3, text_rect, font
													, SDL_Color{ 0, 0, 0, 255 }, true, false, false, nullptr, transition_button, &button_string);

	// ----------------------------------------------

	//In-game menu
	//SDL_Rect main_in_rect{ 0, 388, 466, 447 };
	//main_in_menu = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 280, 180, main_in_rect, false, false, false, nullptr, nullptr);

	//SDL_Rect label1_rect_in{ 1078,242,382,61 };
	//label_1_in = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, 323, 160, label1_rect_in, false, false, false, nullptr,nullptr, main_in_menu);

	//_TTF_Font* font_in_esc = App->font->Load("fonts/Minecraftia-Regular.ttf", 32);
	//std::string string_title_in = "PAUSE MENU";
	////in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 410, 166, textHitbox, font_in_esc, fontRgb, false, false, false, main_in_menu, &string_title_in);

	//SDL_Rect in_rect = { 1057,30,173,45 };
	//SDL_Rect idle_in = { 1057,30,173,45 };
	//SDL_Rect hover_in = { 1057,76,173,45 };
	//SDL_Rect clicked_in = { 1057,126,173,45 };
	//in_buttons_resume = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 430, 247, false, true, false, this, main_in_menu, &idle_in, &hover_in, &clicked_in);
	//in_buttons_save = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 430, 317, false, true, false, this, main_in_menu, &idle_in, &hover_in, &clicked_in);
	//in_buttons_load = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 430, 387, false, true, false, this, main_in_menu, &idle_in, &hover_in, &clicked_in);
	//in_buttons_exit = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 430, 457, false, true, false, this, main_in_menu, &idle_in, &hover_in, &clicked_in);

	//SDL_Rect unmute_rect_in = { 512,147,57,57 };
	//SDL_Rect idle_u_in = { 512,147,57,57 };
	//SDL_Rect hover_u_in = { 512,267,57,57 };
	//SDL_Rect clicked_u_in = { 1479,72,57,57 };
	//unmute_in = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 670, 550, false, true, false, this, main_in_menu, &idle_u_in, &hover_u_in, &clicked_u_in);
	//SDL_Rect mute_rect_in = { 1479, 9, 57, 57 };
	//SDL_Rect idle_m_in = { 1479, 9, 57, 57 };
	//SDL_Rect hover_m_in = { 1544,9,57,57 };
	//SDL_Rect clicked_m_in = { 440,214,57,57 };
	//mute_in = (UI_Button*)App->gui->CreateButton(UI_Element::BUTTON, 300, 550, false, true, false, this, main_in_menu, &idle_m_in, &hover_m_in, &clicked_m_in);

	//_TTF_Font* font_in_button = App->font->Load("fonts/Future Now.ttf", 30);
	//std::string string_play_in_button = "RESUME";
	//std::string string_save_in_button = "SAVE";
	//std::string string_load_in_button = "LOAD";
	//std::string string_exit_in_button = "EXIT";
	///*button_in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 470, 257, textHitbox, font_in_button, fontRgb, false, false, false, main_in_menu, &string_play_in_button);
	//button_in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 485, 327, textHitbox, font_in_button, fontRgb, false, false, false, main_in_menu, &string_save_in_button);
	//button_in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 485, 397, textHitbox, font_in_button, fontRgb, false, false, false, main_in_menu, &string_load_in_button);
	//button_in_text = (UI_Text*)App->gui->CreateText(UI_Element::TEXT, 495, 467, textHitbox, font_in_button, fontRgb, false, false, false, main_in_menu, &string_exit_in_button);*/

	//firstScrollPosCalc = false;
	//secondScrollPosCalc = false;
}

void Main_Menu::OnEventCall(UI* element, UI_Event ui_event)
{
	if (element == in_buttons_resume && ui_event == UI_Event::UNCLICKED)
	{
		App->transition_manager->CreateExpandingBars(SCENES::FIRST_SCENE, 0.5f, true, 5, true, true);
	}

	if (element == transition_button && ui_event == UI_Event::UNCLICKED)
	{
		App->transition_manager->CreateExpandingBars(SCENES::FIRST_SCENE, 0.5f, true, 5, true, true);
	}
}

void Main_Menu::ExecuteTransition()
{
	App->transition_manager->CreateExpandingBars(SCENES::NONE, 0.5f, true, 5, true, true);
}

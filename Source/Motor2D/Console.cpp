#include "Brofiler\Brofiler.h"

#include "p2Defs.h"
#include "p2Log.h"

#include "Application.h"
#include "Render.h"
#include "Fonts.h"
#include "Input.h"
#include "Map.h"
#include "EntityManager.h"

#include "GuiManager.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_InputBox.h"
#include "UI_Scrollbar.h"

#include "Console.h"

Console::Console() : Module(), command_was_found(false)
{
	name = ("console");
}

Console::~Console()
{

}

bool Console::Awake(pugi::xml_node& config)
{
	//config_file = config;

	return true;
}

bool Console::Start()
{
	/*InitConsole();
	CreateConsoleElements();
	CreateConsoleCommands();*/

	return true;
}

bool Console::PreUpdate()
{	
	BROFILER_CATEGORY("Console_PreUpdate", Profiler::Color::PowderBlue);


	/*if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		command_was_found = false;
		
		std::vector<Command*>::iterator command = commands.begin();

		for (; command != commands.end(); ++command)
		{
			Command* comm = (*command);

			if (App->input->CmpStr(App->input->GetInputText(), comm->command.c_str()))
			{
				comm->callback->OnCommand(comm->command.c_str());
				command_history.push_back(comm);
				command_was_found = true;
				break;
			}
		}
		
		if (!command_was_found)
		{
			console_output->RefreshTextInput("Error: COMMAND NOT FOUND");
		}

		App->input->ClearTextInput();
	}*/

	//if (ConsoleIsOpen())
	//{
	//	if (App->gui->focusedElement != console_input)
	//	{
	//		App->gui->focusedElement = console_input;
	//	}
	//}
	
	return true;
}

bool Console::Update(float dt)
{

	return true;
}

bool Console::PostUpdate()
{
	/*DrawBackgroundElement();

	if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN)
	{
		App->gui->SetElementsVisibility(console_background, !console_background->isVisible);
		//SetElementsVisibility(App->scene->upper_bar, !App->scene->upper_bar->isVisible);							//As it does not currently exist it CTDs on a nullptr


		App->input->ClearTextInput();

		if (console_background->isVisible)
		{
			App->gui->focusedElement = console_input;
		}
	}*/

	
	return true;
}

bool Console::CleanUp()
{
	std::vector<Command*>::iterator command = commands.begin();

	for (; command != commands.end(); ++command)
	{
		RELEASE((*command));
	}

	command_history.clear();
	commands.clear();
	
	return true;
}

void Console::InitConsole()
{
	config_file.load_file("config.xml");

	console = config_file.child("config").child("console");

	// --- LOADING THE CONSOLE'S VARIABLES
	// --- BACKGROUND
	bg_position.x = console.child("bg_position").attribute("x").as_int();
	bg_position.y = console.child("bg_position").attribute("y").as_int();

	bg_rect.x = console.child("bg_rect").attribute("x").as_int();
	bg_rect.y = console.child("bg_rect").attribute("y").as_int();
	bg_rect.w = console.child("bg_rect").attribute("w").as_int();
	bg_rect.h = console.child("bg_rect").attribute("h").as_int();

	bg_colour.r = console.child("bg_colour").attribute("r").as_int();
	bg_colour.g = console.child("bg_colour").attribute("g").as_int();
	bg_colour.b = console.child("bg_colour").attribute("b").as_int();
	bg_colour.a = console.child("bg_colour").attribute("a").as_int();

	bg_is_visible = console.child("bg_isVisible").attribute("value").as_bool();
	bg_is_interactible = console.child("bg_isInteractible").attribute("value").as_bool();
	bg_is_draggable = console.child("bg_isDraggable").attribute("value").as_bool();

	// --- OUTPUT
	output_position.x = console.child("output_position").attribute("x").as_int();
	output_position.y = console.child("output_position").attribute("y").as_int();

	output_rect.x = console.child("output_rect").attribute("x").as_int();
	output_rect.y = console.child("output_rect").attribute("y").as_int();
	output_rect.w = console.child("output_rect").attribute("w").as_int();
	output_rect.h = console.child("output_rect").attribute("h").as_int();
	
	output_font_path = console.child("output_font_path").attribute("path").as_string("");
	output_font_size = console.child("output_font_size").attribute("size").as_int();

	output_font_colour.r = console.child("output_font_colour").attribute("r").as_int();
	output_font_colour.g = console.child("output_font_colour").attribute("g").as_int();
	output_font_colour.b = console.child("output_font_colour").attribute("b").as_int();
	output_font_colour.a = console.child("output_font_colour").attribute("a").as_int();

	output_is_visible = console.child("output_isVisible").attribute("value").as_bool();
	output_is_interactible = console.child("output_isInteractible").attribute("value").as_bool();;
	output_is_draggable = console.child("output_isDraggable").attribute("value").as_bool();

	// --- INPUT
	input_position.x = console.child("input_position").attribute("x").as_int();
	input_position.y = console.child("input_position").attribute("y").as_int();

	input_rect.x = console.child("input_rect").attribute("x").as_int();
	input_rect.y = console.child("input_rect").attribute("y").as_int();
	input_rect.w = console.child("input_rect").attribute("w").as_int();
	input_rect.h = console.child("input_rect").attribute("h").as_int();

	input_font_path = console.child("input_font_path").attribute("path").as_string("");
	input_font_size = console.child("input_font_size").attribute("size").as_int();

	input_font_colour.r = console.child("input_font_colour").attribute("r").as_int();
	input_font_colour.g = console.child("input_font_colour").attribute("g").as_int();
	input_font_colour.b = console.child("input_font_colour").attribute("b").as_int();
	input_font_colour.a = console.child("input_font_colour").attribute("a").as_int();

	cursor_rect.x = console.child("cursor_rect").attribute("x").as_int();
	cursor_rect.y = console.child("cursor_rect").attribute("y").as_int();
	cursor_rect.w = console.child("cursor_rect").attribute("w").as_int();
	cursor_rect.h = console.child("cursor_rect").attribute("h").as_int();

	cursor_colour.r = console.child("cursor_colour").attribute("r").as_int();
	cursor_colour.g = console.child("cursor_colour").attribute("g").as_int();
	cursor_colour.b = console.child("cursor_colour").attribute("b").as_int();
	cursor_colour.a = console.child("cursor_colour").attribute("a").as_int();

	input_text_offset.x = console.child("input_text_offset").attribute("x").as_int();
	input_text_offset.y = console.child("input_text_offset").attribute("y").as_int();

	cursor_blink_frequency = console.child("cursor_blinkFrequency").attribute("frequency").as_float();
	input_is_visible = console.child("input_isVisible").attribute("value").as_bool();
	input_is_interactible = console.child("input_isInteractible").attribute("value").as_bool();
	input_is_draggable = console.child("input_isDraggable").attribute("value").as_bool();

	// --- SCROLLBAR
	scroll_position.x = console.child("scroll_position").attribute("x").as_int();
	scroll_position.y = console.child("scroll_position").attribute("y").as_int();

	scrollbar_rect.x = console.child("scrollbar_rect").attribute("x").as_int();
	scrollbar_rect.y = console.child("scrollbar_rect").attribute("y").as_int();
	scrollbar_rect.w = console.child("scrollbar_rect").attribute("w").as_int();
	scrollbar_rect.h = console.child("scrollbar_rect").attribute("h").as_int();

	thumb_rect.x = console.child("thumb_rect").attribute("x").as_int();
	thumb_rect.y = console.child("thumb_rect").attribute("y").as_int();
	thumb_rect.w = console.child("thumb_rect").attribute("w").as_int();
	thumb_rect.h = console.child("thumb_rect").attribute("h").as_int();

	thumb_offset.x = console.child("thumb_offset").attribute("x").as_int();
	thumb_offset.y = console.child("thumb_offset").attribute("y").as_int();

	drag_area.x = console.child("drag_area").attribute("x").as_int();
	drag_area.y = console.child("drag_area").attribute("y").as_int();
	drag_area.w = console.child("drag_area").attribute("w").as_int();
	drag_area.h = console.child("drag_area").attribute("h").as_int();

	drag_factor = console.child("drag_factor").attribute("factor").as_float();
	drag_x_axis = console.child("drag_x_axis").attribute("value").as_bool();
	drag_y_axis = console.child("drag_y_axis").attribute("value").as_bool();
	inverted_scrolling = console.child("inverted_scrolling").attribute("value").as_bool();
	scroll_is_visible = console.child("scroll_isVisible").attribute("value").as_bool();
	scroll_is_interactible = console.child("scroll_isInteractible").attribute("value").as_bool();
	scroll_is_draggable = console.child("scroll_isDraggable").attribute("value").as_bool();
}

void Console::CreateConsoleElements()
{
	output_font = App->font->Load(output_font_path.c_str(), output_font_size);
	input_font = App->font->Load(input_font_path.c_str(), input_font_size);

	std::string defaultString = "DefaultString";

	console_background = (UI_Image*)App->gui_manager->CreateImage(UI_ELEMENT::EMPTY, bg_position.x, bg_position.y, bg_rect, bg_is_visible, bg_is_interactible, bg_is_draggable, nullptr, nullptr);
	
	console_output = (UI_Text*)App->gui_manager->CreateText(UI_ELEMENT::TEXT, output_position.x, output_position.y, output_rect, output_font, output_font_colour
												, output_is_visible, output_is_interactible, output_is_draggable, nullptr, console_background);

	console_input = (UI_InputBox*)App->gui_manager->CreateInputBox(UI_ELEMENT::INPUTBOX, input_position.x, (console_background->GetHitbox().h - input_rect.h), input_rect, input_font, input_font_colour
												, cursor_rect, cursor_colour, input_text_offset, cursor_blink_frequency, input_is_visible, input_is_interactible, input_is_draggable
												, nullptr, console_background, &defaultString, true);

	console_scroll = (UI_Scrollbar*)App->gui_manager->CreateScrollbar(UI_ELEMENT::SCROLLBAR, scroll_position.x, scroll_position.y, scrollbar_rect, thumb_rect, thumb_offset, drag_area, drag_factor
												, drag_x_axis, drag_y_axis, inverted_scrolling, scroll_is_visible, scroll_is_draggable, scroll_is_interactible, nullptr, console_background
												, NULL, iPoint(0, 0), true);

	console_scroll->LinkScroll(console_output);

	if (console_background->is_visible)
	{
		App->gui_manager->focused_element = console_input;
	}
}

void Console::DrawBackgroundElement()
{
	if (console_background->is_visible)
	{
		App->render->DrawQuad(console_background->GetHitbox(), bg_colour.r, bg_colour.g, bg_colour.b, bg_colour.a, true, false);
	}
}

bool Console::ConsoleIsOpen()
{
	return console_background->is_visible;

	return false;
}

Command::Command(const char* command, Module* callback, int min_arg, int max_arg)
{
	this->command = command;
	this->callback = callback;
	this->min_arg = min_arg;
	this->max_arg = max_arg;
}

Command* Console::CreateCommand(const char* command, Module* callback, int min_arg, int max_arg)
{
	Command* comm = nullptr;

	comm = new Command(command, callback, min_arg, max_arg);

	if (comm != nullptr)
	{
		commands.push_back(comm);
	}

	return comm;
}

void Console::CreateConsoleCommands()
{
	//CreateCommand("quit", this, 1, 1);

	// Console Commands
	command_list		= "list";
	
	// App Commands
	enable_pause		= "enable_pause";
	disable_pause		= "disable_pause";
	enable_frame_cap	= "enable_frame_cap";
	disable_frame_cap	= "disable_frame_cap";
	reset_frame_cap		= "reset_frame_cap";
	FPS_30				= "FPS 30";									//EVERYTHING IS A LIE. JUST SMOKE AND MIRRORS.
	FPS_60				= "FPS 60";
	FPS_120				= "FPS 120";

	// Debug Commands
	enable_god_mode		= "enable_god_mode";
	disable_god_mode	= "disable_god_mode";
	
	CreateCommand(command_list, this, 1, 1);
	
	CreateCommand(enable_pause, this, 1, 1);
	CreateCommand(disable_pause, this, 1, 1);
	CreateCommand(enable_frame_cap, this, 1, 1);
	CreateCommand(disable_frame_cap, this, 1, 1);
	CreateCommand(reset_frame_cap, this, 1, 1);
	CreateCommand(FPS_30, this, 1, 1);
	CreateCommand(FPS_60, this, 1, 1);
	CreateCommand(FPS_120, this, 1, 1);

	CreateCommand(enable_god_mode, this, 1, 1);
	CreateCommand(disable_god_mode, this, 1, 1);
}

void Console::OnCommand(const char* command, const char* subCommand)
{
	// --- CONSOLE COMMANDS
	if (App->input->CmpStr(command, command_list))
	{
		ShellExecuteA(NULL, "open", "https://gromeu2000.wixsite.com/mutualcooperation", NULL, NULL, SW_SHOWNORMAL);
	}
	
	// --- APP COMMANDS
	if (App->input->CmpStr(command, enable_pause))						// -----------------------------------------------------------------------------------
	{
		App->pause = true;												// Enable Pause Mode
	}
	if (App->input->CmpStr(command, disable_pause))						// -----------------------------------------------------------------------------------
	{
		App->pause = false;												// Disable Pause Mode
	}
	if (App->input->CmpStr(command, enable_frame_cap))					// -----------------------------------------------------------------------------------
	{
		App->frames_are_capped = true;									// Enable Frame Cap
	}
	if (App->input->CmpStr(command, disable_frame_cap))					// -----------------------------------------------------------------------------------
	{
		App->frames_are_capped = false;									// Disable Frame Cap
	}
	if (App->input->CmpStr(command, reset_frame_cap))					// -----------------------------------------------------------------------------------
	{
		App->frame_cap = App->original_frame_cap;						// Reset Frame Cap
	}
	if (App->input->CmpStr(command, FPS_30))							// -----------------------------------------------------------------------------------
	{
		App->frame_cap = CAP_AT_30;										// Cap Frames at 30
	}
	if (App->input->CmpStr(command, FPS_60))							// -----------------------------------------------------------------------------------
	{
		App->frame_cap = CAP_AT_60;										// Cap Frames at 60
	}
	if (App->input->CmpStr(command, FPS_120))							// -----------------------------------------------------------------------------------
	{
		App->frame_cap = CAP_AT_120;									// Cap Frames at 120
	}
}
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "Module.h"

struct _TTF_Font;

class GuiElement;
class GuiImage;
class GuiText;
class GuiInputBox;
class GuiScrollbar;

class Command
{
public:
	Command(const char* command, Module* callback, int min_arg, int max_arg);
	
	std::string command;	//"map iso.tmx" --> The string is separated at the space and the first string will be the name of the command
							//and the second one are the number of arguments that the command has (speed would have 1 argument).
	uint n_arguments;

	int min_arg;
	int max_arg;

	Module* callback;
};

class Console : public Module
{
public:
	Console();

	~Console();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

public:
	void InitConsole();
	void CreateConsoleElements();
	void CreateConsoleCommands();
	void DrawBackgroundElement();
	bool ConsoleIsOpen();
	Command* CreateCommand(const char* command, Module* callback, int min_arg, int max_arg);
	void OnCommand(const char* command, const char* sub_command = nullptr);
	//void DrawConsoleElements();

public:
	GuiImage*				console_background;
	GuiText*				console_output;
	GuiInputBox*			console_input;
	GuiScrollbar*			console_scroll;

	iPoint					bg_position;
	SDL_Rect				bg_rect;
	SDL_Color				bg_colour;
	bool					bg_is_visible;
	bool					bg_is_interactible;
	bool					bg_is_draggable;

	iPoint					output_position;
	SDL_Rect				output_rect;
	std::string				output_font_path;
	_TTF_Font*				output_font;
	int						output_font_size;
	SDL_Color				output_font_colour;
	bool					output_is_visible;
	bool					output_is_interactible;
	bool					output_is_draggable;

	iPoint					input_position;
	SDL_Rect				input_rect;
	std::string				input_font_path;
	_TTF_Font*				input_font;
	int						input_font_size;
	SDL_Color				input_font_colour;
	SDL_Rect				cursor_rect;
	SDL_Color				cursor_colour;
	iPoint					input_text_offset;
	float					cursor_blink_frequency;
	bool					input_is_visible;
	bool					input_is_interactible;
	bool					input_is_draggable;

	iPoint					scroll_position;
	SDL_Rect				scrollbar_rect;
	SDL_Rect				thumb_rect;
	iPoint					thumb_offset;
	SDL_Rect				drag_area;
	float					drag_factor;
	bool					drag_x_axis;
	bool					drag_y_axis;
	bool					inverted_scrolling;
	bool					scroll_is_visible;
	bool					scroll_is_interactible;
	bool					scroll_is_draggable;


	bool					run_game;
	bool					command_was_found;
	std::vector<Command*>	commands;
	std::vector<Command*>	command_history;

private:
	pugi::xml_document		config_file;
	pugi::xml_node			console;

	// Console Strings
	const char*				command_list;
	const char*				enable_pause;
	const char*				disable_pause;
	const char*				enable_frame_cap;
	const char*				disable_frame_cap;
	const char*				reset_frame_cap;
	const char*				FPS_30;
	const char*				FPS_45;
	const char*				FPS_60;
	const char*				FPS_90;
	const char*				FPS_120;

	const char*				enable_god_mode;
	const char*				disable_god_mode;
};

#endif // !__CONSOLE_H__
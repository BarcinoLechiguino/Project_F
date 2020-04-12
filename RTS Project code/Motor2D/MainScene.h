#include "SDL/include/SDL_rect.h"
#include "Module.h"
#include "Scene.h"

class UI;
class UI_Image;
class UI_Text;
class UI_Button;
class UI_InputBox;
class UI_Scrollbar;

class MainScene : public Scene
{
public:

	MainScene();
	virtual ~MainScene();				// Destructor

	bool Awake(pugi::xml_node&);		// Called before render is available
	bool Start();						// Called before the first frame
	bool PreUpdate();					// Called before all Updates
	bool Update(float dt);				// Called each loop iteration
	bool PostUpdate();					// Called before all Updates
	bool CleanUp();						// Called before quitting

public:

	void LoadGuiElements();

	void OnEventCall(UI* element, UI_EVENT ui_event);

	void ExecuteTransition();

	void LoadOptionsMenu();

public:
	// Main Screen
	UI_Button*	new_game_button;
	UI_Button*	continue_button;
	UI_Button*	options_button;
	UI_Button*	exit_button;

	// Main BACKGROUND
	UI_Image*		background;
	SDL_Rect		background_rect;
	SDL_Texture*	background_texture;

	// Options BACKGROUND
	UI_Image*		background_opt;
	SDL_Rect		background_rect_opt;
	SDL_Texture*	background_texture_opt;

	UI_Button*		back_button;

	//Text:
	UI_Text*		resolution_text;
	UI_Text*		sfx_text;
	UI_Text*		music_text;
	UI_Text*		options_text;


public:
};
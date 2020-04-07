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

	void OnEventCall(UI* element, UI_Event ui_event);

	void ExecuteTransition();

public:
	// Main Screen
	UI_Button* new_game_button;
	UI_Button* continue_button;
	UI_Button* options_button;
	UI_Button* exit_button;
	UI_Text* button_text;

	// tmp BACKGROUND
	UI_Image*		background;
	SDL_Rect		background_rect;
	SDL_Texture*	background_texture;

public:
};
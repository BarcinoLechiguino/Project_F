#include "SDL/include/SDL_rect.h"
#include "Module.h"
#include "Scene.h"

class UI;
class UI_Image;
class UI_Text;
class UI_Button;
class UI_InputBox;
class UI_Scrollbar;

class Main_Menu : public Scene
{
public:

	Main_Menu();
	virtual ~Main_Menu();				// Destructor

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

	UI_Button* play_button;
	SDL_Rect   play_button_idle;
	SDL_Rect   play_button_hover;
	SDL_Rect   play_button_clicked;

public:
	UI_Image* background;
	SDL_Rect				background_rect;
	SDL_Texture* background_texture;

	// In-game menu
	UI_Image* main_in_menu;
	UI_Image* label_1_in;

	UI_Text* in_text;
	UI_Text* button_in_text;

	UI_Button* in_buttons_resume;
	UI_Button* in_buttons_save;
	UI_Button* in_buttons_load;
	UI_Button* in_buttons_exit;
	UI_Button* unmute_in;
	UI_Button* mute_in;

	UI_Scrollbar* scrollbar_in;

	iPoint					firstOriginalPos;
	iPoint					secondOrigianlPos;

	bool					firstScrollPosCalc;
	bool					secondScrollPosCalc;

};
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

	UI_Button* play_button;
	SDL_Rect   play_button_idle;
	SDL_Rect   play_button_hover;
	SDL_Rect   play_button_clicked;

public:
	// TMP TRANSITION BUTTON
	UI_Button* transition_button;
	UI_Text* button_text;
	// ---------------------
};
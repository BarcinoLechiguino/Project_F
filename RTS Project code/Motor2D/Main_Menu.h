#include "Module.h"
#include "SDL/include/SDL_rect.h"
#include <vector>

class UI;
class UI_Image;
class UI_Text;
class UI_Button;
class UI_InputBox;
class UI_Scrollbar;

class Main_Menu : public Module
{
public:

	Main_Menu();

	// Destructor
	virtual ~Main_Menu();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	UI_Button* play_button;
	SDL_Rect   play_button_idle;
	SDL_Rect   play_button_hover;
	SDL_Rect   play_button_clicked;

};
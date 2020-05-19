#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "Scene.h"

class GuiElement;
class GuiImage;
class GuiText;
class GuiButton;
class GuiInputBox;
class GuiScrollbar;

class MainMenuScene : public Scene
{
public:

	MainMenuScene();
	virtual ~MainMenuScene();			// Destructor

	bool Awake(pugi::xml_node&);		// Called before render is available
	bool Start();						// Called before the first frame
	bool PreUpdate();					// Called before all Updates
	bool Update(float dt);				// Called each loop iteration
	bool PostUpdate();					// Called before all Updates
	bool CleanUp();						// Called before quitting

public:
	void InitScene();

	void LoadGuiElements();

	void OnEventCall(GuiElement* element, GUI_EVENT ui_event);

	void ExecuteTransition();

public:
	bool transition_to_gameplay_scene;					// Will keep track whether or not the transition to the gameplay scene has been triggered.
	bool transition_to_options_scene;					// Will keep track whether or not the transition to the options scene has been triggered.
	
	//BACKGROUND
	SDL_Rect		background_rect;
	SDL_Texture*	background_texture;

	// Main Screen
	GuiImage*		main_parent;
	GuiButton*		new_game_button;
	GuiButton*		continue_button;
	GuiButton*		options_button;
	GuiButton*		exit_button;

	//Audio
	uint				menu_song;

};

#endif // !__MAIN_SCENE_H__
#ifndef __OPTIONS_SCENE_H__
#define __OPTIONS_SCENE_H__

#include "Scene.h"

class UI;
class UI_Image;
class UI_Text;
class UI_Button;
class UI_InputBox;
class UI_Scrollbar;

class OptionsScene : public Scene
{
public:

	OptionsScene();
	virtual ~OptionsScene();			// Destructor

	bool Awake(pugi::xml_node&);		// Called before render is available
	bool Start();						// Called before the first frame
	bool PreUpdate();					// Called before all Updates
	bool Update(float dt);				// Called each loop iteration
	bool PostUpdate();					// Called before all Updates
	bool CleanUp();						// Called before quitting

public:
	void InitScene();

	void LoadGuiElements();

	void OnEventCall(UI* element, UI_EVENT ui_event);

	void UpdateVolumeThumbPosition();
	void UpdateFXVolumeThumbPosition();

	void AdjustVolumeWithScrollbar();

	void ExecuteTransition();

public:
	bool transition_to_main_menu_scene;					// Will keep track whether or not the transition to the main menu scene has been triggered.
	
	//BACKGROUND
	SDL_Rect		background_rect;
	SDL_Texture*	background_texture;

	// Options

	UI_Image* options_parent;

	//bool			options_created;
	UI_Button*		fullscreen_off;
	UI_Button*		fullscreen_on;
	UI_Button*		back_button;
	UI_Scrollbar*	music_scrollbar;
	UI_Scrollbar*	sfx_scrollbar;

	//Text:
	UI_Text*		resolution_text;
	UI_Text*		sfx_text;
	UI_Text*		music_text;
	UI_Text*		options_text;

	//Audio
	uint			menu_song;
};

#endif // !__OPTIONS_SCENE_H__

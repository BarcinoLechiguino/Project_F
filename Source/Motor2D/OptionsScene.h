#ifndef __OPTIONS_SCENE_H__
#define __OPTIONS_SCENE_H__

#include "Scene.h"

class GuiElement;
class GuiImage;
class GuiText;
class GuiButton;
class GuiInputBox;
class GuiScrollbar;

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

	void OnEventCall(GuiElement* element, GUI_EVENT ui_event);

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

	GuiImage* options_parent;

	//bool			options_created;
	GuiButton*		fullscreen_off;
	GuiButton*		fullscreen_on;
	GuiButton*		back_button;
	GuiScrollbar*	music_scrollbar;
	GuiScrollbar*	sfx_scrollbar;

	//Text:
	GuiText*		resolution_text;
	GuiText*		sfx_text;
	GuiText*		music_text;
	GuiText*		options_text;

	//Audio
	uint			menu_song;
};

#endif // !__OPTIONS_SCENE_H__

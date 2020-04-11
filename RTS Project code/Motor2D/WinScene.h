#ifndef __WIN_SCENE_H__
#define __WIN_SCENE_H__

#include "Module.h"
#include "Scene.h"

class WinScene : public Scene
{
public:
	WinScene();
	~WinScene();										// Destructor

	bool Awake(pugi::xml_node&);						// Called before render is available
	bool Start();										// Called before the first frame
	bool PreUpdate();									// Called before all Updates
	bool Update(float dt);								// Called each loop iteration
	bool PostUpdate();									// Called before all Updates
	bool CleanUp();										// Called before quitting

public:

	void LoadGuiElements();

	void OnEventCall(UI* element, UI_EVENT ui_event);

	void ExecuteTransition();

public:
	// TMP TRANSITION BUTTON
	UI_Button* transition_button;
	UI_Text* button_text;
	// ---------------------

private:


};

#endif // !_WIN_SCENE_H__

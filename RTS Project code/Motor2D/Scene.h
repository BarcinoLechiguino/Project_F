#ifndef __SCENE_H__
#define __SCENE_H__

#include "SDL/include/SDL_rect.h"
#include "Module.h"												// Scene inherits from Module to allow the use of the "this" keyword in the OnEventCall of each Scene. 
#include "Point.h"

enum class SCENES
{
	LOGO_SCENE,
	MAIN_MENU_SCENE,
	OPTIONS_SCENE,
	GAMEPLAY_SCENE,
	WIN_SCENE,
	LOSE_SCENE,
	NONE
};

class Scene : public Module
{
public:
	Scene(SCENES scene_name);
	virtual ~Scene();

	virtual bool Awake(pugi::xml_node& config);					// Called before render is available
	virtual bool Start();										// Called before the first frame
	virtual bool PreUpdate();									// Called before all Updates
	virtual bool Update(float dt);								// Called each loop iteration
	virtual bool PostUpdate();									// Called before all Updates
	virtual bool CleanUp();										// Called before quitting

public:
	virtual void InitScene();									// Method that will be used to initialize the variables all the scenes.
	virtual void DrawScene();									// Method that will draw the map and all other elements of the scene.
	
	virtual void LoadGuiElements();

	virtual void ExecuteTransition();							// Method that will trigger a new transition depending on the input received.

	virtual void CameraDebugMovement(float dt);					// Method that will move the camera around depending on the input.
	virtual void DebugKeys();									// Method that will contain all the debug keys for a given scene.

public:
	std::string		name_tag;

	SCENES			scene_name;									// SCENE will be both the name and the index of the scene.

	SDL_Texture*	occupied_debug;
	SDL_Texture*	occupied_by_entity_debug;

	int				map_width;									// Width of the map loaded on the scene.
	int				map_height;									// Height of the map loaded on the scene.
	bool			escape;		
};

#endif // !__SCENE_H__
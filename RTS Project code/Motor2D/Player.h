#include "Module.h"
#include "Point.h"
#include "SDL/include/SDL_rect.h"
#include "EntityManager.h"
#include <vector>

class Player : public Module
{
public:

	Player();

	~Player();

	bool Awake(pugi::xml_node& conf);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

public:

	void MouseCalculations();

	void CameraController(float dt);

	void SelectionRect();

	void SelectionOnClick();

	void SelectionShortcuts();

	void MoveToOrder();

	void DrawCursor();

	bool CurrentlyInGameplayScene();

public:

	iPoint			scene_camera_limit;
	iPoint			mouse_position;
	iPoint			mouse_map_position;
	iPoint			mouse_tile;							// The tile where the mouse is currently on.
	fPoint			camera_speed;						// Speed at which the camera will move when moving it with the mouse.
	
	iPoint			selection_start;					// Position where unit selection will start (Selection rect's anchor point).

	SDL_Texture*	cursor_idle;
	SDL_Texture*	mouse_tile_debug;

	SDL_Rect		selection_rect;

	//selection
	bool			is_selecting;

	std::vector<Dynamic_Object*>	units_selected;

	Static_Object*					building_selected;
};

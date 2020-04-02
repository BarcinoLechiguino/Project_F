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

	void MouseCalculations();

	void CameraController(float dt);

	void SelectionRect();

	void MoveToOrder();

	void DrawCursor();

public:

	iPoint scene_camera_limit;
	iPoint mouse_position;
	iPoint mouse_map_position;
	iPoint mouse_tile;
	fPoint camera_speed;
	iPoint selection_start;

	SDL_Texture* cursor_idle;
	SDL_Texture* mouse_tile_debug;

	//selection
	bool selecting;

	SDL_Rect selection_rect;

	std::vector<Dynamic_Object*> units_selected;


};

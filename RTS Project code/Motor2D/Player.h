#include "Module.h"
#include "Point.h"
#include "SDL/include/SDL_rect.h"
#include <vector>
#include "EntityManager.h"

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

	void CameraController(float dt);

	void SelectionRect();

	void Cursor();

public:

	iPoint scene_camera_limit;
	iPoint mouse_position;

	fPoint camera_speed;

	SDL_Texture* cursor_idle;

	//selection
	bool selecting;

	iPoint selection_start;

	SDL_Rect selection_rect;

	std::vector<Gatherer*> units_selected;

};

#include "Module.h"
#include "Point.h"

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

private:

	void CameraController(float dt);

	void Cursor();
public:

	iPoint scene_camera_limit;

	fPoint camera_speed;

	iPoint mouse_position;

	SDL_Texture* cursor_idle;

};

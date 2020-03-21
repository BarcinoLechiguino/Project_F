#include "Module.h"
#include "Point.h"

class Player : public Module
{
public:

	Player();

	~Player();

	bool Awake(pugi::xml_node& conf);

	bool Init();

	bool Start();

	bool PreUpdate();

	bool Update();

	bool PostUpdate();

	bool CleanUp();

private:

	void CameraController();

public:

	iPoint scene_camera_limit_x;

	iPoint camera_speed;

};

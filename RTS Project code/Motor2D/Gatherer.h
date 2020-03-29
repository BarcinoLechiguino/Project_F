#ifndef __GATHERER_H__
#define __GATHERER_H__

class Gatherer : public Dynamic_Object
{
public:

	Gatherer(int x, int y, ENTITY_TYPE type);

	~Gatherer();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt, bool doLogic);

	bool PostUpdate();

	bool CleanUp();

	void OnCollision(Collider* C1, Collider* C2);

	void MoveInput();

	void Move(float dt);

public:

	const std::vector<iPoint>* entity_path;

	SDL_Rect selection_collider;

};

#endif // __ENEMY_H__
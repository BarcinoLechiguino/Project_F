#ifndef __INFANTRY_H__
#define __INFANTRY_H__

class Infantry : public Dynamic_Object
{
public:

	Infantry(int x, int y, ENTITY_TYPE type);

	~Infantry();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt, bool doLogic);

	bool PostUpdate();

	bool CleanUp();

	void OnCollision(Collider* C1, Collider* C2);

public:

	Entity_State	state;

	const std::vector<iPoint>* entity_path;

};

#endif // __ENEMY_H__
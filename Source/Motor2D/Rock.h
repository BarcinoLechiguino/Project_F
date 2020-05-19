#ifndef __ROCK_H__
#define __ROCK_H__

#include "Resource.h"

class Emitter;

class Rock : public Resource
{
public:
	Rock(int x, int y, ENTITY_TYPE type, int level);
	~Rock();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt, bool do_logic);
	bool PostUpdate();
	bool CleanUp();

	void Draw();

public:
	void InitEntity();

	void AttachHealthbarToEntity();

private:
	
	Emitter* ShineEmitter;
};

#endif // !__ROCK_H__
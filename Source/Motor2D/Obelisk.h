#ifndef __OBELISK_H__
#define __OBELISK_H__

#include "Resource.h"

class Obelisk : public Resource
{
public:
	Obelisk(int x, int y, ENTITY_TYPE type, int level);
	~Obelisk();

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

};

#endif // !__OBELISK_H__
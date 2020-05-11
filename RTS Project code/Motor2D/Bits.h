#ifndef __BITS_H__
#define __BITS_H__

#include "Resource.h"

class Bits : public Resource
{
public:
	Bits(int x, int y, ENTITY_TYPE type, int level);
	~Bits();

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

#endif // !__BITS_H__
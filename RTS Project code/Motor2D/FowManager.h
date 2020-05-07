#ifndef __FOW_MANAGER_H__
#define __FOW_MANAGER_H__

#include "Module.h"

enum class FOW_TILE_STATE
{

};

struct FOW_Entity
{
	FOW_Entity();
	FOW_Entity(iPoint position, bool provides_visibility);

	void SetPos(iPoint new_position);

	iPoint position;

	bool is_visible;
	bool provides_visibility;

	std::vector<iPoint> frontier;
	std::vector<iPoint> line_of_sight;
};

class FowManager : public Module
{
public:
	FowManager();
	~FowManager();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

public:
	void SetVisibilityMap(uint width, uint height);
	void ChangeVisibilityMap(iPoint tile_position, uchar fow_tile_state);
	//void ChangeVisibilityMap(iPoint tile_position, FOW_TILE_STATE fow_tile_state);
	void ResetVisibilityMap();

private:
	uint width;
	uint height;

	uchar* visibility_map;
};

#endif // !__FOW_MANAGER_H__
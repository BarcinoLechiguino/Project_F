#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "SDL/include/SDL_rect.h"
#include "Module.h"
#include "Point.h"

class Pathfinding;
class Dynamic_Object;
class Static_Object;

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

public:
	// --- Camera and mouse methods.
	void MouseCalculations();
	void CameraController(float dt);
	void MoveToOrder();
	void DrawCursor();

	// --- Entity Selection Methods.
	void DragSelection();
	void UpdateSelectionRect();
	void SelectEntitiesInSelectionRect();

	void SelectionShortcuts();							// Unit selection shortcuts. Currently Z (All), X (Gatherers), C (Infantries), V (Enemies).
	void SelectionOnClick();							// Method that will select whichever entity is at the mouse's position when it is right-clicked.

	void SelectAllEntities();
	void SelectGatherers();
	void SelectInfatries();
	void SelectEnemies();

	void SelectEntityAt(const iPoint& tile_position);

	// --- Entity Deletion Methods.
	void DeleteOnInput();
	void DeleteEntityAt(const iPoint& tile_position);

	void DeleteEntityFromBuffers(Entity* entity_to_delete);
	void ClearEntityBuffers();

	// --- Entity Spawn Methods.
	void DebugUnitSpawn();

	// --- Conditional Checks
	bool CurrentlyInGameplayScene();
	bool CheckSelectionRectBorders(Dynamic_Object* unit);

public:

	iPoint			scene_camera_limit;
	iPoint			mouse_position;
	iPoint			mouse_map_position;
	iPoint			mouse_tile;							// The tile where the mouse is currently on.
	fPoint			camera_speed;						// Speed at which the camera will move when moving it with the mouse.
	
	iPoint			selection_start;					// Position where unit selection will start (Selection rect's anchor point).

	SDL_Texture*	cursor_idle;
	SDL_Texture*	cursor_hover_ally;
	SDL_Texture*	cursor_hover_enemy;
	SDL_Texture*	cursor_hover_UI;
	SDL_Texture*	cursor_clicked_ally;
	SDL_Texture*	cursor_clicked_enemy;
	SDL_Texture*	cursor_clicked_UI;
	
	SDL_Texture*	mouse_tile_debug;

	SDL_Rect		selection_rect;

	//selection
	bool			is_selecting;

	std::vector<Dynamic_Object*>	units_selected;

	Static_Object*					building_selected;
};

#endif // !__PLAYER_H__
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "SDL/include/SDL_rect.h"
#include "Module.h"
#include "Point.h"

class Pathfinding;
class Entity;
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
	
	void GiveOrder();
										// Will give all units in the units_selected vector a target entity to attack.

	void DrawCursor();

	// --- Entity Selection Methods.
	void DragSelection();										// Method that generates a selection rect that will select all units that are inside its bounds.
	void UpdateSelectionRect();									// Method that will update the selection rect's size by taking into account the current mouse position.
	void SelectEntitiesInSelectionRect();

	void SelectionShortcuts();									// Unit selection shortcuts. Currently Z (All), X (Gatherers), C (Infantries), V (Enemies).
	void SelectOnClick();										// Method that will select whichever entity is at the mouse's position when it is right-clicked.

	void SelectAllUnits();
	void SelectAllyUnits();
	void SelectEnemyUnits();

	void SelectGatherers();
	void SelectScouts();
	void SelectInfantries();
	void SelectHeavys();
	void SelectEnemyGatherers();
	void SelectEnemyScouts();
	void SelectEnemies();
	void SelectEnemyHeavys();

	void SelectEntityAt(const iPoint& tile_position);

	// --- Entity Deletion Methods.
	void DeleteOnInput();										// Method that will delete the entities depending on the input. Currently D (Delete entity at position) and F (Delete All).
	void DeleteEntityAt(const iPoint& tile_position);

	void DeleteEntityFromBuffers(Entity* entity_to_delete);		// Method that will delete a given entity from units_selected, set to null building_selected set to null resource_selected.
	void ClearEntityBuffers();									// Method that will clear all 3 entity buffers (units_selected, building_selected and resource_selected).

	// --- Entity Spawn Methods.
	void DebugUnitSpawn();

	void DebugUnitUpgrade();

	// --- Conditional Checks
	bool CurrentlyInGameplayScene();
	bool CheckSelectionRectBorders(Dynamic_Object* unit);


public:
	bool			god_mode;
	bool			is_selecting;

	iPoint			scene_camera_x_limit;
	iPoint			scene_camera_y_limit;
	iPoint			mouse_position;
	iPoint			mouse_map_position;
	iPoint			mouse_tile;									// The tile where the mouse is currently on.
	fPoint			camera_speed;								// Speed at which the camera will move when moving it with the mouse.
	
	iPoint			selection_start;							// Position where unit selection will start (Selection rect's anchor point).

	SDL_Texture*	cursor_idle;
	SDL_Texture*	cursor_hover_ally;
	SDL_Texture*	cursor_hover_enemy;
	SDL_Texture*	cursor_hover_UI;
	SDL_Texture*	cursor_clicked_ally;
	SDL_Texture*	cursor_clicked_enemy;
	SDL_Texture*	cursor_clicked_UI;
	
	SDL_Texture*	mouse_tile_debug;

	SDL_Rect		selection_rect;


	std::vector<Dynamic_Object*>	units_selected;

	Static_Object*					building_selected;
	Static_Object*					resource_selected;
};

#endif // !__PLAYER_H__
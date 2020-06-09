#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Dependencies/SDL/include/SDL_rect.h"
#include "Module.h"
#include "Point.h"
#include "GuiCursor.h"

class Pathfinding;

class Entity;
class DynamicObject;
class StaticObject;

class GuiCursor;

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
	void InitializePlayer();
	
	// --- Camera and Cursor methods.
	void CursorCalculations();

	void CameraController(float dt);
	
	void MoveCameraWithGameController();
	void MoveCursorWithGameController();

	void GiveOrder();

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
	void SelectEnemyInfantries();
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
	bool CheckSelectionRectBorders(DynamicObject* unit);

public:
	bool				god_mode;									// Will keep track of whether the god_mode has been activated or not.
	bool				has_saved;									// Will keep track of whether the player has saved since application start or not.
	bool				is_selecting;								// Will keep track of whether the player is currently selecting units or not.

	iPoint				original_camera_position;
	iPoint				scene_camera_x_limit;						// x = left limit, y = right limit.
	iPoint				scene_camera_y_limit;						// x = top limit, y = bottom limit.

	iPoint				cursor_position;
	iPoint				cursor_map_position;						// The tile where the cursor is currently on.
	iPoint				cursor_tile;

	//iPoint				mouse_position;
	//iPoint				mouse_map_position;
	//iPoint				mouse_tile;									// The tile where the mouse is currently on.
	
	fPoint				camera_speed;								// Speed at which the camera will move when moving it with the mouse.
	fPoint				controller_cursor_speed;					// Speed at which the cursor will move under the influence of the game controller.
	
	iPoint				selection_start;							// Position where unit selection will start (Selection rect's anchor point).
	SDL_Rect			selection_rect;

	StaticObject*		building_selected;
	StaticObject*		resource_selected;

	std::vector<DynamicObject*>	units_selected;
	
public:																// ---------- CURSOR VARIABLES ----------
	SDL_Texture*		mouse_tile_debug;							// Texture that will be drawn on top of the tile where the cursor is currently at.
	
	GuiCursor			cursor;										// For whatever reason only a pointer works with the forward declaration.
	
	SDL_Rect			idle;										// IDLE Sprite of the cursor.

	SDL_Rect			cursor_hover_ally;							// HOVER Sprite for when the cursor hovers an ally entity.
	SDL_Rect			cursor_hover_enemy;							// HOVER Sprite for when the cursor hovers an enemy entity.
	SDL_Rect			cursor_hover_resource;						// HOVER Sprite for when the cursor hovers a resource entity.
	SDL_Rect			cursor_hover_UI;							// HOVER Sprite for when the cursor hovers an interactible UI_Element.

	SDL_Rect			cursor_clicked_idle;						// CLICKED Sprite for when the cursor clicks and there is nothing under it.
	SDL_Rect			cursor_clicked_ally;						// CLICKED Sprite for when the cursor hovers an ally entity.
	SDL_Rect			cursor_clicked_enemy;						// CLICKED Sprite for when the cursor hovers an enemy entity.
	SDL_Rect			cursor_clicked_resource;					// CLICKED Sprite for when the cursor hovers a resource entity.
	SDL_Rect			cursor_clicked_UI;							// CLICKED Sprite for when the cursor hovers an interactible UI_Element.
};

#endif // !__PLAYER_H__
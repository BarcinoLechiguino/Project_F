#include "Log.h"
#include "EasingFunctions.h"

#include "Input.h"
#include "Render.h"
#include "Application.h"
#include "Window.h"
#include "Audio.h"
#include "Textures.h"
#include "Input.h"
#include "EntityManager.h"
#include "Pathfinding.h"
#include "Map.h"
#include "Minimap.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameplayScene.h"
#include "Movement.h"
#include "DialogManager.h"

#include "GuiManager.h"
#include "GuiElement.h"
//#include "GuiCursor.h"

#include "EntityManager.h"
#include "DynamicObject.h"
#include "Gatherer.h"
#include "Infantry.h"
#include "StaticObject.h"
#include "TownHall.h"
#include "Barracks.h"
#include "EnemyTownHall.h"
#include "EnemyBarracks.h"

#include "Player.h"

Player::Player() : building_selected(nullptr), resource_selected(nullptr), mouse_tile_debug(nullptr)
{

}

Player::~Player()
{

}

bool Player::Awake(pugi::xml_node& conf)
{
	return true;
}

bool Player::Start()
{
	InitializePlayer();

	return true;
}

bool Player::PreUpdate()
{
	if (CurrentlyInGameplayScene() && !App->pause)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
	else
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	return true;
}

bool Player::Update(float dt)
{
	CursorCalculations();
	
	CameraController(dt);
	
	if (!App->pause && !App->gui_manager->VisibleElementIsUnderCursor())			//TMP. Dirty Fix(?)
	{
		DragSelection();

		SelectOnClick();

		DeleteOnInput();

		GiveOrder();
	}

	SelectionShortcuts();

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_STATE::KEY_DOWN)
	{
		DebugUnitSpawn();
	}

	if (App->input->GetKey(SDL_SCANCODE_U) == KEY_STATE::KEY_DOWN)
	{
		DebugUnitUpgrade();
	}

	return true;
}

bool Player::PostUpdate()
{
	MoveCursorWithGameController();
	
	DrawCursor();
	
	return true;
}

bool Player::CleanUp()
{
	App->tex->UnLoad(mouse_tile_debug);
	
	ClearEntityBuffers();
	
	return true;
}

// ------------------- CAMERA AND MOUSE METHODS -------------------
void Player::CursorCalculations()
{
	//Get Cursor Position
	cursor_position = cursor.GetScreenPos();

	iPoint cursor_camera;
	cursor_camera.x = cursor_position.x - App->render->camera.x;
	cursor_camera.y = cursor_position.y - App->render->camera.y;

	// Get Mouse's Map Position (In Tiles)
	iPoint to_map_cursor_pos = App->map->WorldToMap(cursor_camera.x, cursor_camera.y + 20); //magic
	cursor_tile = iPoint(to_map_cursor_pos.x, to_map_cursor_pos.y);

	// Get Mouse's World Poisition (In Pixels)
	iPoint to_world_cursor_pos = App->map->MapToWorld(cursor_tile.x, cursor_tile.y);
	cursor_map_position = iPoint(to_world_cursor_pos.x, to_world_cursor_pos.y);
}

void Player::CameraController(float dt)
{
	int window_width, window_height;
	App->win->GetWindowSize(window_width, window_height);

	if (CurrentlyInGameplayScene() && !App->scene_manager->gameplay_scene->tutorial.lock_camera)																							// If the current scene is FIRST_SCENE (gameplay scene)
	{
		if (!cursor.game_controller_mode)
		{
			if (cursor_position.x <= 10 || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_STATE::KEY_REPEAT)								//Left
			{
				if (App->render->camera.x < scene_camera_x_limit.y)
				{
					App->render->camera.x += (int)(camera_speed.x * dt);
				}
			}

			if (cursor_position.x >= (window_width - 10) || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_STATE::KEY_REPEAT)			//Right
			{
				if (App->render->camera.x > scene_camera_x_limit.x)
				{
					App->render->camera.x -= (int)(camera_speed.x * dt);
				}
			}

			if (cursor_position.y >= (window_height - 10) || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_STATE::KEY_REPEAT)			//Bottom
			{
				if (App->render->camera.y > scene_camera_y_limit.x)
				{
					App->render->camera.y -= (int)(camera_speed.y * dt);
				}
			}

			if (cursor_position.y <= 10 || App->input->GetKey(SDL_SCANCODE_UP) == KEY_STATE::KEY_REPEAT)								//Up
			{
				if (App->render->camera.y < scene_camera_y_limit.y)
				{
					App->render->camera.y += (int)(camera_speed.y * dt);
				}
			}
		}
		else
		{
			MoveCameraWithGameController();
		}
	}
}

void Player::MoveCameraWithGameController()
{
	if (!App->scene_manager->gameplay_scene->tutorial.lock_camera)
	{
		if (App->input->GetGameControllerAxis(SDL_CONTROLLER_AXIS_RIGHTX) == AXIS_STATE::NEGATIVE_AXIS_REPEAT)					//Left
		{
			if (App->render->camera.x < scene_camera_x_limit.y)
			{
				App->render->camera.x += (int)(camera_speed.x * App->GetDt());
			}
		}

		if (App->input->GetGameControllerAxis(SDL_CONTROLLER_AXIS_RIGHTX) == AXIS_STATE::POSITIVE_AXIS_REPEAT)					//Right
		{
			if (App->render->camera.x > scene_camera_x_limit.x)
			{
				App->render->camera.x -= (int)(camera_speed.x * App->GetDt());
			}
		}

		if (App->input->GetGameControllerAxis(SDL_CONTROLLER_AXIS_RIGHTY) == AXIS_STATE::POSITIVE_AXIS_REPEAT)					//Bottom
		{
			if (App->render->camera.y > scene_camera_y_limit.x)
			{
				App->render->camera.y -= (int)(camera_speed.y * App->GetDt());
			}
		}

		if (App->input->GetGameControllerAxis(SDL_CONTROLLER_AXIS_RIGHTY) == AXIS_STATE::NEGATIVE_AXIS_REPEAT)					//Up
		{
			if (App->render->camera.y < scene_camera_y_limit.y)
			{
				App->render->camera.y += (int)(camera_speed.y * App->GetDt());
			}
		}
	}
}

void Player::MoveCursorWithGameController()
{
	SDL_Rect window;
	App->win->GetWindowRect(window);

	if (App->input->GetGameControllerAxis(SDL_CONTROLLER_AXIS_LEFTX) == AXIS_STATE::NEGATIVE_AXIS_REPEAT)			//Left
	{
		iPoint cursor_position = cursor.GetScreenPos();
		
		if (cursor_position.x > window.x)
		{
			cursor_position.x -= (int)(controller_cursor_speed.x * App->GetUnpausableDt());
		}
		else
		{
			cursor_position.x = window.x;
		}

		cursor.SetScreenPos(cursor_position);
	}

	if (App->input->GetGameControllerAxis(SDL_CONTROLLER_AXIS_LEFTX) == AXIS_STATE::POSITIVE_AXIS_REPEAT)			//Right
	{
		iPoint cursor_position = cursor.GetScreenPos();

		if (cursor_position.x < window.x + window.w - 2)															//Magic
		{
			cursor_position.x += (int)(controller_cursor_speed.x * App->GetUnpausableDt());
		}
		else
		{
			cursor_position.x = window.x + window.w - 2;															//Magic
		}

		cursor.SetScreenPos(cursor_position);
	}

	if (App->input->GetGameControllerAxis(SDL_CONTROLLER_AXIS_LEFTY) == AXIS_STATE::POSITIVE_AXIS_REPEAT)			//Bottom
	{
		iPoint cursor_position = cursor.GetScreenPos();

		if (cursor_position.y < window.y + window.h - 2)															//Magic
		{
			cursor_position.y += (int)(controller_cursor_speed.y * App->GetUnpausableDt());
		}
		else
		{
			cursor_position.y = window.y + window.h - 2;															//Magic
		}

		cursor.SetScreenPos(cursor_position);
	}

	if (App->input->GetGameControllerAxis(SDL_CONTROLLER_AXIS_LEFTY) == AXIS_STATE::NEGATIVE_AXIS_REPEAT)			//Up
	{
		iPoint cursor_position = cursor.GetScreenPos();

		if (cursor_position.y > window.y)
		{
			cursor_position.y -= (int)(controller_cursor_speed.y * App->GetUnpausableDt());
		}
		else
		{
			cursor_position.y = window.y;
		}

		cursor.SetScreenPos(cursor_position);
	}
}

void Player::GiveOrder()//fix
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_STATE::KEY_DOWN 
		|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == BUTTON_STATE::BUTTON_DOWN)
	{
		if (units_selected.size() != 0)																						// If there are Units being selected
		{	
			if (App->entity_manager->GetEntityAt(cursor_tile) == nullptr)
			{
				App->movement->OrderUnitsToMove(cursor_tile, units_selected);

				if (App->scene_manager->gameplay_scene->tutorial.tutorial_state == TutorialState::MOVE_UNIT )
				{
					App->scene_manager->gameplay_scene->tutorial.tutorial_state = TutorialState::GATHER_RESOURCE;
					App->dialog_manager->StartDialog(2);
				}
			}
			else
			{
				App->movement->OrderUnitsToAttack(cursor_tile, units_selected);
			}
		}
		else
		{
			LOG("There are no units being currently selected.");
		}
	}
}

void Player::DrawCursor()
{
	if (CurrentlyInGameplayScene())
	{
		if (App->map->CheckMapBoundaries(cursor_tile))
		{
			if (!App->gui_manager->VisibleElementIsUnderCursor() && !App->minimap->InteractingWithMinimap())
			{
				App->render->Blit(mouse_tile_debug, cursor_map_position.x, cursor_map_position.y, nullptr, false, 1.f);
			}
		}
	}
	
	cursor.Draw();
}

// ------------------- ENTITY SELECTION METHODS -------------------
void Player::DragSelection()
{
	if (CurrentlyInGameplayScene())
	{
		if (!App->minimap->player_is_moving_camera)
		{
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_DOWN
				|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_A) == BUTTON_STATE::BUTTON_DOWN
				|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == BUTTON_STATE::BUTTON_DOWN)
			{
				if (!App->gui_manager->VisibleElementIsUnderCursor())
				{
					is_selecting = true;

					//selection_start = mouse_position;
					selection_start = cursor_position;

					ClearEntityBuffers();
				}
			}

			if (is_selecting)
			{
				if (selection_start != cursor_position)
				{
					if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_REPEAT)
					{
						UpdateSelectionRect();
					}

					if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_UP
						|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_A) == BUTTON_STATE::BUTTON_UP
						|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == BUTTON_STATE::BUTTON_UP
						/*|| App->gui_manager->VisibleElementIsUnderMouse()*/)															//TMP FIX. Talk about what to do with this.
					{
						is_selecting = false;

						SelectEntitiesInSelectionRect();

						if (App->scene_manager->gameplay_scene->tutorial.tutorial_state == TutorialState::SELECT_UNIT && !units_selected.empty() && App->dialog_manager->dialog_state == DIALOG_STATE::NOT_ACTIVE)
						{
							App->scene_manager->gameplay_scene->tutorial.tutorial_state = TutorialState::MOVE_UNIT;
							App->dialog_manager->StartDialog(1);
						}
					}
				}
				else
				{
					if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_UP
						|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_A) == BUTTON_STATE::BUTTON_UP
						|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == BUTTON_STATE::BUTTON_UP)
					{
						SelectEntityAt(cursor_tile);

						if (App->scene_manager->gameplay_scene->tutorial.tutorial_state == TutorialState::SELECT_UNIT && !units_selected.empty() && App->dialog_manager->dialog_state == DIALOG_STATE::NOT_ACTIVE)
						{
							App->scene_manager->gameplay_scene->tutorial.tutorial_state = TutorialState::MOVE_UNIT;
							App->dialog_manager->StartDialog(1);
						}
					}
				}
			}
		}
	}
}

void Player::UpdateSelectionRect()
{
	//Cases with cursor pos
	if (cursor_position.x > selection_start.x)
	{
		selection_rect = { selection_start.x , selection_start.y, cursor_position.x - selection_start.x, cursor_position.y - selection_start.y };
	}
	else
	{
		selection_rect = { cursor_position.x ,selection_start.y, selection_start.x - cursor_position.x , cursor_position.y - selection_start.y };
	}

	if (cursor_position.y < selection_start.y)
	{
		selection_rect.y = cursor_position.y;
		selection_rect.h = selection_start.y - cursor_position.y;
	}

	//App->render->DrawQuad(selection_rect, 150, 150, 255, 100, false, false);

	App->render->DrawLine(selection_rect.x, selection_rect.y, selection_rect.x + selection_rect.w, selection_rect.y, 150, 150, 255, 255,false); //Top line
	App->render->DrawLine(selection_rect.x + selection_rect.w , selection_rect.y, selection_rect.x + selection_rect.w, selection_rect.y + selection_rect.h, 150, 150, 255, 255, false); //Right line
	App->render->DrawLine(selection_rect.x , selection_rect.y + selection_rect.h, selection_rect.x + selection_rect.w, selection_rect.y + selection_rect.h, 150, 150, 255, 255, false); //Bottom line
	App->render->DrawLine(selection_rect.x, selection_rect.y, selection_rect.x, selection_rect.y + selection_rect.h, 150, 150, 255, 255, false); //Left line
}

void Player::SelectEntitiesInSelectionRect()
{
	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if (App->entity_manager->IsUnit((*item)))
		{
			DynamicObject* unit = (DynamicObject*)(*item);

			if (unit->is_selectable)
			{
				if (CheckSelectionRectBorders(unit))				//Collision ally units rectangles
				{
					unit->is_selected = true;
					units_selected.push_back(unit);
				}
			}
		}
	}

	LOG("Units selected %d", units_selected.size());
}

void Player::SelectOnClick()
{
	if (CurrentlyInGameplayScene())
	{
		if (!god_mode)
		{
			//what?
		}
	}
}

void Player::SelectionShortcuts()
{
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_STATE::KEY_DOWN 
		|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_Y) == BUTTON_STATE::BUTTON_DOWN)				// Select All Entities
	{
		SelectAllyUnits();
	}

	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_STATE::KEY_DOWN 
		|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_DPAD_UP) == BUTTON_STATE::BUTTON_DOWN)			// Select All Gatherers
	{
		SelectGatherers();
	}

	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_STATE::KEY_DOWN 
		|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == BUTTON_STATE::BUTTON_DOWN)		// Select All Scouts
	{
		SelectScouts();
	}

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_STATE::KEY_DOWN 
		|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == BUTTON_STATE::BUTTON_DOWN)		// Select All Infantries
	{
		SelectInfantries();
	}

	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_STATE::KEY_DOWN 
		|| App->input->GetGameControllerButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT) == BUTTON_STATE::BUTTON_DOWN)		// Select All Heavys
	{
		SelectHeavys();
	}
	
	
	// DEBUG SELECTION SHORTCUTS
	if (App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_STATE::KEY_DOWN)													// Select All Units
	{																												
		SelectAllUnits();
	}


	// Select Enemy Units																							
	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_STATE::KEY_DOWN)													// Select All Enemy Units
	{																												
		SelectEnemyUnits();
	}																												
																													
	if (App->input->GetKey(SDL_SCANCODE_COMMA) == KEY_STATE::KEY_DOWN)												// Select All Enemy Gatherers
	{																												
		SelectEnemyGatherers();
	}																												
																													
	if (App->input->GetKey(SDL_SCANCODE_PERIOD) == KEY_STATE::KEY_DOWN)												// Select All Enemy Scouts
	{																												
		SelectEnemyScouts();
	}																												
																													
	if (App->input->GetKey(SDL_SCANCODE_SLASH) == KEY_STATE::KEY_DOWN)												// Select All Enemy Infantries
	{																												
		SelectEnemyInfantries();
	}																												
																													
	if (App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_STATE::KEY_DOWN)												// Select All Enemy Heavys
	{
		SelectEnemyHeavys();
	}
}

// -------------- UNIT SELECTION METHODS --------------
void Player::SelectAllUnits()
{
	ClearEntityBuffers();

	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if (App->entity_manager->IsUnit((*item)))
		{
			DynamicObject* unit = (DynamicObject*)(*item);

			(*item)->is_selected = true;
			units_selected.push_back(unit);
		}
	}

	LOG("All units selected. Total unit amount: %d", units_selected.size());
}

void Player::SelectAllyUnits()
{
	ClearEntityBuffers();

	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if (App->entity_manager->IsAllyUnit((*item)))
		{
			DynamicObject* unit = (DynamicObject*)(*item);

			if (unit->is_selectable)
			{
				(*item)->is_selected = true;
				units_selected.push_back(unit);
			}
		}
	}

	LOG("All ally units selected. Total unit amount: %d", units_selected.size());
}

void Player::SelectEnemyUnits()
{
	ClearEntityBuffers();

	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if (App->entity_manager->IsEnemyUnit((*item)))
		{
			DynamicObject* unit = (DynamicObject*)(*item);

			if (!unit->is_selectable)
			{
				(*item)->is_selected = true;
				units_selected.push_back(unit);
			}
		}
	}

	LOG("All enemy units selected. Total unit amount: %d", units_selected.size());
}

void Player::SelectGatherers()
{
	ClearEntityBuffers();

	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if ((*item)->type == ENTITY_TYPE::GATHERER)
		{
			(*item)->is_selected = true;
			units_selected.push_back((DynamicObject*)(*item));
		}
	}

	LOG("All gatherers selected. Total gatherer amount: %d", units_selected.size());
}

void Player::SelectScouts()
{
	ClearEntityBuffers();

	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if ((*item)->type == ENTITY_TYPE::SCOUT)
		{
			(*item)->is_selected = true;
			units_selected.push_back((DynamicObject*)(*item));
		}
	}

	LOG("All scouts selected. Total gatherer amount: %d", units_selected.size());
}

void Player::SelectInfantries()
{
	ClearEntityBuffers();

	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if ((*item)->type == ENTITY_TYPE::INFANTRY)
		{
			(*item)->is_selected = true;
			units_selected.push_back((DynamicObject*)(*item));
		}
	}

	LOG("All infantries selected. Total infantry amount: %d", units_selected.size());
}

void Player::SelectHeavys()
{
	ClearEntityBuffers();

	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if ((*item)->type == ENTITY_TYPE::HEAVY)
		{
			(*item)->is_selected = true;
			units_selected.push_back((DynamicObject*)(*item));
		}
	}

	LOG("All Heavys selected. Total infantry amount: %d", units_selected.size());
}

void Player::SelectEnemyGatherers()
{
	if (god_mode)
	{
		ClearEntityBuffers();

		std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

		for (; item != App->entity_manager->entities.end(); ++item)
		{
			if ((*item)->type == ENTITY_TYPE::ENEMY_GATHERER)
			{
				(*item)->is_selected = true;
				units_selected.push_back((DynamicObject*)(*item));
			}
		}

		LOG("All enemy gatherers selected. Total enemy amount: %d", units_selected.size());
	}
}

void Player::SelectEnemyScouts()
{
	if (god_mode)
	{
		ClearEntityBuffers();

		std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

		for (; item != App->entity_manager->entities.end(); ++item)
		{
			if ((*item)->type == ENTITY_TYPE::ENEMY_SCOUT)
			{
				(*item)->is_selected = true;
				units_selected.push_back((DynamicObject*)(*item));
			}
		}

		LOG("All enemy scouts selected. Total enemy amount: %d", units_selected.size());
	}
}

void Player::SelectEnemyInfantries()
{
	if (god_mode)
	{
		ClearEntityBuffers();

		std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

		for (; item != App->entity_manager->entities.end(); ++item)
		{
			if ((*item)->type == ENTITY_TYPE::ENEMY_INFANTRY)
			{
				(*item)->is_selected = true;
				units_selected.push_back((DynamicObject*)(*item));
			}
		}

		LOG("All enemies selected. Total enemy amount: %d", units_selected.size());
	}
}

void Player::SelectEnemyHeavys()
{
	if (god_mode)
	{
		ClearEntityBuffers();

		std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

		for (; item != App->entity_manager->entities.end(); ++item)
		{
			if ((*item)->type == ENTITY_TYPE::ENEMY_HEAVY)
			{
				(*item)->is_selected = true;
				units_selected.push_back((DynamicObject*)(*item));
			}
		}

		LOG("All enemy heavys selected. Total enemy amount: %d", units_selected.size());
	}
}

void Player::SelectEntityAt(const iPoint& tile_position)
{
	if (App->entity_manager->entity_map != nullptr)
	{
		Entity* clicked_entity = App->entity_manager->GetEntityAt(tile_position);						//clicked_entity will be assigned the entity in the entity_map at the given position.

		if (clicked_entity != nullptr)
		{
			if (App->pathfinding->IsOccupied(tile_position))
			{
				if (App->entity_manager->IsUnit(clicked_entity))
				{
					ClearEntityBuffers();

					DynamicObject* unit = (DynamicObject*)clicked_entity;

					if (unit->is_selectable)
					{
						unit->is_selected = true;
						units_selected.push_back((DynamicObject*)clicked_entity);
					}

					return;
				}
			}

			if (App->pathfinding->IsNonWalkable(tile_position))
			{
				if (App->entity_manager->IsBuilding(clicked_entity))
				{
					ClearEntityBuffers();

					clicked_entity->is_selected = true;
					building_selected = (StaticObject*)clicked_entity;

					//LOG("A BUILDING WAS SELECTED AT TILE (%d, %d)", tile_position.x, tile_position.y);

					return;
				}

				if (App->entity_manager->IsResource(clicked_entity))
				{
					ClearEntityBuffers();

					clicked_entity->is_selected = true;
					resource_selected = (StaticObject*)clicked_entity;
				}
			}
		}
		else
		{
			ClearEntityBuffers();

			LOG("There is no Entity at tile (%d, %d)", tile_position.x, tile_position.y);
		}
	}
}

// ------------------- ENTITY DELETION METHODS -------------------
void Player::DeleteOnInput()
{
	if (CurrentlyInGameplayScene())
	{
		if (god_mode)
		{
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_STATE::KEY_DOWN)											// Delete entity at mouse position.
			{
				//DeleteEntityAt(mouse_tile);
				DeleteEntityAt(cursor_tile);
			}

			if (App->input->GetKey(SDL_SCANCODE_F) == KEY_STATE::KEY_DOWN)											// Delete all entities.
			{
				ClearEntityBuffers();
				App->entity_manager->DestroyEntities();
			}
		}
	}
}

void Player::DeleteEntityAt(const iPoint& tile_position)
{
	if (App->entity_manager->entity_map != nullptr)
	{
		//Entity* clicked_entity = App->entity_manager->GetEntityAt(mouse_tile);
		Entity* clicked_entity = App->entity_manager->GetEntityAt(cursor_tile);

		if (clicked_entity != nullptr)
		{
			DeleteEntityFromBuffers(clicked_entity);
			App->entity_manager->DeleteEntity(clicked_entity);
		}
	}
}

void Player::DeleteEntityFromBuffers(Entity* entity_to_delete)
{
	if (App->entity_manager->IsUnit(entity_to_delete))										// If the entity selected to be deleted is a unit.
	{
		std::vector<DynamicObject*>::iterator item = units_selected.begin();

		for (; item != units_selected.end(); ++item)
		{
			if ((*item) == entity_to_delete)												// If the entity selected to be deleted is in the units_selected vector.
			{
				units_selected.erase(item);
				break;
			}
		}
	}

	if (App->entity_manager->IsBuilding(entity_to_delete))
	{
		if (entity_to_delete == building_selected)
		{
			building_selected = nullptr;
		}
	}

	if (App->entity_manager->IsResource(entity_to_delete))
	{
		if (entity_to_delete == resource_selected)
		{
			resource_selected = nullptr;
		}
	}
}

void Player::ClearEntityBuffers()
{
	if (units_selected.size() != 0)
	{
		for (int i = 0; i < (int)units_selected.size(); ++i)
		{
			units_selected[i]->is_selected = false; //fix
		}

		units_selected.clear();
	}

	if (building_selected != nullptr)
	{
		building_selected->is_selected = false;
		building_selected = nullptr;
	}

	if (resource_selected != nullptr)
	{
		resource_selected->is_selected = false;
		resource_selected = nullptr;
	}
}

// ------------------- ENTITY SPAWN METHODS -------------------
void Player::DebugUnitSpawn()
{
	if (building_selected != nullptr)
	{
		TownHall* townhall				= nullptr;
		Barracks* barrack				= nullptr;
		EnemyTownHall* enemy_townhall	= nullptr;
		EnemyBarracks* enemy_barrack	= nullptr;

		switch (building_selected->type)
		{
		case ENTITY_TYPE::TOWNHALL:
			townhall = (TownHall*)building_selected;
			townhall->GenerateUnit(ENTITY_TYPE::GATHERER, townhall->level);
			break;

			case ENTITY_TYPE::BARRACKS:
			barrack = (Barracks*)building_selected;
			barrack->GenerateUnit(ENTITY_TYPE::INFANTRY, barrack->level);
			break;

		case ENTITY_TYPE::ENEMY_TOWNHALL:
			enemy_townhall = (EnemyTownHall*)building_selected;
			enemy_townhall->GenerateUnit(ENTITY_TYPE::ENEMY_GATHERER, enemy_townhall->level);
			break;

		case ENTITY_TYPE::ENEMY_BARRACKS:
			enemy_barrack = (EnemyBarracks*)building_selected;
			enemy_barrack->GenerateUnit(ENTITY_TYPE::ENEMY_INFANTRY, enemy_barrack->level);
			break;
		}
	}
}

// ------------------- ENTITY SPAWN METHODS -------------------

void Player::DebugUnitUpgrade()
{
	if (building_selected != nullptr)
	{
		TownHall* townhall				= nullptr;
		Barracks* barrack				= nullptr;
		EnemyTownHall* enemy_townhall	= nullptr;
		EnemyBarracks* enemy_barrack	= nullptr;

		switch (building_selected->type)
		{
		case ENTITY_TYPE::TOWNHALL:
			townhall = (TownHall*)building_selected;
			townhall->level++;
			townhall->LevelChanges();
			break;

		case ENTITY_TYPE::BARRACKS:
			barrack = (Barracks*)building_selected;
			barrack->level++;
			barrack->LevelChanges();
			break;

		case ENTITY_TYPE::ENEMY_TOWNHALL:
			enemy_townhall = (EnemyTownHall*)building_selected;
			enemy_townhall->level++;
			enemy_townhall->LevelChanges();
			break;

		case ENTITY_TYPE::ENEMY_BARRACKS:
			enemy_barrack = (EnemyBarracks*)building_selected;
			enemy_barrack->level++;
			enemy_barrack->LevelChanges();
			break;
		}
	}
}

// ------------------- CONDITIONAL CHECKS -------------------
bool Player::CurrentlyInGameplayScene()
{
	bool ret = false;

	if (App->scene_manager->current_scene->scene_name == SCENES::GAMEPLAY_SCENE)
	{
		ret = true;
	}

	return ret;
}

bool Player::CheckSelectionRectBorders(DynamicObject* unit)
{
	return (unit->selection_collider.x + unit->selection_collider.w > selection_rect.x - App->render->camera.x) &&			// Unit is inside the left border.
			(unit->selection_collider.x < selection_rect.x - App->render->camera.x + selection_rect.w) &&					// Unit is inside the right border.
			(unit->selection_collider.y + unit->selection_collider.h > selection_rect.y - App->render->camera.y) &&			// Unit is inside the top border.
			(unit->selection_collider.y < selection_rect.y - App->render->camera.y + selection_rect.h);						// Unit is inside the bottom border.
}

void Player::InitializePlayer()
{
	SDL_ShowCursor(SDL_DISABLE);

	selection_rect = { 0, 0, 0, 0 };

	// --- Loading from xml ---
	pugi::xml_node player = App->config_file.child("config").child("player");

	god_mode					= player.child("god_mode").attribute("value").as_bool();
	is_selecting				= player.child("is_selecting").attribute("value").as_bool();
	
	mouse_tile_debug			= App->tex->Load(player.child("mouse_tile_tex").attribute("path").as_string());

	original_camera_position.x	= player.child("camera").child("original_position").attribute("x").as_int();
	original_camera_position.y	= player.child("camera").child("original_position").attribute("y").as_int();
	
	scene_camera_x_limit.x		= player.child("camera").child("x_axis_limit").attribute("left").as_int();
	scene_camera_x_limit.y		= player.child("camera").child("x_axis_limit").attribute("right").as_int();
	scene_camera_y_limit.x		= player.child("camera").child("y_axis_limit").attribute("top").as_int();
	scene_camera_y_limit.y		= player.child("camera").child("y_axis_limit").attribute("bottom").as_int();

	camera_speed.x				= player.child("camera").child("speed").attribute("x").as_float();
	camera_speed.y				= player.child("camera").child("speed").attribute("y").as_float();

	controller_cursor_speed.x	= player.child("game_controller").child("cursor_speed").attribute("x").as_float();
	controller_cursor_speed.y	= player.child("game_controller").child("cursor_speed").attribute("y").as_float();

	// --- Cursor Section Sprites
	// -- Cursor IDLE
	idle.x = player.child("cursor_sections").child("idle").attribute("x").as_int();
	idle.y = player.child("cursor_sections").child("idle").attribute("y").as_int();
	idle.w = player.child("cursor_sections").child("idle").attribute("w").as_int();
	idle.h = player.child("cursor_sections").child("idle").attribute("h").as_int();
	
	// -- Cursor HOVER Ally
	cursor_hover_ally.x = player.child("cursor_sections").child("hover_ally").attribute("x").as_int();
	cursor_hover_ally.y = player.child("cursor_sections").child("hover_ally").attribute("y").as_int();
	cursor_hover_ally.w = player.child("cursor_sections").child("hover_ally").attribute("w").as_int();
	cursor_hover_ally.h = player.child("cursor_sections").child("hover_ally").attribute("h").as_int();

	// -- Cursor HOVER Enemy
	cursor_hover_enemy.x = player.child("cursor_sections").child("hover_enemy").attribute("x").as_int();
	cursor_hover_enemy.y = player.child("cursor_sections").child("hover_enemy").attribute("y").as_int();
	cursor_hover_enemy.w = player.child("cursor_sections").child("hover_enemy").attribute("w").as_int();
	cursor_hover_enemy.h = player.child("cursor_sections").child("hover_enemy").attribute("h").as_int();

	// -- Cursor HOVER Resource
	cursor_hover_resource.x = player.child("cursor_sections").child("hover_resource").attribute("x").as_int();
	cursor_hover_resource.y = player.child("cursor_sections").child("hover_resource").attribute("y").as_int();
	cursor_hover_resource.w = player.child("cursor_sections").child("hover_resource").attribute("w").as_int();
	cursor_hover_resource.h = player.child("cursor_sections").child("hover_resource").attribute("h").as_int();

	// -- Cursor HOVER UI
	cursor_hover_UI.x = player.child("cursor_sections").child("hover_UI").attribute("x").as_int();
	cursor_hover_UI.y = player.child("cursor_sections").child("hover_UI").attribute("y").as_int();
	cursor_hover_UI.w = player.child("cursor_sections").child("hover_UI").attribute("w").as_int();
	cursor_hover_UI.h = player.child("cursor_sections").child("hover_UI").attribute("h").as_int();

	// -- Cursor CLICKED IDLE
	cursor_clicked_idle.x = player.child("cursor_sections").child("clicked_idle").attribute("x").as_int();
	cursor_clicked_idle.y = player.child("cursor_sections").child("clicked_idle").attribute("y").as_int();
	cursor_clicked_idle.w = player.child("cursor_sections").child("clicked_idle").attribute("w").as_int();
	cursor_clicked_idle.h = player.child("cursor_sections").child("clicked_idle").attribute("h").as_int();
	
	// -- Cursor CLICKED Ally
	cursor_clicked_ally.x = player.child("cursor_sections").child("clicked_ally").attribute("x").as_int();
	cursor_clicked_ally.y = player.child("cursor_sections").child("clicked_ally").attribute("y").as_int();
	cursor_clicked_ally.w = player.child("cursor_sections").child("clicked_ally").attribute("w").as_int();
	cursor_clicked_ally.h = player.child("cursor_sections").child("clicked_ally").attribute("h").as_int();

	// -- Cursor CLICKED Enemy
	cursor_clicked_enemy.x = player.child("cursor_sections").child("clicked_enemy").attribute("x").as_int();
	cursor_clicked_enemy.y = player.child("cursor_sections").child("clicked_enemy").attribute("y").as_int();
	cursor_clicked_enemy.w = player.child("cursor_sections").child("clicked_enemy").attribute("w").as_int();
	cursor_clicked_enemy.h = player.child("cursor_sections").child("clicked_enemy").attribute("h").as_int();

	// -- Cursor CLICKED Resource
	cursor_clicked_resource.x	= player.child("cursor_sections").child("clicked_resource").attribute("x").as_int();
	cursor_clicked_resource.y = player.child("cursor_sections").child("clicked_resource").attribute("y").as_int();
	cursor_clicked_resource.w = player.child("cursor_sections").child("clicked_resource").attribute("w").as_int();
	cursor_clicked_resource.h = player.child("cursor_sections").child("clicked_resource").attribute("h").as_int();

	// -- Cursor CLICKED UI
	cursor_clicked_UI.x = player.child("cursor_sections").child("clicked_UI").attribute("x").as_int();
	cursor_clicked_UI.y = player.child("cursor_sections").child("clicked_UI").attribute("y").as_int();
	cursor_clicked_UI.w = player.child("cursor_sections").child("clicked_UI").attribute("w").as_int();
	cursor_clicked_UI.h = player.child("cursor_sections").child("clicked_UI").attribute("h").as_int();

	
	// --- UI_CURSOR ---
	iPoint mouse_pos;
	App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);

	cursor = GuiCursor(GUI_ELEMENT_TYPE::CURSOR, mouse_pos.x, mouse_pos.y, true, &idle, &cursor_clicked_idle, &cursor_hover_ally, &cursor_hover_enemy, &cursor_hover_resource, nullptr
		, &cursor_clicked_ally, &cursor_clicked_enemy, &cursor_clicked_resource, nullptr);
}
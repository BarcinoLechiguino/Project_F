#include "p2Log.h"

#include "Input.h"
#include "Render.h"
#include "Application.h"
#include "Window.h"
#include "Textures.h"
#include "EntityManager.h"
#include "Pathfinding.h"
#include "Map.h"
#include "SceneManager.h"
#include "Scene.h"

#include "EntityManager.h"
#include "Dynamic_Object.h"
#include "Static_Object.h"
#include "TownHall.h"
#include "Barracks.h"
#include "Gatherer.h"
#include "Infantry.h"

#include "Player.h"

Player::Player() : building_selected(nullptr), resource_selected(nullptr)
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
	cursor_idle = App->tex->Load("gui/cursor/cursor_idle.png");

	mouse_tile_debug = App->tex->Load("maps/mouse_tile_debug.png");

	scene_camera_limit = iPoint(0, 1000);

	camera_speed.x = 700.0f;
	camera_speed.y = 700.0f;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	is_selecting = false;

	return true;
}

bool Player::PreUpdate()
{
	return true;
}

bool Player::Update(float dt)
{
	MouseCalculations();
	
	CameraController(dt);

	DragSelection();

	SelectOnClick();
	
	DeleteOnInput();

	SelectionShortcuts();

	MoveToOrder();

	DebugUnitSpawn();

	return true;
}

bool Player::PostUpdate()
{
	DrawCursor();
	
	return true;
}

bool Player::CleanUp()
{
	App->tex->UnLoad(cursor_idle);
	App->tex->UnLoad(mouse_tile_debug);
	
	ClearEntityBuffers();
	
	return true;
}

// ------------------- CAMERA AND MOUSE METHODS -------------------
void Player::MouseCalculations()
{
	//Get Mouse Position
	App->input->GetMousePosition(mouse_position.x, mouse_position.y);

	iPoint mouse_camera;
	mouse_camera.x = mouse_position.x - App->render->camera.x;
	mouse_camera.y = mouse_position.y - App->render->camera.y;

	// Get Mouse's Map Position (In Tiles)
	iPoint to_map_mouse_pos		= App->map->WorldToMap(mouse_camera.x, mouse_camera.y + 20); //magic
	mouse_tile					= iPoint(to_map_mouse_pos.x, to_map_mouse_pos.y);

	// Get Mouse's World Poisition (In Pixels)
	iPoint to_world_mouse_pos	= App->map->MapToWorld(mouse_tile.x, mouse_tile.y);
	mouse_map_position			= iPoint(to_world_mouse_pos.x, to_world_mouse_pos.y);
}

void Player::CameraController(float dt)
{
	int window_width, window_height;
	App->win->GetWindowSize(window_width, window_height);
	
	if (CurrentlyInGameplayScene())										// If the current scene is FIRST_SCENE (gameplay scene)
	{
		if (mouse_position.x <= 10 || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)								//Left
		{
			App->render->camera.x += camera_speed.x * dt;
		}

		if (mouse_position.x >= (window_width - 10) || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)			//Right
		{
			App->render->camera.x -= camera_speed.x * dt;
		}

		if (mouse_position.y >= (window_height - 10) || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)			//Bottom
		{
			App->render->camera.y -= camera_speed.y * dt;
		}

		if (mouse_position.y <= 10 || App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)								//Up
		{
			App->render->camera.y += camera_speed.y * dt;
		}
	}
}

void Player::MoveToOrder()//fix
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		if (units_selected.size() != 0)																								// If there are Units being selected
		{
			if (App->pathfinding->IsWalkable(iPoint(mouse_tile.x, mouse_tile.y)))
			{
				for (std::vector<Dynamic_Object*>::iterator item = units_selected.begin(); item != units_selected.end(); item++)
				{
					App->pathfinding->ChangeWalkability((*item)->occupied_tile, (*item), WALKABLE);
				}

				App->pathfinding->FindNearbyWalkable(iPoint(mouse_tile.x, mouse_tile.y), units_selected);							//Gives units targets around main target
			}
			else
			{
				LOG("Tile cannot be targeted");
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
		App->render->Blit(mouse_tile_debug, mouse_map_position.x, mouse_map_position.y, nullptr, false, 1.f);
	}

	App->render->Blit(cursor_idle, mouse_position.x, mouse_position.y, nullptr, false, 0.f);
}

// ------------------- ENTITY SELECTION METHODS -------------------
void Player::DragSelection()
{
	if (CurrentlyInGameplayScene())
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			is_selecting = true;

			selection_start = mouse_position;

			units_selected.clear();
		}

		if (is_selecting)
		{
			UpdateSelectionRect();

			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
			{
				is_selecting = false;

				SelectEntitiesInSelectionRect();
			}
		}
	}
}

void Player::UpdateSelectionRect()
{
	//Cases with mouse pos
	if (mouse_position.x > selection_start.x)
	{
		selection_rect = { selection_start.x , selection_start.y, mouse_position.x - selection_start.x, mouse_position.y - selection_start.y };
	}
	else
	{
		selection_rect = { mouse_position.x ,selection_start.y, selection_start.x - mouse_position.x , mouse_position.y - selection_start.y };
	}

	if (mouse_position.y < selection_start.y)
	{
		selection_rect.y = mouse_position.y;
		selection_rect.h = selection_start.y - mouse_position.y;
	}

	App->render->DrawQuad(selection_rect, 150, 150, 255, 100, true, false);
}

void Player::SelectEntitiesInSelectionRect()
{
	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if (App->entity_manager->IsUnit((*item)))
		{
			Dynamic_Object* unit = (Dynamic_Object*)(*item);

			if (unit->is_selectable)
			{
				if (CheckSelectionRectBorders(unit))				//Collision ally units rectangles
				{
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
		if (!App->scene_manager->god_mode)
		{
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				SelectEntityAt(mouse_tile);
			}
		}
	}
}

void Player::SelectionShortcuts()
{
	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)													// Select All Entities
	{
		SelectAllEntities();
	}

	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)													// Select All Gatherers
	{
		SelectGatherers();
	}

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)													// Select All Infantries
	{
		SelectInfatries();
	}

	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
	{
		SelectEnemies();																				// Select All Enemies
	}
}

void Player::SelectAllEntities()
{
	units_selected.clear();

	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if (App->entity_manager->IsUnit((*item)))
		{
			Dynamic_Object* unit = (Dynamic_Object*)(*item);

			if (unit->is_selectable)
			{
				units_selected.push_back(unit);
			}
		}
	}

	LOG("All units selected. Total unit amount: %d", units_selected.size());
}

void Player::SelectGatherers()
{
	units_selected.clear();

	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if ((*item)->type == ENTITY_TYPE::GATHERER)
		{
			units_selected.push_back((Dynamic_Object*)(*item));
		}
	}

	LOG("All gatherers selected. Total gatherer amount: %d", units_selected.size());
}

void Player::SelectInfatries()
{
	units_selected.clear();

	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if ((*item)->type == ENTITY_TYPE::INFANTRY)
		{
			units_selected.push_back((Dynamic_Object*)(*item));
		}
	}

	LOG("All infantries selected. Total infantry amount: %d", units_selected.size());
}

void Player::SelectEnemies()
{
	if (App->scene_manager->god_mode)
	{
		units_selected.clear();

		std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

		for (; item != App->entity_manager->entities.end(); ++item)
		{
			if ((*item)->type == ENTITY_TYPE::ENEMY)
			{
				units_selected.push_back((Dynamic_Object*)(*item));
			}
		}

		LOG("All enemies selected. Total enemy amount: %d", units_selected.size());
	}
}

void Player::SelectEntityAt(const iPoint& tile_position)
{
	if (App->entity_manager->entity_map != nullptr)
	{
		Entity* clicked_entity = App->entity_manager->GetEntityAt(tile_position);						//clicked_entity will be assigned the entity in the entity_map at the given position.

		if (clicked_entity != nullptr)
		{
			if (App->entity_manager->IsUnit(clicked_entity))
			{
				units_selected.push_back((Dynamic_Object*)clicked_entity);
				return;
			}

			if (App->entity_manager->IsBuilding(clicked_entity))
			{
				building_selected = (Static_Object*)clicked_entity;

				LOG("A BUILDING WAS SELECTED AT TILE (%d, %d)", tile_position.x, tile_position.y);

				return;
			}

			if (App->entity_manager->IsResource(clicked_entity))
			{
				resource_selected = (Static_Object*)clicked_entity;
			}
		}
		else
		{
			units_selected.clear();

			LOG("There is no Entity at tile (%d, %d)", tile_position.x, tile_position.y);
		}
	}
}

// ------------------- ENTITY DELETION METHODS -------------------
void Player::DeleteOnInput()
{
	if (CurrentlyInGameplayScene())
	{
		if (App->scene_manager->god_mode)
		{
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)											// Delete entity at mouse position.
			{
				DeleteEntityAt(mouse_tile);
			}

			if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)											// Delete all entities.
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
		Entity* clicked_entity = App->entity_manager->GetEntityAt(mouse_tile);

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
		std::vector<Dynamic_Object*>::iterator item = units_selected.begin();

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
	units_selected.clear();

	building_selected = nullptr;
}

// ------------------- ENTITY SPAWN METHODS -------------------
void Player::DebugUnitSpawn()
{
	if (building_selected != nullptr)
	{
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		{
			if (building_selected->type == ENTITY_TYPE::TOWNHALL)
			{
				TownHall* townhall = (TownHall*)building_selected;
				townhall->GenerateUnit(ENTITY_TYPE::GATHERER);
			}
			if (building_selected->type == ENTITY_TYPE::BARRACKS)
			{
				Barracks* barrack = (Barracks*)building_selected;
				barrack->GenerateUnit(ENTITY_TYPE::INFANTRY);
			}
		}
	}
}

// ------------------- CONDITIONAL CHECKS -------------------
bool Player::CurrentlyInGameplayScene()
{
	if (App->scene_manager->current_scene->scene_name == SCENES::GAMEPLAY_SCENE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Player::CheckSelectionRectBorders(Dynamic_Object* unit)
{
	return (unit->selection_collider.x + unit->selection_collider.w > selection_rect.x - App->render->camera.x) &&		// Unit is inside the left border.
		(unit->selection_collider.x < selection_rect.x - App->render->camera.x + selection_rect.w) &&					// Unit is inside the right border.
		(unit->selection_collider.y + unit->selection_collider.h > selection_rect.y - App->render->camera.y) &&			// Unit is inside the top border.
		(unit->selection_collider.y < selection_rect.y - App->render->camera.y + selection_rect.h);						// Unit is inside the bottom border.
}
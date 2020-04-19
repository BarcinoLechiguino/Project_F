#include "p2Log.h"

#include "Input.h"
#include "Render.h"
#include "Application.h"
#include "Window.h"
#include "Audio.h"
#include "Textures.h"
#include "EntityManager.h"
#include "Pathfinding.h"
#include "Map.h"
#include "Minimap.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameplayScene.h"
#include "Gui.h"
#include "UI.h"

#include "EntityManager.h"
#include "Dynamic_Object.h"
#include "Static_Object.h"
#include "TownHall.h"
#include "EnemyTownHall.h"
#include "Barracks.h"
#include "EnemyBarracks.h"
#include "Gatherer.h"
#include "Infantry.h"



#include "Player.h"

Player::Player() : god_mode(nullptr), is_selecting(nullptr), building_selected(nullptr), resource_selected(nullptr)
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

	if (!App->gui->FirstElementUnderMouse()->CheckMousePos() && !App->pause)			//TMP. Dirty Fix(?)
	{
		DragSelection();

		SelectOnClick();

		DeleteOnInput();

		GiveOrder();
	}

	SelectionShortcuts();

	DebugUnitSpawn();

	DebugUnitUpgrade();

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

void Player::GiveOrder()//fix
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		if (units_selected.size() != 0)																								// If there are Units being selected
		{	
			if (App->entity_manager->GetEntityAt(mouse_tile) == nullptr)
			{
				OrderUnitsToMove();
			}
			else
			{
				OrderUnitsToAttack();
			}
		}
		else
		{
			LOG("There are no units being currently selected.");
		}
	}
}

void Player::OrderUnitsToMove()
{
	if (App->pathfinding->IsWalkable(mouse_tile))
	{
		std::vector<Dynamic_Object*>::iterator item = units_selected.begin();
		
		for (; item != units_selected.end(); item++)
		{
			if ((*item)->target != nullptr)
			{
				(*item)->target = nullptr;
			}

			App->pathfinding->ChangeWalkability((*item)->occupied_tile, (*item), WALKABLE);
		}

		App->pathfinding->FindNearbyWalkable(mouse_tile, units_selected);							//Gives units targets around main target
	}
	else
	{
		LOG("Tile cannot be targeted");
	}
}

void Player::OrderUnitsToAttack()
{
	Entity* target = App->entity_manager->GetEntityAt(mouse_tile);

	if (target != nullptr)
	{
		if (App->entity_manager->IsEnemyEntity(target))
		{
			std::vector<Dynamic_Object*> infantries;																		//Temporal fix. For now we only have infantries as combat units.

			for(int i = 0; i < units_selected.size(); ++i)
			{
				if (App->entity_manager->IsInfantry(units_selected[i]))
				{
					infantries.push_back(units_selected[i]);
				}
			}

			for (int i = 0; i < infantries.size(); ++i)
			{
				infantries[i]->target = target;
				App->pathfinding->ChangeWalkability(infantries[i]->occupied_tile, infantries[i], WALKABLE);
			}

			App->pathfinding->FindNearbyWalkable(target->tile_position, infantries);

			infantries.clear();
		}

		if (App->entity_manager->IsResource(target))
		{
			std::vector<Dynamic_Object*> gatherers;

			for (int i = 0; i < units_selected.size(); ++i)
			{
				if (App->entity_manager->IsGatherer(units_selected[i]))
				{
					gatherers.push_back(units_selected[i]);
				}
			}

			for (int i = 0; i < gatherers.size(); ++i)
			{
				gatherers[i]->target = target;
				App->pathfinding->ChangeWalkability(gatherers[i]->occupied_tile, gatherers[i], WALKABLE);
			}

			App->pathfinding->FindNearbyWalkable(target->tile_position, gatherers);							//Gives units targets around main target

			gatherers.clear();
		}
		else
		{
			LOG("Target entity is not an enemy entity.");
		}
	}
}

void Player::DrawCursor()
{
	if (CurrentlyInGameplayScene() && !App->gui->FirstElementUnderMouse()->CheckMousePos() && !App->pause)			//TMP. Dirty Fix(?)
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

			ClearEntityBuffers();
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
		SelectInfantries();
	}

	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
	{
		SelectEnemies();																				// Select All Enemies
	}
}

void Player::SelectAllEntities()
{
	ClearEntityBuffers();

	std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

	for (; item != App->entity_manager->entities.end(); ++item)
	{
		if (App->entity_manager->IsUnit((*item)))
		{
			Dynamic_Object* unit = (Dynamic_Object*)(*item);

			if (unit->is_selectable)
			{
				(*item)->is_selected = true;
				units_selected.push_back(unit);
			}
		}
	}

	LOG("All units selected. Total unit amount: %d", units_selected.size());
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
			units_selected.push_back((Dynamic_Object*)(*item));
		}
	}

	LOG("All gatherers selected. Total gatherer amount: %d", units_selected.size());
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
			units_selected.push_back((Dynamic_Object*)(*item));
		}
	}

	LOG("All infantries selected. Total infantry amount: %d", units_selected.size());
}

void Player::SelectEnemies()
{
	if (god_mode)
	{
		ClearEntityBuffers();

		std::vector<Entity*>::iterator item = App->entity_manager->entities.begin();

		for (; item != App->entity_manager->entities.end(); ++item)
		{
			if ((*item)->type == ENTITY_TYPE::ENEMY)
			{
				(*item)->is_selected = true;
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
				ClearEntityBuffers();
				
				Dynamic_Object* unit = (Dynamic_Object*)clicked_entity;

				if (unit->is_selectable)
				{
					unit->is_selected = true;
					units_selected.push_back((Dynamic_Object*)clicked_entity);
				}

				return;
			}

			if (App->entity_manager->IsBuilding(clicked_entity))
			{
				ClearEntityBuffers();
				
				clicked_entity->is_selected = true;
				building_selected = (Static_Object*)clicked_entity;

				//LOG("A BUILDING WAS SELECTED AT TILE (%d, %d)", tile_position.x, tile_position.y);

				return;
			}

			if (App->entity_manager->IsResource(clicked_entity))
			{
				ClearEntityBuffers();
				
				clicked_entity->is_selected = true;
				resource_selected = (Static_Object*)clicked_entity;
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
	if (units_selected.size() != 0)
	{
		for (int i = 0; i < units_selected.size(); ++i)
		{
			units_selected[i]->is_selected = false;
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
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		{
			TownHall* townhall = nullptr;
			EnemyTownHall* enemy_townhall = nullptr;
			Barracks* barrack = nullptr;
			EnemyBarracks* enemy_barrack;

			switch (building_selected->type)
			{
			case ENTITY_TYPE::TOWNHALL:
				townhall = (TownHall*)building_selected;
				townhall->GenerateUnit(ENTITY_TYPE::GATHERER, townhall->level);
				break;

			case ENTITY_TYPE::ENEMY_TOWNHALL:
				enemy_townhall = (EnemyTownHall*)building_selected;
				enemy_townhall->GenerateUnit(ENTITY_TYPE::GATHERER, enemy_townhall->level);
				break;
				
			case ENTITY_TYPE::BARRACKS:
				barrack = (Barracks*)building_selected;
				barrack->GenerateUnit(ENTITY_TYPE::INFANTRY, barrack->level);
				break;

			case ENTITY_TYPE::ENEMY_BARRACKS:
				enemy_barrack = (EnemyBarracks*)building_selected;
				enemy_barrack->GenerateUnit(ENTITY_TYPE::ENEMY, enemy_barrack->level);
				break;
			}
		}
	}
}

// ------------------- ENTITY SPAWN METHODS -------------------

void Player::DebugUnitUpgrade()
{
	if (building_selected != nullptr)
	{
		if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN)
		{
			TownHall* townhall = nullptr;
			EnemyTownHall* enemy_townhall = nullptr;
			Barracks* barrack = nullptr;
			EnemyBarracks* enemy_barrack;

			switch (building_selected->type)	
			{
			case ENTITY_TYPE::TOWNHALL:
				townhall = (TownHall*)building_selected;
				townhall->level++;
				townhall->LevelChanges();
				break;
			
			case ENTITY_TYPE::ENEMY_TOWNHALL:
				enemy_townhall = (EnemyTownHall*)building_selected;
				enemy_townhall->level++;
				enemy_townhall->LevelChanges();
				break;

			case ENTITY_TYPE::BARRACKS:
				barrack = (Barracks*)building_selected;
				barrack->level++;
				barrack->LevelChanges();
				break;

			case ENTITY_TYPE::ENEMY_BARRACKS:
				enemy_barrack = (EnemyBarracks*)building_selected;
				enemy_barrack->level++;
				enemy_barrack->LevelChanges();
				break;
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
	return (unit->selection_collider.x + unit->selection_collider.w > selection_rect.x - App->render->camera.x) &&			// Unit is inside the left border.
			(unit->selection_collider.x < selection_rect.x - App->render->camera.x + selection_rect.w) &&					// Unit is inside the right border.
			(unit->selection_collider.y + unit->selection_collider.h > selection_rect.y - App->render->camera.y) &&			// Unit is inside the top border.
			(unit->selection_collider.y < selection_rect.y - App->render->camera.y + selection_rect.h);						// Unit is inside the bottom border.
}
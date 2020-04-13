#include "Player.h"
#include "Input.h"
#include "Render.h"
#include "Application.h"
#include "Window.h"
#include "Textures.h"
#include "p2Log.h"
#include "EntityManager.h"
#include "Pathfinding.h"
#include "Map.h"
#include "SceneManager.h"

#include "EntityManager.h"
#include "Dynamic_Object.h"
#include "Static_Object.h"
#include "Gatherer.h"
#include "Infantry.h"

Player::Player() : building_selected(nullptr)
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

	SelectionRect();

	SelectionOnClick();

	SelectionShortcuts();

	MoveToOrder();

	return true;
}

bool Player::PostUpdate()
{
	DrawCursor();
	
	return true;
}

bool Player::CleanUp()
{
	return true;
}

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
					App->pathfinding->ChangeWalkability((*item)->occupied_tile, WALKABLE);
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

void Player::SelectionRect()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		is_selecting = true;

		selection_start = mouse_position;

		units_selected.clear();
	}

	if (is_selecting)
	{
		//Cases with mouse pos
		if (mouse_position.x > selection_start.x)
		{
			selection_rect = { selection_start.x ,selection_start.y, mouse_position.x - selection_start.x, mouse_position.y - selection_start.y };
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

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
		{
			is_selecting = false;
			
			for (std::vector<Dynamic_Object*>::iterator item = App->entity_manager->dynamic_objects.begin() ; item != App->entity_manager->dynamic_objects.end(); ++item)
			{
				if ((*item)->is_selectable)
				{
					/*LOG("camera %d, %d", -App->render->camera.x, -App->render->camera.y);
					LOG("Selection pos %d %d", selection_rect.x - App->render->camera.x , selection_rect.y - App->render->camera.y);
					LOG("gatherer rect %d %d", (*item)->selection_collider.x, (*item)->selection_collider.y);*/

					//Collision allied units rectangles
					if (((*item)->selection_collider.x + (*item)->selection_collider.w > selection_rect.x - App->render->camera.x) &&
						((*item)->selection_collider.x < selection_rect.x - App->render->camera.x + selection_rect.w) &&
						((*item)->selection_collider.y + (*item)->selection_collider.h > selection_rect.y - App->render->camera.y) &&
						((*item)->selection_collider.y < selection_rect.y - App->render->camera.y + selection_rect.h))
					{
						units_selected.push_back((*item));
						
					}
				}
			}

			LOG("Units selected %d", units_selected.size() );
		}
	}
}

void Player::SelectionOnClick()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		Entity* clicked_entity = App->entity_manager->GetEntityAt(mouse_tile);
		
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

				LOG("A BUILDING WAS SELECTED AT TILE (%d, %d)", mouse_tile.x, mouse_tile.y);

				return;
			}
		}
		else
		{
			units_selected.clear();
			
			LOG("There is no Entity at tile (%d, %d)", mouse_tile.x, mouse_tile.y);
		}
	}
}

void Player::SelectionShortcuts()
{
	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)													// Select All Entities
	{
		units_selected.clear();

		std::vector<Dynamic_Object*>::iterator item = App->entity_manager->dynamic_objects.begin();

		for (; item != App->entity_manager->dynamic_objects.end(); ++item)
		{
			units_selected.push_back((*item));
		}

		LOG("All units selected. Total unit amount: %d", units_selected.size());
	}

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)													// Select All Gatherers
	{
		units_selected.clear();

		std::vector<Gatherer*>::iterator item = App->entity_manager->gatherers.begin();

		for (; item != App->entity_manager->gatherers.end(); ++item)
		{
			units_selected.push_back((*item));
		}

		LOG("All gatherers selected. Total gatherer amount: %d", units_selected.size());
	}

	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)													// Select All Infantries
	{
		units_selected.clear();

		std::vector<Infantry*>::iterator item = App->entity_manager->infantries.begin();

		for (; item != App->entity_manager->infantries.end(); ++item)
		{
			units_selected.push_back((*item));
		}

		LOG("All infantries selected. Total infantry amount: %d", units_selected.size());
	}
}

void Player::DrawCursor() //fix (?)
{
	if (CurrentlyInGameplayScene())
	{
		App->render->Blit(mouse_tile_debug, mouse_map_position.x, mouse_map_position.y, nullptr, false, 1.f);
	}

	App->render->Blit(cursor_idle, mouse_position.x, mouse_position.y,nullptr,false,0.f);
}

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
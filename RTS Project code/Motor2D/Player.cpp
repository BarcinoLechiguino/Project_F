#include "Player.h"
#include "Input.h"
#include "Render.h"
#include "Application.h"
#include "Window.h"
#include "Textures.h"
#include "p2Log.h"
#include "EntityManager.h"
#include "Scene1.h"
#include "Pathfinding.h"
#include "Map.h"

Player::Player()
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

	selecting = false;

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

	MoveToOrder();

	Cursor();

	return true;
}

bool Player::PostUpdate()
{
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

	mouse_tile = iPoint(App->map->WorldToMap(mouse_camera.x, mouse_camera.y + 20).x, App->map->WorldToMap(mouse_camera.x, mouse_camera.y + 20).y); //magic

	mouse_map_position = iPoint(App->map->MapToWorld(mouse_tile.x, mouse_tile.y).x, App->map->MapToWorld(mouse_tile.x, mouse_tile.y).y);
}

void Player::MoveToOrder()//fix
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		int i = 0;
		for (std::vector<Dynamic_Object*>::iterator item = units_selected.begin(); item != units_selected.end(); item++)
		{
			if (App->pathfinding->IsWalkable( iPoint(mouse_tile.x + i,mouse_tile.y) ) )
			{
				(*item)->GiveNewTarget(iPoint(mouse_tile.x + i, mouse_tile.y));
			}
			LOG("path given");
			i++;
		}
	}

}

void Player::CameraController(float dt)
{
	int window_width, window_height;
	App->win->GetWindowSize(window_width, window_height);
	
	//Left
	if (mouse_position.x <= 10 || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		App->render->camera.x += camera_speed.x * dt;
	}
	////Right
	if (mouse_position.x >= ( window_width - 10) || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		App->render->camera.x -= camera_speed.x * dt;
	}
	//Bottom
	if (mouse_position.y >= (window_height - 10) || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		App->render->camera.y -= camera_speed.y * dt;
	}
	//Up
	if (mouse_position.y <= 10 || App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		App->render->camera.y += camera_speed.y * dt;
	}
}

void Player::SelectionRect()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		selecting = true;

		selection_start = mouse_position;

		units_selected.clear();
	}

	if (selecting)
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
			selecting = false;

			int num_selected = 0;

			for (std::vector<Dynamic_Object*>::iterator item = App->entityManager->dynamic_objects.begin() ; item != App->entityManager->dynamic_objects.end(); ++item)
			{
				if ((*item)->selectable_unit)
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
						num_selected++;
					}
				}
			}
			LOG("Units selected %d", num_selected);
		}
	}
}

void Player::Cursor() //fix
{
	App->render->Blit(mouse_tile_debug, mouse_map_position.x, mouse_map_position.y, nullptr, false, 1.f);

	App->render->Blit(cursor_idle, mouse_position.x, mouse_position.y,nullptr,false,0.f);
}
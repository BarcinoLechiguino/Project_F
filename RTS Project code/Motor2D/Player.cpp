#include "Player.h"
#include "Input.h"
#include "Render.h"
#include "Application.h"
#include "Window.h"
#include "Textures.h"
#include "p2Log.h"

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

	scene_camera_limit = iPoint(0, 1000);

	camera_speed.x = 500.0f;
	camera_speed.y = 500.0f;

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
	//Get Mouse Position
	App->input->GetMousePosition(mouse_position.x, mouse_position.y);

	CameraController(dt);

	SelectionRect();

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

void Player::CameraController(float dt)
{
	uint window_width, window_height;
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
	}

	if (selecting)
	{
		selection_rect = { selection_start.x,selection_start.y, mouse_position.x - selection_start.x, mouse_position.y - selection_start.y };

		App->render->DrawQuad(selection_rect, 150, 150, 255, 100, true, false);

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
		{
			selecting = false;
		}
	}
}

void Player::Cursor()
{

	App->render->Blit(cursor_idle, mouse_position.x, mouse_position.y,nullptr,false,0.f);

}
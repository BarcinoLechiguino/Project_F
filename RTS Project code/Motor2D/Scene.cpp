#include "Scene.h"
#include "Render.h"
#include "Input.h"
#include "Collisions.h"
#include "Map.h"
#include "Gui.h"

Scene::Scene(SCENES scene_name) : scene_name(scene_name), escape(true)		// This Here
{

}

Scene::~Scene()
{

}

bool Scene::Awake(pugi::xml_node& config)
{
	return true;
}

bool Scene::Start()
{
	return true;
}

bool Scene::PreUpdate()
{
	return true;
}

bool Scene::Update(float dt)
{
	return true;
}

bool Scene::PostUpdate()
{
	return true;
}

bool Scene::CleanUp()
{
	return true;
}

void Scene::InitScene()
{

}

void Scene::DrawScene()
{

}

void Scene::LoadGuiElements()
{

}

void Scene::ExecuteTransition()
{

}

void Scene::CameraDebugMovement(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		App->render->camera.y += floor(500.0f * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		App->render->camera.y -= floor(500.0f * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		App->render->camera.x += floor(500.0f * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		App->render->camera.x -= floor(500.0f * dt);
	}
}

void Scene::DebugKeys()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)			//Load First Level Key
	{
		//App->fadescene->FadeToBlack("Test_map.tmx");
	}

	//if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)			//Load Second Level Key
	//{
	//	App->fadescene->FadeToBlack("Test_map_2.tmx");
	//}

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		App->pause = !App->pause;
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)			//Save Game Key
	{
		App->SaveGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)			//Load Game Key
	{
		App->LoadGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)			//Enable / Diable free camera movement Key
	{
		//App->render->cam.camera_debug = !App->render->cam.camera_debug;
	}

	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)			//PathfindingCollisions meta layer Debug Key
	{
		App->gui->ui_debug = !App->gui->ui_debug;
	}

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)			//Collider Debug Key
	{
		App->collisions->collider_debug = !App->collisions->collider_debug;
		App->map->pathfindingMetaDebug = !App->map->pathfindingMetaDebug;
	}

	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)		//Enabling / Disabling frame cap
	{
		//App->framesAreCapped = !App->framesAreCapped;
		if (App->frame_cap == CAP_AT_60)
		{
			App->frame_cap = CAP_AT_30;
		}
		else
		{
			App->frame_cap = CAP_AT_60;
		}
	}
}
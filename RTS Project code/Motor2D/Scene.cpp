#include "Application.h"
#include "Render.h"
#include "Input.h"
#include "Collisions.h"
#include "Map.h"
#include "Gui.h"
#include "Player.h"
#include "SceneManager.h"
#include "EntityManager.h"

#include "Scene.h"

Scene::Scene(SCENES scene_name) : scene_name(scene_name), escape(true), occupied_debug(nullptr), occupied_by_entity_debug(nullptr),map_height(0),map_width(0)// This Here
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
	if (App->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT)
	{
		App->render->camera.y += (int)floor(500.0f * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_REPEAT)
	{
		App->render->camera.y -= (int)floor(500.0f * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_REPEAT)
	{
		App->render->camera.x += (int)floor(500.0f * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_REPEAT)
	{
		App->render->camera.x -= (int)floor(500.0f * dt);
	}
}

void Scene::DebugKeys()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KeyState::KEY_DOWN)						// Enable/Disable God Mode.
	{

		if (App->player->CurrentlyInGameplayScene())
		{
			App->player->god_mode = !App->player->god_mode;
		}
		
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KeyState::KEY_DOWN)						// Switch the frame cap between 60 and 30 FPS.
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

	if (App->input->GetKey(SDL_SCANCODE_F3) == KeyState::KEY_DOWN)						//Enable/Disable Pause Mode.
	{
		if (App->player->CurrentlyInGameplayScene())
		{
			App->pause = !App->pause;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F4) == KeyState::KEY_DOWN)						//Enable/Disable UI_Debug Key. (Display all UI elements hitboxes)
	{
		App->gui->ui_debug = !App->gui->ui_debug;
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KeyState::KEY_DOWN)						//Save Game Key
	{
		//App->SaveGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KeyState::KEY_DOWN)						//Load Game Key
	{
		//App->LoadGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F7) == KeyState::KEY_DOWN)						//Enable/Disable pathfinding meta debug. (Display pathfinding meta tiles)
	{
		if (App->player->CurrentlyInGameplayScene())
		{
			App->map->pathfinding_meta_debug = !App->map->pathfinding_meta_debug;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F8) == KeyState::KEY_DOWN)						//Enable/Disable walkability map debug. (Display walkability map tiles)
	{
		if (App->player->CurrentlyInGameplayScene())
		{
			App->map->walkability_debug = !App->map->walkability_debug;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F9) == KeyState::KEY_DOWN)						//Enable/Dasable entity map debug. (Display entity map tiles)
	{
		if (App->player->CurrentlyInGameplayScene())
		{
			App->map->entity_map_debug = !App->map->entity_map_debug;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KeyState::KEY_DOWN)						//Enable/Disable collider debug. (Display all colliders.)
	{
		App->collisions->collider_debug = !App->collisions->collider_debug;
	}

	if (App->input->GetKey(SDL_SCANCODE_F11) == KeyState::KEY_DOWN)
	{
		App->entity_manager->debug_center_point = !App->entity_manager->debug_center_point;
	}

	if (App->input->GetKey(SDL_SCANCODE_P) == KeyState::KEY_DOWN)
	{
		App->map->smaller_camera = !App->map->smaller_camera;
	}

}
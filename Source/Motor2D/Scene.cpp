#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Input.h"
#include "Map.h"
#include "EntityManager.h"
#include "GuiManager.h"
#include "Player.h"
#include "FowManager.h"
#include "SceneManager.h"
#include "TransitionManager.h"

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
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_STATE::KEY_REPEAT)
	{
		App->render->camera.y += (int)floor(500.0f * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_STATE::KEY_REPEAT)
	{
		App->render->camera.y -= (int)floor(500.0f * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_STATE::KEY_REPEAT)
	{
		App->render->camera.x += (int)floor(500.0f * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_STATE::KEY_REPEAT)
	{
		App->render->camera.x -= (int)floor(500.0f * dt);
	}
}

void Scene::DebugKeys()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_STATE::KEY_DOWN)						// Enable/Disable God Mode.
	{
		if (App->player->CurrentlyInGameplayScene())
		{
			App->player->god_mode = !App->player->god_mode;
			
			App->fow_manager->fow_debug = !App->fow_manager->fow_debug;					// TMP
			App->fow_manager->SwapVisibilityMaps();
		}	
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_STATE::KEY_DOWN)						// Switch the frame cap between 60 and 30 FPS.
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

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_STATE::KEY_DOWN)						//Enable/Disable Pause Mode.
	{
		if (App->player->CurrentlyInGameplayScene())
		{
			App->pause = !App->pause;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_STATE::KEY_DOWN)						//Enable/Disable UI_Debug Key. (Display all UI elements hitboxes)
	{
		App->gui_manager->ui_debug = !App->gui_manager->ui_debug;
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_STATE::KEY_DOWN)						//Save Game Key
	{
		//App->SaveGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_STATE::KEY_DOWN)						//Load Game Key
	{
		//App->LoadGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_STATE::KEY_DOWN)						//Enable/Disable pathfinding meta debug. (Display pathfinding meta tiles)
	{
		if (App->player->CurrentlyInGameplayScene())
		{
			App->map->pathfinding_meta_debug = !App->map->pathfinding_meta_debug;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_STATE::KEY_DOWN)						//Enable/Disable walkability map debug. (Display walkability map tiles)
	{
		if (App->player->CurrentlyInGameplayScene())
		{
			App->map->walkability_debug = !App->map->walkability_debug;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_STATE::KEY_DOWN)						//Enable/Dasable entity map debug. (Display entity map tiles)
	{
		if (App->player->CurrentlyInGameplayScene())
		{
			App->map->entity_map_debug = !App->map->entity_map_debug;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_STATE::KEY_DOWN)						//Enable/Disable collider debug. (Display all colliders.)
	{
		App->entity_manager->debug_center_point = !App->entity_manager->debug_center_point;
	}

	if (App->input->GetKey(SDL_SCANCODE_HOME) == KEY_STATE::KEY_DOWN)
	{
		if (App->player->CurrentlyInGameplayScene())
		{
			App->map->smaller_camera = !App->map->smaller_camera;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_STATE::KEY_DOWN)
	{
		App->win->ToggleFullscreen();
	}

	if (App->input->GetKey(SDL_SCANCODE_END) == KEY_STATE::KEY_DOWN)
	{
		App->fow_manager->fow_debug = !App->fow_manager->fow_debug;
		App->fow_manager->SwapVisibilityMaps();
	}
}

void Scene::ExecuteDebugTransition()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_STATE::KEY_DOWN)
	{
		if (scene_name != SCENES::LOGO_SCENE)
		{
			if (App->pause)
			{
				App->pause = false;
			}

			App->transition_manager->CreateExpandingBars(SCENES::LOGO_SCENE, 0.5f, true, 3, true, true);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_STATE::KEY_DOWN)
	{
		if (scene_name != SCENES::MAIN_MENU_SCENE)
		{
			if (App->pause)
			{
				App->pause = false;
			}
			
			App->transition_manager->CreateExpandingBars(SCENES::MAIN_MENU_SCENE, 0.5f, true, 3, false, true);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_STATE::KEY_DOWN)
	{
		if (scene_name != SCENES::OPTIONS_SCENE)
		{
			if (App->pause)
			{
				App->pause = false;
			}
			
			App->transition_manager->CreateExpandingBars(SCENES::OPTIONS_SCENE, 0.5f, true, 5, false, true);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_STATE::KEY_DOWN)
	{
		if (scene_name != SCENES::GAMEPLAY_SCENE)
		{
			if (App->pause)
			{
				App->pause = false;
			}
			
			App->transition_manager->CreateExpandingBars(SCENES::GAMEPLAY_SCENE, 0.5f, true, 7, false, true);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_STATE::KEY_DOWN)
	{
		if (scene_name != SCENES::WIN_SCENE)
		{
			if (App->pause)
			{
				App->pause = false;
			}
			
			App->transition_manager->CreateExpandingBars(SCENES::WIN_SCENE, 0.5f, true, 7, true, true);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_6) == KEY_STATE::KEY_DOWN)
	{
		if (scene_name != SCENES::LOSE_SCENE)
		{
			if (App->pause)
			{
				App->pause = false;
			}
			
			App->transition_manager->CreateExpandingBars(SCENES::LOSE_SCENE, 0.5f, true, 7, true, true);
		}
	}
}
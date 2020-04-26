#include "p2Log.h"

#include "Application.h"
#include "Render.h"
#include "Input.h"

#include "SceneManager.h"
#include "Scene.h"
#include "LogoScene.h"
#include "MainMenuScene.h"
#include "OptionsScene.h"
#include "GameplayScene.h"
#include "WinScene.h"
#include "LoseScene.h"


SceneManager::SceneManager() : current_scene(nullptr)
{
	name = "scene_manager";
}

SceneManager::~SceneManager()
{

}

bool SceneManager::Awake(pugi::xml_node& config)
{
	bool ret = true;
	
	ScenePushbacks();
	
	LoadInitialScene();

	current_scene->Awake(config);

	return ret;
}

bool SceneManager::Start()
{
	bool ret = true;

	current_scene->Start();

	return ret;
}

bool SceneManager::PreUpdate()
{
	bool ret = true;

	current_scene->PreUpdate();

	return ret;
}

bool SceneManager::Update(float dt)
{
	bool ret = true;

	current_scene->Update(dt);

	return ret;
}

bool SceneManager::PostUpdate()
{
	bool ret = true;

	current_scene->PostUpdate();

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_STATE::KEY_DOWN)
	{
		ret = false;
	}

	return current_scene->escape;
}

bool SceneManager::CleanUp()
{
	bool ret = true;

	std::vector<Scene*>::iterator item = scenes.begin();

	for (; item != scenes.end(); ++item)
	{
		RELEASE((*item));
	}
	scenes.clear();

	return ret;
}

// -------------- SCENE MANAGEMENT METHODS --------------
void SceneManager::LoadInitialScene()
{
	current_scene = (*scenes.begin());
}

void SceneManager::SwitchScene(SCENES scene_name)
{	
	current_scene->CleanUp();

	std::vector<Scene*>::iterator item = scenes.begin();

	for (; item != scenes.end(); ++item)
	{
		if ((*item)->scene_name == scene_name)
		{
			current_scene = (*item);
		}
	}

	if (current_scene != nullptr)
	{
 		current_scene->Awake(config.child(current_scene->name_tag.c_str()));
		current_scene->Start();
	}
}

void SceneManager::LoadScene(SCENES scene_name)
{
	std::vector<Scene*>::iterator item = scenes.begin();

	for (; item != scenes.end(); ++item)
	{
		if ((*item)->scene_name == scene_name)
		{
			if (current_scene == nullptr)
			{
				current_scene = (*item);
			}
			else
			{
				if (next_scene == nullptr)
				{
					next_scene = (*item);
				}
				else
				{
					next_scene->CleanUp();

					next_scene = (*item);
				}
			}
		}
	}

	next_scene->Start();
}

void SceneManager::UnloadScene(Scene* scene_to_unload)
{
	std::vector<Scene*>::iterator item = scenes.begin();

	for (; item != scenes.end(); ++item)
	{
		if ((*item) == scene_to_unload)
		{
			if ((*item) == current_scene)
			{
				if (next_scene != nullptr)															// Cleaning up current_scene. next_scene is re-assigned to current_scene first.
				{
					Scene* tmp = current_scene;

					current_scene = next_scene;

					next_scene = tmp;

					tmp = nullptr;

					next_scene->CleanUp();

					next_scene = nullptr;
				}
				else
				{
					LOG("UNLOAD SCENE ERROR: Two scenes were not active at the same time.");
				}

				break;
			}
			else
			{
				if (current_scene != nullptr)
				{
					(*item)->CleanUp();

					next_scene = nullptr;
				}
				else
				{
					LOG("UNLOAD SCENE ERROR: Two scenes were not active at the same time.");
				}
				
				break;
			}
		}
	}
}

// ---------------- CREATE SCENE METHODS ----------------
Scene* SceneManager::CreateScene(SCENES scene_name)
{
	Scene* item = nullptr;

	switch (scene_name)
	{
	case SCENES::LOGO_SCENE:

		item = new LogoScene();

		break;

	case SCENES::MAIN_MENU_SCENE:

		item = new MainMenuScene();

		break;

	case SCENES::OPTIONS_SCENE:

		item = new OptionsScene();

		break;

	case SCENES::GAMEPLAY_SCENE:
		
		item = new GameplayScene();
		
	break;

	case SCENES::WIN_SCENE:

		item = new WinScene();

		break;

	case SCENES::LOSE_SCENE:

		item = new LoseScene();

		break;

	case SCENES::NONE:
		
		break;
	}

	if (item != nullptr)
	{
		scenes.push_back(item);
	}

	return item;
}

void SceneManager::ScenePushbacks()
{
	CreateScene(SCENES::LOGO_SCENE);
	CreateScene(SCENES::MAIN_MENU_SCENE);
	CreateScene(SCENES::OPTIONS_SCENE);
	CreateScene(SCENES::GAMEPLAY_SCENE);
	CreateScene(SCENES::WIN_SCENE);
	CreateScene(SCENES::LOSE_SCENE);
}
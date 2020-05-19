#include "QuestManager.h"
#include "Module.h"
#include "Point.h"
#include "Log.h"
#include <vector>
#include "EntityManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameplayScene.h"
#include "Application.h"

QuestManager::QuestManager() {}


QuestManager::~QuestManager()
{
	for (std::vector <Quest*>::iterator it = loaded_quests.begin(); it != loaded_quests.end(); it++)
	{
		loaded_quests.erase(it);
	}
	for (std::vector <Quest*>::iterator it = quests.begin(); it != quests.end(); it++)
	{
		quests.erase(it);
	}
}

Quest::~Quest()
{

}

bool QuestManager::Awake(pugi::xml_node& config)
{
	LOG("STARTING QUEST_MANAGER");

	pugi::xml_node quest_node;
	quest_node = LoadQuests(quest_data);  //Loads the xml file that you pass in the "xmlfile" and returns a node

	if (quest_node == NULL)
	{
		LOG("Could not load quests_file.xml");
	}

	for (quest_node = quest_node.child("quest"); quest_node; quest_node = quest_node.next_sibling("quest"))
	{

		Quest* new_quest = new Quest();

		new_quest->id = quest_node.attribute("id").as_int();
		new_quest->title = quest_node.attribute("title").as_string();
		new_quest->trigger = quest_node.attribute("trigger").as_int();
		new_quest->requisites = quest_node.attribute("requisites").as_int();
		new_quest->description = quest_node.attribute("description").as_string();


		if (new_quest->trigger == 1)
		{
			quests.push_back(new_quest);
		}
		else
		{
			loaded_quests.push_back(new_quest);
		}
	}

	return true;
}


bool QuestManager::Start()
{

	return true;
}

bool QuestManager::Update(float dt)
{
	if (App->scene_manager->current_scene->scene_name == SCENES::GAMEPLAY_SCENE)
	{
		CheckQuests();
	}

	return true;
}

pugi::xml_node QuestManager::LoadQuests(pugi::xml_document& file) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = file.load_file("quest_data.xml");

	if (result == NULL)
	{
		LOG("Could not load  xml file <loadxmlfunction> pugi error: %s", result.description());
	}
	else
	{
		ret = file.child("quests");
		LOG("XML LOADED");
	}

	return ret;
}

void QuestManager::CheckQuests()
{
	for (std::vector<Quest*>::iterator it = quests.begin(); it != quests.end(); it++)
	{
		int quest_id = (*it)->id;

		switch (quest_id)
		{
		case 0:
			if (App->scene_manager->gameplay_scene->CheckForTownHall() == false && ((*it)->requisites == 0))
			{
				(*it)->completed = true;	
			}
			break;

		case 1:
			if (App->entity_manager->resource_data > 0 && App->entity_manager->resource_electricity > 0 && ((*it)->requisites == 0))
			{
				(*it)->completed = true;				
			}
			break;

		case 2:
			if (App->entity_manager->resource_bits >= 3 && ((*it)->requisites == 0))
			{
				(*it)->completed = true;
			}
			break;

		case 3:
			if (App->entity_manager->kill_count >= 10)
			{
				(*it)->completed = true;
			}
			break;
		default:
			break;
		}
	}
}


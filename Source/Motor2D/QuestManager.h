#ifndef __QUEST_MANAGER_H__
#define __QUEST_MANAGER_H__

#include "Dependencies/SDL/include/SDL_rect.h"

#include "Module.h"
#include "Point.h"
#include <vector>

class Quest
{
public:
	Quest();
	~Quest();

public:
	int id;
	int trigger;					//whether or not a quest is active
	int requisites;

	std::string title;
	std::string description;

	bool completed = false;
};

class QuestManager : public Module
{
public:
	QuestManager();
	~QuestManager();

	bool Awake(pugi::xml_node& file);
	bool Start();
	bool Update(float dt);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	pugi::xml_node LoadQuests(pugi::xml_document& quest_file) const;
	void CheckQuests();

public:
	pugi::xml_document quest_data;

	std::vector<Quest*> loaded_quests;
	std::vector<Quest*> quests;
};

#endif // !__QUEST_MANAGER_H__
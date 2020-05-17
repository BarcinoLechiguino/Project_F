#include "Module.h"
#include "Point.h"
#include <vector>
#include <list>
#include "Dependencies/SDL/include/SDL_rect.h"


class Quest
{
public:

	Quest() {};
	~Quest();

	int id = 0;
	int trigger = 0;	//whether or not a quest is active
	int requisites = 0;

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

	void CheckQuests();

	pugi::xml_node LoadQuests(pugi::xml_document& quest_file) const;

	pugi::xml_document quest_data;

	std::vector<Quest*> loaded_quests;
	std::vector<Quest*> quests;
};



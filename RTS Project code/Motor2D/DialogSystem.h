#ifndef __DIALOGSYSTEM_H__
#define __DIALOGSYSTEM_H__

#include <string>
#include <vector>
#include "Module.h"

//Options to be chosen
class DialogOption
{
public:

	DialogOption();
	DialogOption(const char text, int next_node);

public:

	std::string text;
	int next_node;
};

//NPC phrase
class DialogNode
{
public:

	DialogNode();
	DialogNode(const char text, int node_id);

public:

	std::string text;
	std::vector <DialogOption*> dialog_options;
	int node_id;

};

//Tree storing all the nodes and options
class DialogTree
{
public:

	DialogTree();

public:

	std::vector <DialogNode*> dialog_nodes;
	int tree_id;
};

class DialogSystem : public Module
{
public:
	DialogSystem();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpddate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();


public:

	std::vector <DialogTree*> dialog_trees;

};
#endif // __DIALOGSYSTEM_H__
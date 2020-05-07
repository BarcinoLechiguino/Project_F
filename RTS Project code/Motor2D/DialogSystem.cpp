#include "DialogSystem.h"
#include "p2Log.h"

DialogOption::DialogOption()
{
	next_node = 0;

	text = "I have no text";
}

DialogOption::DialogOption(const char text, int next_node)
{
	this->text = text;

	this->next_node = next_node;
}

DialogNode::DialogNode()
{
	node_id = 0;

	text = "I have nothing to say";
}

DialogNode::DialogNode(const char text, int node_id)
{
	this->node_id = node_id;

	this->text = text;
}

DialogTree::DialogTree()
{
	tree_id = 0;
}

DialogSystem::DialogSystem()
{

}

bool DialogSystem::Awake(pugi::xml_node&)
{
	
	return true;
}

bool DialogSystem::Start()
{
	return true;
}

bool DialogSystem::PreUpddate()
{
	return true;
}

bool DialogSystem::Update(float dt)
{
	return true;
}

bool DialogSystem::PostUpdate()
{
	return true;
}

bool DialogSystem::CleanUp()
{
	return true;
}
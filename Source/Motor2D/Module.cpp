#include "Module.h"

Module::Module() : is_active(true)
{

}

void Module::Init()
{

}

bool Module::Awake(pugi::xml_node&)
{
	return true;
}

bool Module::Start()
{
	return true;
}

bool Module::PreUpdate()
{
	return true;
}

bool Module::Update(float dt)
{
	return true;
}

bool Module::PostUpdate()
{
	return true;
}

bool Module::CleanUp()
{
	return true;
}

void Module::Enable()
{
	if (!is_active)
	{
		is_active = true;
		Start();
	}
}

void Module::Disable()
{
	if (is_active)
	{
		is_active = false;
		CleanUp();
	}
}

// --- Other General Methods
bool Module::Load(pugi::xml_node&)
{
	return true;
}

bool Module::Save(pugi::xml_node&) const
{
	return true;
}

void Module::OnEventCall(GuiElement* element, GUI_EVENT ui_event)
{
	return;
}
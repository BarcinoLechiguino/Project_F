// ----------------------------------------------------
// Module.h
// Interface for all engine modules
// ----------------------------------------------------

#ifndef __MODULE_H__
#define __MODULE_H__

#include "PugiXml\src\pugixml.hpp"
#include <string>
#include <vector>
#include <list>

struct		SDL_Texture;
struct		SDL_Rect;
class		Application;

struct		Collider;

class		GuiElement;
enum class	GUI_EVENT;

class Module
{
public:

	Module() : is_active(true)
	{}

	void Init()
	{
		
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

	void Enable()																	//Start()
	{
		if (!is_active)
		{
			is_active = true;
			Start();
		}
	}

	void Disable()																	//CleanUp()
	{
		if (is_active)
		{
			is_active = false;
			CleanUp();
		}
	}

	virtual void OnCollision(Collider* C1, Collider* C2) {}							//Collision handling

	virtual void OnEventCall(GuiElement* element, GUI_EVENT ui_event)
	{
		return;
	}

	virtual void OnCommand(const char* command, const char* subCommand = nullptr)
	{
		return;
	}

public:
	std::string name;
	bool		is_active;
};

#endif // __MODULE_H__
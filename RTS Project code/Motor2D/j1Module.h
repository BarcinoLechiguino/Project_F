// ----------------------------------------------------
// j1Module.h
// Interface for all engine modules
// ----------------------------------------------------

#ifndef __j1MODULE_H__
#define __j1MODULE_H__

#include "PugiXml\src\pugixml.hpp"
#include <string>

class j1App;
struct SDL_Texture;
struct SDL_Rect; 
class Animation; 

struct Collider;

class UI;
enum class UI_Event;

class j1Module
{
public:

	j1Module() : is_active(true)
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

	void Enable()	//Start()
	{
		if (!is_active)
		{
			is_active = true;
			Start();
		}
	}

	void Disable()	//CleanUp()
	{
		if (is_active)
		{
			is_active = false;
			CleanUp();
		}
	}

	//Collision handling
	virtual void OnCollision(Collider* C1, Collider* C2) {}

	virtual void OnEventCall(UI* element, UI_Event ui_event)
	{
		return;
	}

	virtual void OnCommand(const char* command, const char* subCommand = nullptr)
	{
		return;
	}

public:

	std::string name;
	bool is_active;
};

#endif // __j1MODULE_H__
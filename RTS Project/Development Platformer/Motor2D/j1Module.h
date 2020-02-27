// ----------------------------------------------------
// j1Module.h
// Interface for all engine modules
// ----------------------------------------------------

#ifndef __j1MODULE_H__
#define __j1MODULE_H__

#include "p2SString.h"
#include "PugiXml\src\pugixml.hpp"

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

	j1Module() : active(false)
	{}

	void Init()
	{
		active = true;
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

	void Enable()
	{
		if (active == false)
		{
			active = true;
			Start();
		}
	}

	void Disable()
	{
		if (active == true)
		{
			active = false;
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

	p2SString name;
	bool active;
	bool enabled = true;
};

#endif // __j1MODULE_H__
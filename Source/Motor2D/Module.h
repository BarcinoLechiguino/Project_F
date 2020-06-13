// ----------------------------------------------------
// Module.h
// Interface for all engine modules
// ----------------------------------------------------

#ifndef __MODULE_H__
#define __MODULE_H__

#include "Dependencies\PugiXml\src\pugixml.hpp"
#include <string>
#include <vector>
#include <list>

struct		SDL_Texture;
struct		SDL_Rect;
class		Application;

class		GuiElement;
enum class	GUI_EVENT;

class Module
{
public:
	Module();

	void Init();

	virtual bool Awake(pugi::xml_node&);			// Called before render is available
	virtual bool Start();							// Called before the first frame
	virtual bool PreUpdate();						// Called each loop iteration
	virtual bool Update(float dt);					// Called each loop iteration
	virtual bool PostUpdate();						// Called each loop iteration
	virtual bool CleanUp();							// Called before quitting

	void Enable();									//Start()
	void Disable();									//CleanUp()

	virtual bool Load(pugi::xml_node&);
	virtual bool Save(pugi::xml_node&);

	virtual void OnEventCall(GuiElement* element, GUI_EVENT ui_event);				// Gui Event Handling

public:
	std::string name;
	bool		is_active;
};

#endif // __MODULE_H__
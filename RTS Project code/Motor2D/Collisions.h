#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include "SDL/include/SDL_rect.h"
#include "Module.h"

enum Object_Type;	//If it was enum class we would need to refer to the types with Object_Type::SOLID for examole.
struct ObjectData;	//Declaration gives access to Object Data from here.

struct Collider
{
	SDL_Rect		collider;					//Rectangle that will be assigned to the collider.
	Object_Type		type;						//Type that the collider will have. The type will be passed by the objects from the tmx file.
	Module*			callback = NULL;			//Pointer that, when CheckCollision retuns true, it calls the OnCollision() method in the player1.cpp and player2.cpp and passes them 2 colliders as arguments.
	bool			to_delete;					//Used to delete colliders that are not needed anymore or

	Collider(SDL_Rect collider, Object_Type type, Module* callback = NULL) {};

	Collider(ObjectData object);

	Collider() {};

	void Set_Position(int x, int y)
	{
		collider.x = x;
		collider.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const;
};

class Collisions : public Module 
{
	//Methods
public:

	Collisions();

	bool Init();

	// Destructor
	virtual ~Collisions() {};

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();
	 
	// Called before quitting
	bool CleanUp();

	Collider* AddCollider(SDL_Rect collider, Object_Type type, Module* callback);
	void LoadColliderFromMap();

private:
	void Collider_Debug();

public:
	//Variables
	Collider collider; //Call to struct from the collision module.
	bool collider_debug;

	std::vector<Collider*> collider_list;

private:

};
#endif // __Collisions_H__
#ifndef __j1COLLISIONS_H__
#define __j1COLLISIONS_H__

#include "j1Module.h"
#include "SDL/include/SDL.h"

enum Object_Type;	//If it was enum class we would need to refer to the types with Object_Type::SOLID for examole.
struct ObjectData;	//Declaration gives access to Object Data from here.

struct Collider
{
	SDL_Rect		collider;					//Rectangle that will be assigned to the collider.
	Object_Type		type;						//Type that the collider will have. The type will be passed by the objects from the tmx file.
	j1Module*		callback = NULL;			//Pointer that, when Check_Collision retuns true, it calls the OnCollision() method in the player1.cpp and player2.cpp and passes them 2 colliders as arguments.
	bool			to_delete;					//Used to delete colliders that are not needed anymore or

	Collider(SDL_Rect collider, Object_Type type, j1Module* callback = NULL) {};

	Collider(ObjectData object);

	Collider() {};

	void Set_Position(int x, int y)
	{
		collider.x = x;
		collider.y = y;
	}

	bool Check_Collision(const SDL_Rect& r) const;
};

class j1Collisions : public j1Module 
{
	//Methods
public:

	j1Collisions();

	bool Init();

	// Destructor
	virtual ~j1Collisions() {};

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

	Collider* AddCollider(SDL_Rect collider, Object_Type type, j1Module* callback);
	void LoadColliderFromMap();

private:
	void Collider_Debug();

public:
	//Variables
	Collider collider; //Call to struct from the j1 collision module.
	p2List<Collider*> collider_list;
	bool collider_debug;

private:

};
#endif // __j1Collisions_H__
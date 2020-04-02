#include "Application.h"
#include "Module.h"
#include "Point.h"
#include "Render.h"
#include "Input.h"
#include "p2Log.h"
#include "Map.h"
#include "Scene1.h"
#include "Textures.h"
#include "Collisions.h"
#include "Audio.h"
#include "Pathfinding.h"

#include "EntityManager.h"


Infantry::Infantry(int x, int y, ENTITY_TYPE type) : Dynamic_Object(x, y, type)  //Constructor. Called at the first frame.
{
	entity_sprite = App->tex->Load("maps/debug_infantry_tile.png");

	selectable_unit = true;

	speed = 500;
	health = 300;
	damage = 30;

	SDL_Rect empty_rect{ 0,158,230,18 };
	empty_bar = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, int(pixel_position.x), int(pixel_position.y) - 30, empty_rect, true, false, false,nullptr,this, NULL); //Magic Number

	SDL_Rect health_rect{ 0,198,230,18 };
	health_bar = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, int(pixel_position.x), int(pixel_position.y) - 30, health_rect, true, false, false, nullptr, this, NULL); //Magic Number
};

Infantry::~Infantry()  //Destructor. Called at the last frame.
{

};

bool Infantry::Awake(pugi::xml_node& config)
{
	return true;
};

bool Infantry::Start()
{
	return true;
};

bool Infantry::PreUpdate()
{
	return true;
};

bool Infantry::Update(float dt, bool doLogic)
{
	HandleMovement(dt);

	selection_collider.x = pixel_position.x;
	selection_collider.y = pixel_position.y;

	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y, nullptr);

	App->render->DrawQuad(selection_collider, 255, 255, 0, 100);


	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
	{
		ApplyDamage(this);
		LOG("Infantry health: %f", health);
	}

	return true;
};

bool Infantry::PostUpdate()
{
	return true;
};

bool Infantry::CleanUp()
{
	return true;
};

// Collision Handling ---------------------------------------
void Infantry::OnCollision(Collider* C1, Collider* C2)
{
	return;
}

void Infantry::ApplyDamage(Entity* attacker)
{
	health -= attacker->damage;
	return;
}
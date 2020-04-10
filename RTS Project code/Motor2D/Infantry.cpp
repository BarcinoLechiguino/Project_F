#include "p2Log.h"
#include "Point.h"
#include "Application.h"
#include "Module.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Collisions.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Gui.h"

#include "EntityManager.h"


Infantry::Infantry(int x, int y, ENTITY_TYPE type) : Dynamic_Object(x, y, type)  //Constructor. Called at the first frame.
{
	entity_sprite = App->tex->Load("maps/debug_infantry_tile.png");

	selectable_unit = true;

	speed = 500;

	max_health = 300;
	current_health = max_health;
	damage = 30;

	healthbar_background_rect = { 618, 12, MAX_UNIT_HEALTHBAR_WIDTH, 9 };
	healthbar_rect = { 618, 23, MAX_UNIT_HEALTHBAR_WIDTH, 9 };

	healthbar = (UI_Healthbar*)App->gui->CreateHealthbar(UI_Element::HEALTHBAR, (int)pixel_position.x, (int)pixel_position.y - 30, true, &healthbar_rect, &healthbar_background_rect, this); //Magic Number
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

	if (current_health < 300)
	{
		LOG("Infantry health: %d", current_health);
	}

	return true;
};

bool Infantry::PostUpdate()
{
	if (current_health <= 0)
	{	
		CleanUp();
	}

	return true;
};

bool Infantry::CleanUp()
{	
	/*App->entityManager->entities;
	App->entityManager->dynamic_objects;
	App->entityManager->infantries;*/
	
	return true;
};

// Collision Handling ---------------------------------------
void Infantry::OnCollision(Collider* C1, Collider* C2)
{
	return;
}

void Infantry::ApplyDamage(Entity* objective)
{
	return;
}
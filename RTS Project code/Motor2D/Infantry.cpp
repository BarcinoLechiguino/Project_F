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
	entity_sprite = App->tex->Load("textures/Spritesheets/Entities/infantry_lowres.png");

	selectable_unit = true;

	speed = 500;

	max_health = 300;
	current_health = max_health;
	damage = 30;

	empty_rect = { 0,158,MAX_HEALTHBAR_WIDTH,18 };
	empty_bar = (UI_Image*)App->gui->CreateImage(UI_Element::IMAGE, int(pixel_position.x), int(pixel_position.y) - 30, empty_rect, true, false, false,nullptr,this, NULL); //Magic Number

	health_rect = { 0,198,MAX_HEALTHBAR_WIDTH,18 };
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


	//change section according to pathing. 
	switch (this->unit_state) {
	case entity_state::PATHING_DOWN:
		entity_sprite_section = { 71,47,70,52 };
		break;
	case entity_state::PATHING_RIGHT:
		entity_sprite_section = { 202,47,59,52 };
		break;
	case entity_state::PATHING_LEFT:
		entity_sprite_section = { 142,47,59,52 };
		break;
	case entity_state::PATHING_UP:
		entity_sprite_section = { 0,47,70,52 };
		break;
	case entity_state::PATHING_DOWN_RIGHT:
		entity_sprite_section = { 58,0,58,47 };
		break;
	case entity_state::PATHING_DOWN_LEFT:
		entity_sprite_section = { 0,0,58,47 };
		break;
	case entity_state::PATHING_UP_RIGHT:
		entity_sprite_section = { 116,0,60,47 };
		break;
	case entity_state::PATHING_UP_LEFT:
		entity_sprite_section = { 176,0,59,47 };
		break;
	}

	App->render->Blit(this->entity_sprite, pixel_position.x, pixel_position.y-15, &entity_sprite_section);

	App->render->DrawQuad(selection_collider, 255, 255, 0, 100);

	if (current_health < 300)
	{
		LOG("Infantry health: %f", current_health );
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

void Infantry::ApplyDamage(Entity* objective)
{
	return;
}
#include "Definitions.h"
#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Map.h"
#include "Player.h"
#include "EntityManager.h"
#include "GuiHealthbar.h"
#include "SceneManager.h"
#include "ParticleManager.h"
#include "Emitter.h"

#include "Entity.h"


Entity::Entity() : tile_position(0, 0), type(ENTITY_TYPE::UNKNOWN), level(1), entity_sprite(nullptr), animation(nullptr), red_state(false), red_state_timer(0)
{

}

Entity::Entity(int x, int y, ENTITY_TYPE type, int level) : tile_position(x, y), type(type), level(level), entity_sprite(nullptr), animation(nullptr), red_state(false), red_state_timer(0)	//Initializes the variables to the specified values when the constructor is called.
{
	return;
}

//j1Entity::~j1Entity()
//{
//	App->tex->UnLoad(entity_sprite);
//	entity_sprite = nullptr;
//
//}

bool Entity::Awake(pugi::xml_node& config)
{
	return true;
}

bool Entity::Start()
{
	return true;
}

bool Entity::PreUpdate()
{
	return true;
}

bool Entity::Update(float dt, bool do_logic)
{
	return true;
}

bool Entity::PostUpdate()
{
	return true;
}

bool Entity::CleanUp()
{
	return true;
}

//-------------------- LOAD AND SAVE --------------------
/*bool Entity::Load(pugi::xml_node&)
{
	return true;
}

bool Entity::Save(pugi::xml_node&) const
{
	return true;
}

//------------------- ENTITY METHODS -------------------
void Entity::Restart()
{
	return;
}*/

void Entity::InitEntity()
{
	return;
}

void Entity::AttachHealthbarToEntity()
{
	return;
}

void Entity::Draw()
{
	return;
}

void Entity::ApplyDamage(Entity* target)
{
	if (target != nullptr)
	{
		
		if(App->player->god_mode)
		{
			if (App->entity_manager->IsAllyEntity(target))
			{
				return;
			}
		}

		target->current_health -= attack_damage;
		target->healthbar->UpdateHealthbarValue();
		if (App->entity_manager->IsUnit(target))
		{
			Emitter* damage_emitter = App->particle_manager->SpawnEmitter({ target->pixel_position.x, target->pixel_position.y }, EMITTER_TYPE::EMITTER_DAMAGE);
		}

		App->render->DrawLine(this->center_point.x, this->center_point.y, target->center_point.x, target->center_point.y, 255, 0, 0, 255); //debug
	}
}

void Entity::RecieveDamage(int damage)
{
	if (App->player->god_mode)
	{
		if (App->entity_manager->IsAllyEntity(this))
		{
			return;
		}
	}
	current_health -= damage;
	healthbar->UpdateHealthbarValue();

	if (App->entity_manager->IsUnit(this))
	{
		Emitter* damage_emitter = App->particle_manager->SpawnEmitter({ pixel_position.x, pixel_position.y }, EMITTER_TYPE::EMITTER_DAMAGE);
	}
	red_state = true;
}

void Entity::UpdateRedState() {
	if (red_state) 
	{
		red_state_timer++;
		if (red_state_timer > 5) 
		{
			red_state = false;
			red_state_timer = 0;
		}
	}
	
	return;
}
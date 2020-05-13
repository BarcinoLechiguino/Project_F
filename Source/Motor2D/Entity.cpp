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
#include "UI_Healthbar.h"
#include "SceneManager.h"

#include "Entity.h"


Entity::Entity() : tile_position(0, 0), type(ENTITY_TYPE::UNKNOWN), level(1), entity_sprite(nullptr), collider(nullptr), animation(nullptr)
{

}

Entity::Entity(int x, int y, ENTITY_TYPE type, int level) : tile_position(x, y), type(type), level(level), entity_sprite(nullptr), collider(nullptr), animation(nullptr)	//Initializes the variables to the specified values when the constructor is called.
{
	return;
}

//j1Entity::~j1Entity()
//{
//	App->tex->UnLoad(entity_sprite);
//	entity_sprite = nullptr;
//	if (collider != nullptr)
//	{
//		collider->delete_collider = true;
//	}
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
		if (!App->player->god_mode)
		{
			target->current_health -= attack_damage;

			target->healthbar->UpdateHealthbarValue();
		}
		else
		{
			if (!App->entity_manager->IsAllyEntity(target))
			{
				target->current_health -= attack_damage;

				target->healthbar->UpdateHealthbarValue();
			}
		}
	}
}

void Entity::OnCollision(Collider* C1, Collider* C2)
{
	return;
}
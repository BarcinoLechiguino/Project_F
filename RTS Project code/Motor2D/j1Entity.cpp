#include "j1EntityManager.h"
#include "j1Entity.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Input.h"
#include "j1FadeScene.h"
#include "j1Scene.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"

j1Entity::j1Entity(int x, int y, ENTITY_TYPE type) : position(x, y), type(type), entity_sprite(nullptr), collider(nullptr), animation(nullptr)	//Initializes the variables to the specified values when the constructor is called.
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

bool j1Entity::Awake(pugi::xml_node& config)
{
	return true;
}

bool j1Entity::Start()
{
	return true;
}

bool j1Entity::PreUpdate()
{
	return true;
}

bool j1Entity::Update(float dt, bool doLogic)
{
	return true;
}

bool j1Entity::PostUpdate()
{
	return true;
}

bool j1Entity::CleanUp()
{
	return true;
}

//-------------------- LOAD AND SAVE --------------------
bool j1Entity::Load(pugi::xml_node&)
{
	return true;
}

bool j1Entity::Save(pugi::xml_node&) const
{
	return true;
}

//------------------- ENTITY METHODS -------------------
void j1Entity::Restart()
{
	return;
}

void j1Entity::LoadAnimationPushbacks()
{
	return;
}

void j1Entity::LoadEntityProperties()
{
	return;
}

void j1Entity::LoadEntityAudio()
{
	return;
}

void j1Entity::BlitEntity(int x, int y, SDL_Rect entity_rect, bool flip)
{
	App->render->Blit(entity_sprite, x, y, &entity_rect, flip);

	//App->render->Blit(entity_sprite, x, y, &entity_rect, flip, SDL_FLIP_HORIZONTAL);	//Check out how to use the flip already integrated in the Blit.
}

void j1Entity::OnCollision(Collider* C1, Collider* C2)
{
	return;
}


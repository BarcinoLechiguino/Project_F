#include "ParticleManager.h"
#include "Log.h"
#include "Application.h"

#include "Render.h"
#include "Emitter.h"



ParticleManager::ParticleManager() : Module()
{
	name = "particle_manager";
}

ParticleManager::~ParticleManager()
{
}

bool ParticleManager::Start()
{


	return true;
}

bool ParticleManager::Awake(pugi::xml_node& config)
{
	bool ret = true;

	pugi::xml_document config_ps;
	pugi::xml_node* node = &App->LoadParticleSystemConfig(config_ps);
	pugi::xml_node info = node->child("info").child("emitter");

	for (; info != nullptr; info = info.next_sibling())
	{
			LoadData(info);
	}

	return ret;
}

bool ParticleManager::Update(float dt)
{
	for (std::list<Emitter*>::iterator it = emittersList.begin(); it != emittersList.end(); ++it)
	{
		if ((*it) != nullptr) {
			(*it)->Update(dt);
		}
	}

	return true;
}

bool ParticleManager::PostUpdate()
{
	for (std::list<Emitter*>::iterator it = emittersList.begin(); it != emittersList.end(); ++it)
	{
		if ((*it)->remove)
		{
			delete (*it);
			emittersList.erase(it);
		}
	}

	return true;
}

bool ParticleManager::CleanUp()
{

	for (std::list<Emitter*>::iterator it = emittersList.begin(); it != emittersList.end(); ++it)
	{
		if ((*it) != nullptr)
			delete (*it);
	}

	emittersList.clear();

	return true;
}

Emitter* ParticleManager::SpawnEmitter(fPoint pos, EMITTER_TYPE type)
{
	Emitter* ret = nullptr;

	ret = new Emitter(pos, vecData[(int)type].speed, vecData[(int)type].size, vecData[(int)type].angle, vecData[(int)type].rnd, vecData[(int)type].emission,
			vecData[(int)type].particleLife, vecData[(int)type].emitterLife, vecData[(int)type].startColor, vecData[(int)type].endColor, vecData[(int)type].r, vecData[(int)type].path.c_str());

	
	emittersList.push_back(ret);
	
	return ret;
}

void ParticleManager::LoadData(pugi::xml_node& node)
{
	DataToLoad data;

	int typeNum = node.attribute("type").as_int();

	data.speed = node.child("speed").attribute("value").as_float();
	data.size = node.child("size").attribute("value").as_float();
	data.angle.x = node.child("angle").attribute("min").as_float();
	data.angle.y = node.child("angle").attribute("max").as_float();
	data.emission = node.child("emission").attribute("value").as_int();
	data.rnd = node.child("rnd").attribute("value").as_int();
	data.particleLife = node.child("particleLife").attribute("value").as_int();
	data.emitterLife = node.child("emitterLife").attribute("value").as_float();

	data.startColor.r = node.child("startColor").attribute("r").as_uint();
	data.startColor.g = node.child("startColor").attribute("g").as_uint();
	data.startColor.b = node.child("startColor").attribute("b").as_uint();
	data.startColor.a = node.child("startColor").attribute("a").as_uint();

	data.endColor.r = node.child("endColor").attribute("r").as_uint();
	data.endColor.g = node.child("endColor").attribute("g").as_uint();
	data.endColor.b = node.child("endColor").attribute("b").as_uint();
	data.endColor.a = node.child("endColor").attribute("a").as_uint();

	data.r.x = node.child("rect").attribute("x").as_int();
	data.r.y = node.child("rect").attribute("y").as_int();
	data.r.w = node.child("rect").attribute("w").as_int();
	data.r.h = node.child("rect").attribute("h").as_int();

	vecData[(int)type] = data;

	data.path = node.child("texture_path").attribute("path").as_string();
	//Expand Here
	//particle_tex = App->tex->Load("textures/particle_small.png");
}



bool ParticleManager::RemoveEverything()
{
	bool ret = false;

	for (std::list<Emitter*>::const_iterator it = emittersList.begin(); it != emittersList.end(); ++it)
	{
		if ((*it) != nullptr) {
			(*it)->remove = true;
			ret = true;
		}
	}

	return ret;
}

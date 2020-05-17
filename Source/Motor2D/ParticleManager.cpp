#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Emitter.h"

#include "ParticleManager.h"

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
	pugi::xml_node node = App->LoadParticleSystemConfig(config_ps);
	pugi::xml_node info = node.child("info").child("emitter");

	
	
	LoadData();

	return ret;
}

bool ParticleManager::Update(float dt)
{
	 for (std::vector<Emitter*>::iterator it = emittersList.begin(); it != emittersList.end(); ++it)
	 {
		 if ((*it) != nullptr) {
			 (*it)->Update(dt);
		 }
	 }

	return true;
}

bool ParticleManager::PostUpdate()
{
/*
	for (std::vector<Emitter*>::iterator it = emittersList.begin(); it != emittersList.end(); ++it)
	{
		if ((*it) != nullptr) {
			(*it)->PostUpdate();
		}
	}

	for (std::vector<Emitter*>::iterator it = emittersList.begin(); it != emittersList.end(); ++it)
	{
		if ((*it)->remove)
		{
			emittersList.erase(it);
		}
	}
*/	
	for (int i = 0; i < (int)emittersList.size(); ++i)
	{
		if (emittersList[i] != nullptr) {
			emittersList[i]->PostUpdate();
		}
		if (emittersList[i]->IsEmitterToDelete())
		{
			RELEASE(emittersList[i]);
		}
	}
	return true;
}

bool ParticleManager::CleanUp()
{
	for (std::vector<Emitter*>::iterator it = emittersList.begin(); it != emittersList.end(); ++it)
	{
		if ((*it) != nullptr)
			RELEASE(*it);
	}
	emittersList.clear();
	return true;
}

Emitter* ParticleManager::SpawnEmitter(fPoint pos, EMITTER_TYPE type)
{
	Emitter* ret = nullptr;

	ret = new Emitter(pos, vecData[(int)type].speed, vecData[(int)type].size, vecData[(int)type].coneAngle, vecData[(int)type].rnd, vecData[(int)type].emission,
			vecData[(int)type].particleLife, vecData[(int)type].emitterLife, vecData[(int)type].startColor, vecData[(int)type].endColor, vecData[(int)type].r, vecData[(int)type].path.c_str(), vecData[(int)type].cameraspeed, vecData[(int)type].spreadDirection, vecData[(int)type].layer);
	
	emittersList.push_back(ret);
	
	return ret;
}

void ParticleManager::LoadData()
{
	DataToLoad data;

	pugi::xml_document config_ps;

	config_ps.load_file("ps_config.xml");

	pugi::xml_node node = config_ps.child("particlesystem").child("info").child("emitter");

	for(; node != nullptr; node = node.next_sibling())
	{
		int typeNum				= node.attribute("type").as_int();										// Heh~

		data.speed				= node.child("speed").attribute("value").as_float();
		data.size				= node.child("size").attribute("value").as_float();
		data.coneAngle.x		= node.child("cone_angle").attribute("min").as_float();
		data.coneAngle.y		= node.child("cone_angle").attribute("max").as_float();
		data.emission			= node.child("emission").attribute("value").as_float();
		data.rnd				= node.child("rnd").attribute("value").as_int();
		data.particleLife		= node.child("particleLife").attribute("value").as_int();
		data.emitterLife		= node.child("emitterLife").attribute("value").as_float();

		data.startColor.r		= node.child("startColor").attribute("r").as_uint();
		data.startColor.g		= node.child("startColor").attribute("g").as_uint();
		data.startColor.b		= node.child("startColor").attribute("b").as_uint();
		data.startColor.a		= node.child("startColor").attribute("a").as_uint();

		data.endColor.r			= node.child("endColor").attribute("r").as_uint();
		data.endColor.g			= node.child("endColor").attribute("g").as_uint();
		data.endColor.b			= node.child("endColor").attribute("b").as_uint();
		data.endColor.a			= node.child("endColor").attribute("a").as_uint();

		data.r.x				= node.child("rect").attribute("x").as_int();
		data.r.y				= node.child("rect").attribute("y").as_int();
		data.r.w				= node.child("rect").attribute("w").as_int();
		data.r.h				= node.child("rect").attribute("h").as_int();

		data.cameraspeed		= node.child("camera_speed").attribute("speed").as_float();
		data.spreadDirection	= node.child("spread_direction").attribute("a").as_float();
		data.layer				= node.child("layer").attribute("value").as_uint();

		data.path				= node.child("texture_path").attribute("path").as_string();

		vecData[typeNum] = data;
	}
}

bool ParticleManager::RemoveEverything()
{
	bool ret = false;

	for (std::vector<Emitter*>::const_iterator it = emittersList.begin(); it != emittersList.end(); ++it)
	{
		if ((*it) != nullptr) {
			(*it)->remove = true;
			ret = true;
		}
	}
	return ret;
}

void ParticleManager::DeleteEmitter(Emitter* todelete) {

	std::vector<Emitter*>::iterator item = emittersList.begin();

	for (; item != emittersList.end(); ++item)
	{
		if ((*item) == todelete)
		{
			RELEASE(*item);

			emittersList.erase(item);

			emittersList.resize(emittersList.size());

			break;
		}
	}
}
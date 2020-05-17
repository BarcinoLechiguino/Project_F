#ifndef __PARTICLE_MANAGER_H__
#define __PARTICLE_MANAGER_H__

#include "Application.h"
#include "Point.h"
#include "Module.h"
//#include "Emitter.h"
//#include "j1Pool.h"

#include <list>
#include <string>

#include "Dependencies/SDL/include/SDL_rect.h"

class Emitter;
enum EMITTER_TYPE;

#define NUM_EMITTERS 20


class ParticleManager : public Module
{
private:

	struct DataToLoad
	{
		EMITTER_TYPE type;
		SDL_Rect r;

		float speed;
		float emission;
		fPoint coneAngle;
		float spreadDirection;
		int particleLife;
		double emitterLife;
		float size;
		int rnd;
		float cameraspeed;
		uint layer;

		SDL_Color startColor;
		SDL_Color endColor;

		std::string path;
	};

public:

	ParticleManager();
	~ParticleManager();

	bool Start();
	bool Awake(pugi::xml_node& config_node);
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();


	Emitter* SpawnEmitter(fPoint pos, EMITTER_TYPE type);
	void LoadData();
	bool RemoveEverything();
	void DeleteEmitter(Emitter* todelete);


	EMITTER_TYPE type;
	std::vector<Emitter*> emittersList;
	
	//Loads Emitter characteristics at Awake, to avoid reopening xml file
	DataToLoad vecData[NUM_EMITTERS];
};

#endif // !__PARTICLE_MANAGER_H__
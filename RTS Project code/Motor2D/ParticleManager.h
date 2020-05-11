//#ifndef __J1PARTICLEMANAGER_H__
//#define __J1PARTICLEMANAGER_H__

#include "Application.h"
#include "Point.h"
#include "Module.h"
//#include "Emitter.h"
//#include "j1Pool.h"

#include <list>
#include <string>

#include "SDL/include/SDL_rect.h"

class Emitter;
enum class EMITTER_TYPE;

#define NUM_EMITTERS 20


class ParticleManager : public Module
{
private:

	struct DataToLoad
	{
		EMITTER_TYPE type;
		SDL_Rect r;

		float speed;
		int emission;
		fPoint angle;
		int particleLife;
		double emitterLife;
		float size;
		int rnd;

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
	void LoadData(pugi::xml_node& node, EMITTER_TYPE type);
	bool RemoveEverything();


	EMITTER_TYPE type;
	std::list<Emitter*> emittersList;
	
	//Loads Emitter characteristics at Awake, to avoid reopening xml file
	DataToLoad vecData[NUM_EMITTERS];

};

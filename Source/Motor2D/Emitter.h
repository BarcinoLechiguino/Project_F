#ifndef __j1EMITTER_H__
#define __j1EMITTER_H__

#include "Point.h"

//#include "j1Pool.h"
#include "PerfTimer.h"
#include "SDL/include/SDL_rect.h"

#include <vector>

struct SDL_Texture;
class Pool;

enum EMITTER_TYPE
{
	EMITTER_NONE,
	EMITTER_BACKGROUND,
	EMITTER_EXPLOSION
};

struct Particle
{
	fPoint pos;
	fPoint speed;
	float size;
	float startSize;
	int life;
	int startLife;
	SDL_Rect rect;
	SDL_Rect rectSize;
	SDL_Color startColor;
	SDL_Color endColor;
	float transparency;
	float fraction;

};

class Emitter
{
private:

	EMITTER_TYPE type = EMITTER_NONE;

	fPoint pos;
	fPoint angle;
	int spreadDirection;
	float speed;
	float emitterSize;
	int emission;
	int rnd;
	int particleLife;
	SDL_Rect rect;
	double emitterLife;
	float cameraspeed;
	uint layer; 

	SDL_Color startColor;
	SDL_Color endColor;

	SDL_Texture* particle_tex = nullptr;

public:

	Emitter(fPoint pos, float maxSpeed, float maxSize, fPoint angleRange, int emitVariance, int emitNumber, int maxParticleLife, double emitterLife, SDL_Color startColor, SDL_Color endColor, SDL_Rect textureRect, const char* path, float cameraspeed, int spreadDirection, uint layer);
	~Emitter();

	bool Update(float dt);
	bool PostUpdate();
	float RandomizeParticles(float min, float max);
	int GetSize() const { return size; }
	void UpdatePos(fPoint newPos) { pos = newPos; }

	Pool* container = nullptr;
	int size = 5000;
	bool remove = false;

	float randomSpeed;
	float randomAngle;
	float randomSize;

	//POOL related
	void CreateParticles(fPoint pos, float speed, float angle, float size, int life, SDL_Rect tex, SDL_Color startColor, SDL_Color endColor);

	std::vector<Particle*> particle_vec; 

	int poolsize = 0;

	//PARTICLE related
	SDL_Color InterpolateColors(SDL_Color color1, SDL_Color color2, float fraction);
	float RandomizeParticleColor(float min, float max);

	//void Emitter::LoadParticleProperties(fPoint pos, float speed, float angle, float size, int life, SDL_Rect tex, SDL_Color startColor, SDL_Color endColor);
};

#endif

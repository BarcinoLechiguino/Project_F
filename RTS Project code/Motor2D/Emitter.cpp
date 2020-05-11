#include "Emitter.h"
#include "Render.h"
//#include "j1Pool.h"
//#include "j1Particle.h"
#include "ParticleManager.h"
#include "Textures.h"

#include <time.h>
#include <assert.h>

#define PI 3.14159265359f 

Emitter::Emitter(fPoint pos, float speed, float size, fPoint angle, int rnd, int emission, int particleLife, double emitterLife, SDL_Color startColor, SDL_Color endColor, SDL_Rect rectangle, const char* path)
{
	srand(time(NULL));

	this->pos = pos;
	this->speed = speed;
	this->emitterSize = size;
	this->angle = angle;
	this->rnd = rnd;
	this->emission = emission;
	this->particleLife = particleLife;
	this->emitterLife = emitterLife;
	this->startColor = startColor;
	this->endColor = endColor;
	this->rect = rectangle;
	this->particle_tex = App->tex->Load(path); 

}

Emitter::~Emitter()
{
	App->tex->UnLoad(particle_tex);

	delete container;
}

bool Emitter::Update(float dt)
{
	bool ret = true;

	for (int i = 0; i < size; i++)
	{
		if (vec[i].life > 0)
		{
			//vec[i].Update(dt);
			vec[i].life--;

			vec[i].transparency = (float)vec[i].life / (float)vec[i].startLife;
			float alpha = (vec[i].transparency * 255.0f);
			vec[i].startColor.a = alpha;
			vec[i].size = vec[i].startSize * vec[i].transparency;
			vec[i].rectSize.w = vec[i].rectSize.h = vec[i].size;

			vec[i].pos.x += vec[i].speed.x * dt;
			vec[i].pos.y += vec[i].speed.y * dt;

			//vec[i].Draw();
			vec[i].fraction += (RandomizeParticleColor(0.0f, 15.0f) / vec[i].startLife);

			SDL_Rect drawRect = { (int)vec[i].startSize, (int)vec[i].startSize };
			App->render->ColoredBlit(this->particle_tex, vec[i].pos.x + ((drawRect.w - vec[i].rectSize.w) / 2), vec[i].pos.y + ((drawRect.h - vec[i].rectSize.h) / 2), &vec[i].rect, &vec[i].rectSize, InterpolateColors(vec[i].startColor, vec[i].endColor, vec[i].fraction));


		}
		else if (vec[i].life <= 0)
		{
			startParticle = &vec[i];

		}
		else {
			return false;
		}
	}

	int emissionRate = (int)(emission + rnd);

	for (int i = 1; i <= emissionRate; i++)
	{
		randomSpeed = speed * RandomizeParticles(0.0f, 2.0f);
		randomAngle = RandomizeParticles(angle.x, angle.y);
		randomSize = emitterSize * RandomizeParticles(0.0f, 1.0f);
		CreateParticles(pos, randomSpeed, randomAngle, randomSize, particleLife, rect, startColor, endColor);
	}

	return ret;
}

float Emitter::RandomizeParticles(float min, float max)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	return (max - min + 0.25) * random + min;
}

void Emitter::CreateParticles(fPoint pos, float speed, float angle, float size, int life, SDL_Rect tex, SDL_Color startColor, SDL_Color endColor)
{
	assert(startParticle != nullptr);

	//LoadParticleProperties(pos, speed, angle, size, life, tex, startColor, endColor);
	Particle* particle_tocreate = startParticle;

	particle_tocreate->pos = pos;
	particle_tocreate->speed.x = speed * cos(angle * PI / (180));
	particle_tocreate->speed.y = -speed * sin(angle * PI / (180));
	particle_tocreate->life = particle_tocreate->startLife = life;
	particle_tocreate->size = particle_tocreate->startSize = size;
	particle_tocreate->rect = particle_tocreate->rectSize = tex;
	particle_tocreate->startColor = startColor;
	particle_tocreate->endColor = endColor;

	particle_tocreate->fraction = 0.0f;
}

SDL_Color Emitter::InterpolateColors(SDL_Color color1, SDL_Color color2, float fraction)
{

	SDL_Color result;

	result.r = (color2.r - color1.r) * fraction + color1.r;
	result.g = (color2.g - color1.g) * fraction + color1.g;
	result.b = (color2.b - color1.b) * fraction + color1.b;

	return result;
}

float Emitter::RandomizeParticleColor(float min, float max)
{
	float random = ((float)rand()) / (float)RAND_MAX;

	return (max - min + 0.25) * random + min;
}

/*

void Emitter::LoadParticleProperties(fPoint pos, float speed, float angle, float size, int life, SDL_Rect tex, SDL_Color startColor, SDL_Color endColor)
{




}
*/

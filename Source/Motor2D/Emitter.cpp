#include "Emitter.h"
#include "Render.h"
#include "ParticleManager.h"
#include "Textures.h"
#include "Log.h"

#include <time.h>
#include <assert.h>
#include <math.h>

#define PI 3.14159265359f 

Emitter::Emitter(fPoint pos, float speed, float size, fPoint angle, int rnd, float emission, int particleLife, double emitterLife, SDL_Color startColor, SDL_Color endColor, SDL_Rect rectangle, const char* path, float cameraspeed, int spreadDirection, uint layer)
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
	this->cameraspeed = cameraspeed; 
	this->spreadDirection = spreadDirection;
	this->layer = layer;

	//INIT VALUES UNUSED
	randomAngle = -1.0f;
	randomSize = -1.0f;
	randomSpeed = -1.0f;


	emissionRate = emission;
}

Emitter::~Emitter()
{
	App->tex->UnLoad(particle_tex);

	delete container;
}

bool Emitter::Update(float dt)
{
	bool ret = true;


	if (emissionRate > 1) {
		for (int i = 1; i <= emissionRate; i++)
		{
			randomSpeed = speed * RandomizeParticles(0.0f, 2.0f);
			randomAngle = RandomizeParticles(angle.x, angle.y);
			randomSize = emitterSize * RandomizeParticles(0.5f, 1.25f);
			randomRotation = RandomizeParticles(0.0f, 360.0f);
			CreateParticles(pos, randomSpeed, randomAngle, randomSize, particleLife, randomRotation, rect, startColor, endColor);
		}
		if (emission < 1) {
			emissionRate = 0;
		}
	}
	else if (emissionRate < 0) {
		LOG("Something broke at EmissionRate");
		return false;
	}
	else{
		emissionRate += emission;
	}


	for (int i = 0; i < (int)particle_vec.size(); ++i)
	{
		if (particle_vec[i]->life > 0)
		{
			//vec[i].Update(dt);
			particle_vec[i]->life--;

			particle_vec[i]->transparency = (float)particle_vec[i]->life / (float)particle_vec[i]->startLife;
			float alpha = (particle_vec[i]->transparency * 255.0f);
			particle_vec[i]->startColor.a = alpha;
			particle_vec[i]->size = particle_vec[i]->startSize * particle_vec[i]->transparency;
			particle_vec[i]->rectSize.w = particle_vec[i]->rectSize.h = particle_vec[i]->size;

			//Update particle position
			particle_vec[i]->pos.x += particle_vec[i]->speed.x * dt; //* cos(spreadDirection * (PI / 180.0f));
			particle_vec[i]->pos.y += particle_vec[i]->speed.y * dt; //* sin(spreadDirection * (PI / 180.0f));

			//vec[i].Draw();
			if (layer == 0) {
				particle_vec[i]->fraction += (RandomizeParticleColor(0.0f, 3.0f) / particle_vec[i]->startLife);

				SDL_Rect drawRect = { (int)particle_vec[i]->startSize, (int)particle_vec[i]->startSize };
				App->render->ColoredBlit(this->particle_tex, particle_vec[i]->pos.x + ((drawRect.w - particle_vec[i]->rectSize.w) / 2), particle_vec[i]->pos.y + ((drawRect.h - particle_vec[i]->rectSize.h) / 2), /*&particle_vec[i]->rect*/ NULL, &particle_vec[i]->rectSize, InterpolateColors(particle_vec[i]->startColor, particle_vec[i]->endColor, particle_vec[i]->fraction), cameraspeed, particle_vec[i]->rotation);
			}
		}
		else if (particle_vec[i]->life <= 0)
		{
			//DELETE PARTICLE;
			particle_vec.erase(particle_vec.begin() + i);
		}
	}

	return ret;
}

bool Emitter::PostUpdate() {
	
	for (int i = 0; i < (int)particle_vec.size(); ++i)
	{
		if (layer == 1) {
			particle_vec[i]->fraction += (RandomizeParticleColor(0.0f, 1.5f) / particle_vec[i]->startLife);

			SDL_Rect drawRect = { (int)particle_vec[i]->startSize, (int)particle_vec[i]->startSize };
			App->render->ColoredBlit(this->particle_tex, particle_vec[i]->pos.x + ((drawRect.w - particle_vec[i]->rectSize.w) / 2), particle_vec[i]->pos.y + ((drawRect.h - particle_vec[i]->rectSize.h) / 2), /*&particle_vec[i]->rect*/ NULL, &particle_vec[i]->rectSize, InterpolateColors(particle_vec[i]->startColor, particle_vec[i]->endColor, particle_vec[i]->fraction), cameraspeed);

		}
	}
	return true;
}


float Emitter::RandomizeParticles(float min, float max)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	return (max - min) * random + min;
}

void Emitter::CreateParticles(fPoint pos, float speed, float angle, float size, int life, float rotation,SDL_Rect tex, SDL_Color startColor, SDL_Color endColor)
{
	Particle* particle_tocreate = new Particle;

	float random = ((float)rand()) / (float)RAND_MAX;

	particle_tocreate->pos.x = (this->rect.w) * random + pos.x;

	random = ((float)rand()) / (float)RAND_MAX;
	particle_tocreate->pos.y = (this->rect.h) * random + pos.y;

	float test = sin(angle * PI / 180.0f) + sin(spreadDirection * PI / 180.0f);

	particle_tocreate->speed.x = speed * (cos(angle * PI / 180.0f));
	particle_tocreate->speed.y = -speed * (sin(angle * PI /180.0f));
	particle_tocreate->life = particle_tocreate->startLife = life;
	particle_tocreate->size = particle_tocreate->startSize = size;
	particle_tocreate->rect = particle_tocreate->rectSize = tex;
	particle_tocreate->startColor = startColor;
	particle_tocreate->endColor = endColor;

	particle_tocreate->rotation = rotation;

	particle_tocreate->fraction = 0.0f;

	particle_vec.push_back(particle_tocreate);
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

	return (max - min) * random + min;
}



#include "Particle.h"
#include "Game.h"
#include "TimeManager.h"

Particle::Particle(ParticleSystem* sys, QVector3 pos, int tex)
{
	system = sys;
	Position = pos;
	TexIndex = tex;
	//Scale = 0.05f;
	Scale = 0.01f;

	BirthTime = system->engine->tman->GetTime();
	LifeTime = 0;
}

bool Particle::CheckDecay()
{
	//check if particle decay time is exceeded. if so, return true
	if (LifeTime > system->GetDecayTime()) return true;

	//otherwise
	return false;
}

void Particle::Update()
{
	//move and scale the particle
	Position += system->GetVelocity();
	Scale += system->GetScaleFactor() * system->engine->tman->GetDeltaTime();		//ewentualnie *=

	//update time
	LifeTime = system->engine->tman->GetTime() - BirthTime;
}
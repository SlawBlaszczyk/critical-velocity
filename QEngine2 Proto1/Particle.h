#ifndef PARTICLE_H
#define PARTICLE_H

#include "ParticleSystem.h"

class Particle
{
	ParticleSystem* system;
	QVector3 Position;
	float Scale;
	float LifeTime;
	float BirthTime;

	int TexIndex;

public:
	Particle(ParticleSystem* sys, QVector3 pos, int tex);
	void Update();

	//returns true if the particle has decayed
	bool CheckDecay();

	QVector3 GetPosition() {return Position;}
	int GetTexIndex() {return TexIndex;}
	float GetScale() {return Scale;}
};

#endif
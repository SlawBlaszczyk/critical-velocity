#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#define PARTICLESYS_MAX_TEX 32

#include "QVector3.h"
#include <Windows.h>
#include <list>

using namespace std;

class Particle;
class Game;

class ParticleSystem
{
	int TextureIndices [PARTICLESYS_MAX_TEX];
	int nTextures;
	int MainTexIndex;
	QVector3 Position;
	float DecayTime;	//time after which particle disappears
	float ScaleFactor;
	QVector3 Velocity;	//initial velocity vector of particles

	//particle STL list
	list <Particle*> particles;

public:
	Game* engine;

	ParticleSystem(Game* eng, LPCTSTR TexFileName, QVector3 pos, float decay);
	int AddTexture(LPCTSTR TexFileName);
	void Update();	//updates all particles in system
	void Emit();	//emits a new particle
	void Draw();
	float GetDecayTime() {return DecayTime;}
	float GetScaleFactor() {return ScaleFactor;}
	QVector3 GetVelocity() {return Velocity;}
	void SetPosition(QVector3 pos) {Position = pos;}
	~ParticleSystem();

};

#endif
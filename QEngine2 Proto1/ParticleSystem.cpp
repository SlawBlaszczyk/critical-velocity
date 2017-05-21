#include "ParticleSystem.h"
#include "Particle.h"
#include "IQGraphics.h"
#include "Game.h"

ParticleSystem::ParticleSystem(Game* eng, LPCTSTR TexFileName, QVector3 pos, float decay)
{
	engine = eng;
	nTextures = 0;
	MainTexIndex = AddTexture(TexFileName);
	Position = pos;
	DecayTime = decay;
	ScaleFactor = 0.04f;//1.1f;
	//Velocity = QVector3(0, 0.03f, 0);
	Velocity = QVector3(0, 0.0f, 0);
}

int ParticleSystem::AddTexture(LPCTSTR TexFileName)
{
	TextureIndices [nTextures] = engine->graphics->AllocateTexture(TexFileName);
	nTextures++;
	return TextureIndices [nTextures-1];
}

void ParticleSystem::Emit()
{
	Particle* p = new Particle(this, Position, MainTexIndex);

	//add a particle to the queue
	particles.push_back(p);
}

void ParticleSystem::Update()
{
	//update each particle
	if (!particles.empty())
		for(list<Particle*>::iterator it = particles.begin(); it != particles.end(); it++)
		{
			(*it)->Update();
		}

	//check particles for decay
	if (!particles.empty())
		while(particles.front()->CheckDecay())
		{
			delete particles.front();
			particles.pop_front();
			if(particles.empty())
				break;
		}

}

void ParticleSystem::Draw()
{
	//render all particles
	D3DXVECTOR3 center (64.0f, 64.0f, 0.0f);	//half of a texture dimensions
	D3DXVECTOR3 pos;
	D3DXVECTOR3 scale;

	if (!particles.empty())
		for(list<Particle*>::iterator it = particles.begin(); it != particles.end(); it++)
		{
			pos.x = (*it)->GetPosition().x;
			pos.y = (*it)->GetPosition().y;
			pos.z = (*it)->GetPosition().z;
			scale = D3DXVECTOR3 ((*it)->GetScale(), (*it)->GetScale(), (*it)->GetScale());
			engine->graphics->DrawSprite(center, pos, scale, (*it)->GetTexIndex());
		}
}

ParticleSystem::~ParticleSystem()
{
	//destroy all particles
	for(list<Particle*>::iterator it = particles.begin(); it != particles.end(); it++)
	{
		delete (*it);
	}
	particles.clear();
}
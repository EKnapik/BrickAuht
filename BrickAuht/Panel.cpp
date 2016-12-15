#include "Panel.h"

Panel::Panel()
{
}

ParticleComponent::ParticleComponent(GameObject * parentObject, Scene * parentScene)
{
	ParentObject = parentObject;
	ParentScene = parentScene;

	emitter = new ParticleEmitter("particle", "default");

	ParentScene->ParticleEmitters.push_back(emitter);
}

void ParticleComponent::Update(float dt)
{
	emitter->Position = ParentObject->entity->GetPosition();
}

ParticleComponent::~ParticleComponent()
{
	
}

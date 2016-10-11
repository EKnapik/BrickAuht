#include "GameObject.h"

KinematicComponent::KinematicComponent(GameEntity * enitity)
{
	this->entity = entity;
	enitity->AddReference();
}

KinematicComponent::KinematicComponent()
{
}

void KinematicComponent::Update(float dt)
{
	acceleration.x += velocity.x * dt;
	acceleration.y += velocity.y * dt;
	acceleration.z += velocity.z * dt;

	if (entity != nullptr)
	{
		oldPosition = entity->GetPosition();
		entity->SetPosition(VEC3(oldPosition.x + velocity.x * dt, oldPosition.y + velocity.y * dt, oldPosition.z + velocity.z * dt));
	}
	else 
	{
		oldPosition = position;
		position = VEC3(oldPosition.x + velocity.x * dt, oldPosition.y + velocity.y * dt, oldPosition.z + velocity.z * dt);
	}
}

KinematicComponent::~KinematicComponent()
{
	if (entity != nullptr)
	{
		entity->Release();
	}
}

#include "Ball.h"

Ball::Ball()
{
	kinematics = new KinematicComponent();
	components.push_back(kinematics);
}

void Ball::Update(float dt)
{
	float boxSize = 5;
	if (kinematics->GetPosition().x < -boxSize)
	{
		kinematics->velocity.x *= -1;
	}
	if (kinematics->GetPosition().x > boxSize)
	{
		kinematics->velocity.x *= -1;
	}

	if (kinematics->GetPosition().y > boxSize)
	{
		kinematics->velocity.y *= -1;
	}
	if (kinematics->GetPosition().y < -boxSize)
	{
		kinematics->velocity.y *= -1;
	}

	if (kinematics->GetPosition().z > boxSize)
	{
		kinematics->velocity.z *= -1;
	}
	if (kinematics->GetPosition().z < -boxSize)
	{
		kinematics->SetPosition(VEC3(kinematics->GetPosition().x, kinematics->GetPosition().y, -boxSize));
		kinematics->velocity.z *= -1;
	}
}

void Ball::SetEntity(GameEntity * entity)
{
	kinematics->entity = entity;

	if (this->entity != nullptr)
	{
		this->entity->Release();
	}
	this->entity = entity;
	entity->AddReference();
}

#pragma once
#include "GameObject.h"
#include "KinematicsComponent.h"

class Ball : public GameObject{
public:
	Ball();
	KinematicComponent* kinematics;
	virtual void SetEntity(GameEntity* entity);
	void Update(float dt);
};
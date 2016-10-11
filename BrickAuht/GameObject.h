#pragma once
#include "GameEntity.h"

class Component {
public:
	virtual void Update(float dt) = 0;
};

class KinematicComponent : Component {
	KinematicComponent(GameEntity* enitity);
	KinematicComponent();
	void Update(float dt);
	VEC3 GetPosition() { return entity != nullptr ? entity->GetPosition() : position; }
	void SetPosition(VEC3 pos) { entity != nullptr ? entity->SetPosition(pos) : position = pos; }
	VEC3 oldPosition;
	VEC3 velocity;
	VEC3 acceleration;
	virtual ~KinematicComponent();
private:
	VEC3 position;
	GameEntity* entity;
};

class RenderableComponent : Component {

};

class GameObject {
public:
	virtual void Update(float dt);
	std::vector<Component> components;
};
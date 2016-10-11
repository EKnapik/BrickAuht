#pragma once
#include "GameEntity.h"

class Component {
public:
	virtual void Update(float dt) = 0;
};

class GameObject {
public:
	virtual void Update(float dt) {};
	std::vector<Component*> components;
	GameEntity* entity = nullptr;
	virtual void SetEntity(GameEntity* newEntity);
	void AddComponent(Component* component);

	~GameObject();
};
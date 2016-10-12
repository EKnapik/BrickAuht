#pragma once
#include "GameObject.h"

class Scene {
public:
	virtual void Initialize() {};
	virtual void Update() {};
	virtual void End() {};
	std::vector<GameObject*> GameObjects;
	virtual ~Scene(){ for (int i = 0; i < GameObjects.size(); i++) { delete GameObjects.at(i); } };
};
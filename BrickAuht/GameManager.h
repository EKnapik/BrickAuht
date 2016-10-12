#pragma once
#include <stdio.h>
#include <vector>

#include "GameMath.h"
#include "GameObject.h"

class GameManager {
public:
	std::vector<GameObject*> GameObjects;

	void Update(float dt);

	void AddObject(GameObject* object) { GameObjects.push_back(object); }

	~GameManager();
};
#pragma once
#include <stdio.h>
#include <vector>

#include "GameMath.h"
#include "GameObject.h"
#include "Scene.h"

class GameManager {
public:
	std::vector<GameObject*> GameObjects;
	std::vector<GameEntity*> GameEntities;

	void Update(float dt);

	void SetActiveScene(Scene* nextScene);

	void AddObject(GameObject* object) { GameObjects.push_back(object); }

	~GameManager();

private:
	Scene* activeScene = nullptr;
};
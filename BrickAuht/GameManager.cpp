#include "GameManager.h"

void GameManager::Update(float dt)
{
	GameEntities.clear();
	for (int i = 0; i < GameObjects->size(); i++)
	{
		if (GameObjects->at(i)->ToDelete == true)
		{
			delete GameObjects->at(i);
			GameObjects->erase(GameObjects->begin() + i);
			i--;
		}
		GameObjects->at(i)->Update(dt);
		std::vector<Component*>* objectComponents = &GameObjects->at(i)->components;
		for (int p = 0; p < objectComponents->size(); p++)
		{
			objectComponents->at(p)->Update(dt);
		}
		if (GameObjects->at(i)->entity != nullptr)
		{
			GameEntities.push_back(GameObjects->at(i)->entity);
		}
	}
	if (activeScene)
	{
		activeScene->Update();
	}
}

void GameManager::SetActiveScene(Scene * nextScene)
{
	if (activeScene)
	{
		activeScene->End();
		delete activeScene;
	}

	activeScene = nextScene;
	activeScene->Initialize();
	GameObjects = &activeScene->GameObjects;
	GameEntities.clear();
}

GameManager::~GameManager()
{
	delete activeScene;
}

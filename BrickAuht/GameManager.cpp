#include "GameManager.h"

void GameManager::Update(float dt)
{
	for (int i = 0; i < GameObjects.size(); i++)
	{
		GameObjects.at(i)->Update(dt);
		std::vector<Component*>* objectComponents = &GameObjects.at(i)->components;
		for (int p = 0; p < objectComponents->size(); p++)
		{
			objectComponents->at(p)->Update(dt);
		}
	}
}

GameManager::~GameManager()
{
	for (int i = 0; i < GameObjects.size(); i++)
	{
		delete GameObjects.at(i);
	}
}

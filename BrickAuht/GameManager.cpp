#include "GameManager.h"

void GameManager::Update(float dt)
{
	for (int i = 0; i < GameObjects.size(); i++)
	{
		GameObjects.at(i).Update(dt);
		std::vector<Component>* objectComponents = &GameObjects.at(i).components;
		for (int p = 0; 0 < objectComponents->size(); p++)
		{
			objectComponents->at(p).Update(dt);
		}
	}
}

#include "GameManager.h"

void GameManager::Update(float dt)
{
	if (entitesSize != GameEntities.size() || entitesSize == -1 || gameObjectsSize != GameObjects->size() || activeScene->ObjectsDirty)
	{
		EntitiesDirty = true;
		activeScene->ObjectsDirty = false;
		GameEntities.clear();
	}
	for (int i = 0; i < GameObjects->size(); i++)
	{
		if (GameObjects->at(i)->ToDelete == true)
		{
			delete GameObjects->at(i);
			GameObjects->erase(GameObjects->begin() + i);
			i--;
			continue;
		}
		GameObjects->at(i)->Update(dt);
		std::vector<Component*>* objectComponents = &GameObjects->at(i)->components;
		for (int p = 0; p < objectComponents->size(); p++)
		{
			objectComponents->at(p)->Update(dt);
		}
		if (EntitiesDirty == true && GameObjects->at(i)->entity != nullptr)
		{
			GameEntities.push_back(GameObjects->at(i)->entity);
		}
	}
	if (activeScene)
	{
		activeScene->Update();
	}
	gameObjectsSize = GameObjects->size();
	entitesSize = GameEntities.size();
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
	EntitiesDirty			= true;
	DirectionalLightsDirty	= true;
	PointLightsDirty		= true;
	ParticleEmittersDirty	= true;
	entitesSize = -1;
	gameObjectsSize = -1;
}

GameManager::GameManager()
{
	EntitiesDirty			= true;
	DirectionalLightsDirty = true;
	PointLightsDirty = true;
	ParticleEmittersDirty = true;
	entitesSize = -1;
	gameObjectsSize = -1;
}

GameManager::~GameManager()
{
	delete activeScene;
}

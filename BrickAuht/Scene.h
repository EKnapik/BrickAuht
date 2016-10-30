#pragma once
#include "GameObject.h"
#include "Lights.h"

class Scene {
public:
	virtual void Initialize() {};
	virtual void Update() {};
	virtual void End() {};
	std::vector<GameObject*> GameObjects;
	std::vector<SceneDirectionalLight> DirectionalLights;
	std::vector<ScenePointLight> PointLights;
	std::string SkyBox = "skybox";
	virtual ~Scene(){ for (int i = 0; i < GameObjects.size(); i++) { delete GameObjects.at(i); } };
};
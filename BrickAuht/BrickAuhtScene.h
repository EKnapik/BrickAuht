#pragma once

#include "Scene.h"
#include "Ball.h"
#include "Material.h"
#include "Mesh.h"
#include "Panel.h"

class BrickAuhtScene : public Scene {
public:
	BrickAuhtScene();
	void Initialize();
	void Update();
	std::vector<Panel*> blocks;
	std::vector<ScenePointLight*> crazyLights;
	std::vector<Ball*> balls;
	GameObject* court;
	ScenePointLight* playerLight;
	ScenePointLight* ballLight;

	~BrickAuhtScene();

private:
	int blockCount = 0;
	bool spacePressed = false;
	int ammo = 10;

	void Shoot();
};
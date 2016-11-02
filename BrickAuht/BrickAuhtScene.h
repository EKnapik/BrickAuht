#pragma once

#include "Scene.h"
#include "Ball.h"
#include "Material.h"
#include "Mesh.h"

class BrickAuhtScene : public Scene {
public:
	BrickAuhtScene();
	void Initialize();
	void Update();
	std::vector<Ball*> blocks;
	Ball* ball;
	Ball* paddle;
	GameObject* court;
	ScenePointLight* playerLight;
	ScenePointLight* ballLight;

	~BrickAuhtScene();
};
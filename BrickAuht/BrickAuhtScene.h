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
	Ball* ball;
	Ball* paddle;
	GameObject* court;
	ScenePointLight* playerLight;
	ScenePointLight* ballLight;

	~BrickAuhtScene();
};
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
	std::vector<Ball*> balls;
	std::vector<Ball*> blocks;
	Ball* paddle;
};
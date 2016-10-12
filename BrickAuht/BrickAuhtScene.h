#pragma once

#include "Scene.h"
#include "Ball.h"
#include "Material.h"
#include "Mesh.h"

class BrickAuhtScene : public Scene {
public:
	BrickAuhtScene(Mesh* shape, Material* mat);
	void Initialize();
	void Update();
	std::vector<Ball*> balls;
	std::vector<Ball*> blocks;
	Ball* paddle;
};
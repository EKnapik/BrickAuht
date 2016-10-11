#pragma once
#include <stdio.h>
#include <vector>

#include "GameMath.h"
#include "GameObject.h"

class GameManager {
	std::vector<GameObject> GameObjects;

	void Update(float dt);
};
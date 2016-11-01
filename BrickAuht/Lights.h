#pragma once
#include "GameMath.h"

struct DirectionalLight {
	VEC4 AmbientColor;
	VEC4 DiffuseColor;
	VEC3 Direction;
};

struct PointLight {
	VEC4 Color;
	VEC3 Position;
};

class SceneDirectionalLight {
public:
	SceneDirectionalLight(VEC4 aColor, VEC4 dColor, VEC3 direction);
	VEC4 AmbientColor;
	VEC4 DiffuseColor;
	VEC3 Direction;
	MAT4X4 ViewMatrix;
};

class ScenePointLight {
public:
	ScenePointLight(VEC4 color, VEC3 position);
	VEC4 Color;
	VEC3 Position;
	MAT4X4 ViewMatrix;
};
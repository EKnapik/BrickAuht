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
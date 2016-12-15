#pragma once

#include "Scene.h"
#include "GameObject.h"
#include "ParticleEmitter.h"

class Panel : public GameObject {
public:
	Panel();
	float width = 1, height = 1;
	void SetWidthHeight(float width, float height) { this->width = width; this->height = height; };
};

class ParticleComponent : public Component {
public:
	ParticleComponent(GameObject* parentObject, Scene* parentScene);
	void Update(float dt);
	virtual ~ParticleComponent();
	GameEntity* entity = nullptr;
private:
	ParticleEmitter* emitter;
	GameObject* ParentObject;
	Scene* ParentScene;
};

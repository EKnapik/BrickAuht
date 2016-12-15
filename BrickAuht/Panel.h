#pragma once

#include "Scene.h"
#include "GameObject.h"
#include "ParticleEmitter.h"

class Panel : public GameObject {
public:
	Panel();
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

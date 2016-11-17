#pragma once

#include <vector>
#include "GameMath.h"
#include <map>
#include <string>

#ifdef WITH_DX
#include "DXCore.h"
#include "SimpleShader.h"
#endif // WITH_DX
#include "GameEntity.h"
#include "Camera.h"

#include "Lights.h"

#include "GameManager.h"
#include "Renderer.h"

#include "ParticleEmitter.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:
	Camera* camera;
	bool mouseDown = false;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void LoadMeshes();
	void LoadMaterials();

	std::vector<GameEntity*> entities;

	GameManager gameManager;
	Renderer* renderer;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	DirectionalLight light;

	bool rShiftToggle = false;
};


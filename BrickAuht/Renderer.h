#pragma once
#include <DirectXMath.h>
#include "DXMathImpl.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Lights.h"



class Renderer
{
public:
	Renderer(Camera *camera, ID3D11DeviceContext *context,
		ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView);
	~Renderer();

	void DrawOneMaterial(GameEntity *entities, INT numEntities, FLOAT deltaTime, FLOAT totalTime);

private:
	Camera *camera;

	ID3D11DeviceContext*	context;
	ID3D11RenderTargetView* backBufferRTV;
	ID3D11DepthStencilView* depthStencilView;
};


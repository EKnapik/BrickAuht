#pragma once

#include "Renderer.h"

class DefferedRenderer : public Renderer
{
public:
	DefferedRenderer(Camera *camera, ID3D11DeviceContext *context, ID3D11Device* device,
		ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView, int width, int height);
	~DefferedRenderer();
	void Render(std::vector<GameEntity*>* gameEntitys, std::vector<SceneDirectionalLight>* directionalLights, std::vector<ScenePointLight>* pointLights, FLOAT deltaTime, FLOAT totalTime);
	

private:

	void gBufferRender(std::vector<GameEntity*>* gameEntitys, std::vector<SceneDirectionalLight>* directionalLights,
					std::vector<ScenePointLight>* pointLights, FLOAT deltaTime, FLOAT totalTime);
	void pointLightRender(std::vector<ScenePointLight>* pointLights);
	void directionalLightRender(std::vector<SceneDirectionalLight>* dirLights);
	//void DrawOneMaterial(std::vector<GameEntity*>* gameEntitys);
	void DrawMultipleMaterials(std::vector<GameEntity*>* gameEntitys);

	// Albedo
	ID3D11RenderTargetView *	AlbedoRTV;
	ID3D11ShaderResourceView*	AlbedoSRV;

	// Normal
	ID3D11RenderTargetView *	NormalRTV;
	ID3D11ShaderResourceView*	NormalSRV;

	// Depth
	ID3D11RenderTargetView *	PositionRTV;
	ID3D11ShaderResourceView*	PositionSRV;

	ID3D11SamplerState* simpleSampler;
	ID3D11BlendState* blendState;
	ID3D11RasterizerState* lightRastState;

	float windowWidth;
	float windowHeight;
};


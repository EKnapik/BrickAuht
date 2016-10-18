#pragma once

#include <DirectXMath.h>
#include "SimpleShader.h"
#include "DXMathImpl.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Lights.h"

class DefferedRenderer
{
public:
	DefferedRenderer(ID3D11DeviceContext *context, ID3D11Device* device, int width, int height);
	~DefferedRenderer();
	void Render();

private:

	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	// Albedo
	ID3D11RenderTargetView *	AlbedoRenderTargetView;
	ID3D11ShaderResourceView*	AlbedoShaderRV;

	// Normal
	ID3D11RenderTargetView *	NormalRenderTargetView;
	ID3D11ShaderResourceView*	NormalShaderRV;

	// Depth
	ID3D11RenderTargetView *	DepthRenderTargetView;
	ID3D11ShaderResourceView*	DepthShaderRV;


};


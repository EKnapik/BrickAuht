#pragma once

#include "Renderer.h"

class DefferedRenderer : public Renderer
{
public:
	DefferedRenderer(Camera *camera, ID3D11DeviceContext *context, ID3D11Device* device,
		ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView, int width, int height);
	~DefferedRenderer();
	void Render();
	

private:

	void gBufferRender();
	void lightRender();

	// Albedo
	ID3D11RenderTargetView *	AlbedoRTV;
	ID3D11ShaderResourceView*	AlbedoSRV;

	// Normal
	ID3D11RenderTargetView *	NormalRTV;
	ID3D11ShaderResourceView*	NormalSRV;

	// Depth
	ID3D11RenderTargetView *	DepthRTV;
	ID3D11ShaderResourceView*	DepthSRV;


};


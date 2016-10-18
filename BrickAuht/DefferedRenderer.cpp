#include "DefferedRenderer.h"



DefferedRenderer::DefferedRenderer(ID3D11DeviceContext *context, ID3D11Device* device, int width, int height)
{
	HRESULT hr = S_OK;

	// Create the albedo texture.
	D3D11_TEXTURE2D_DESC descAlbedoTexture;
	ID3D11Texture2D* AlbedoTexture;
	ZeroMemory(&descAlbedoTexture, sizeof(descAlbedoTexture));
	descAlbedoTexture.Width = width;
	descAlbedoTexture.Height = height;
	descAlbedoTexture.MipLevels = 1;
	descAlbedoTexture.ArraySize = 1;
	descAlbedoTexture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	descAlbedoTexture.SampleDesc.Count = 1;
	descAlbedoTexture.SampleDesc.Quality = 0;
	descAlbedoTexture.Usage = D3D11_USAGE_DEFAULT;
	descAlbedoTexture.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	descAlbedoTexture.CPUAccessFlags = 0;
	descAlbedoTexture.MiscFlags = 0;

	hr = device->CreateTexture2D(&descAlbedoTexture, NULL, &AlbedoTexture);

	if (FAILED(hr))
		printf("Error creating albedo texture.\n");

	// Create the albedo render target.
	D3D11_RENDER_TARGET_VIEW_DESC albedoRTVDesc;
	ZeroMemory(&albedoRTVDesc, sizeof(albedoRTVDesc));
	albedoRTVDesc.Format = descAlbedoTexture.Format;
	albedoRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	albedoRTVDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(AlbedoTexture, &albedoRTVDesc, &AlbedoRenderTargetView);

	if (FAILED(hr))
		printf("Error creating albedo RT.\n");


	// Create the albedo shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC albedoSRVDesc;
	albedoSRVDesc.Format = descAlbedoTexture.Format;
	albedoSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	albedoSRVDesc.Texture2D.MostDetailedMip = 0;
	albedoSRVDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(AlbedoTexture, &albedoSRVDesc, &AlbedoShaderRV);

	if (FAILED(hr))
		printf("Error creating albedo SRV.\n");

	// Don't need the actual texture anymore
	AlbedoTexture->Release();


	// Normal.

	// Create the normal texture.
	D3D11_TEXTURE2D_DESC descNormalTexture;
	ID3D11Texture2D* NormalTexture;
	ZeroMemory(&descNormalTexture, sizeof(descNormalTexture));
	descNormalTexture.Width = width;
	descNormalTexture.Height = height;
	descNormalTexture.MipLevels = 1;
	descNormalTexture.ArraySize = 1;
	descNormalTexture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	descNormalTexture.SampleDesc.Count = 1;
	descNormalTexture.SampleDesc.Quality = 0;
	descNormalTexture.Usage = D3D11_USAGE_DEFAULT;
	descNormalTexture.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	descNormalTexture.CPUAccessFlags = 0;
	descNormalTexture.MiscFlags = 0;

	hr = device->CreateTexture2D(&descNormalTexture, NULL, &NormalTexture);

	if (FAILED(hr))
		printf("Error creating normal texture.\n");


	// Create the Normal render target.
	D3D11_RENDER_TARGET_VIEW_DESC normalRTVDesc;
	ZeroMemory(&normalRTVDesc, sizeof(normalRTVDesc));
	normalRTVDesc.Format = descNormalTexture.Format;
	normalRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	normalRTVDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(NormalTexture, &normalRTVDesc, &NormalRenderTargetView);

	if (FAILED(hr))
		printf("Error creating normal RT.\n");


	// Create the Normal shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC normalSRVDesc;
	normalSRVDesc.Format = descNormalTexture.Format;
	normalSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	normalSRVDesc.Texture2D.MostDetailedMip = 0;
	normalSRVDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(NormalTexture, &normalSRVDesc, &NormalShaderRV);

	if (FAILED(hr))
		printf("Error creating normal SRV.\n");

	// Don't need the actual normal texture
	NormalTexture->Release();

	// Depth.

	// Create the depth texture.
	D3D11_TEXTURE2D_DESC descDepthTexture;
	ID3D11Texture2D* DepthTexture;
	ZeroMemory(&descDepthTexture, sizeof(descDepthTexture));
	descDepthTexture.Width = width;
	descDepthTexture.Height = height;
	descDepthTexture.MipLevels = 1;
	descDepthTexture.ArraySize = 1;
	descDepthTexture.Format = DXGI_FORMAT_R32_FLOAT;
	descDepthTexture.SampleDesc.Count = 1;
	descDepthTexture.SampleDesc.Quality = 0;
	descDepthTexture.Usage = D3D11_USAGE_DEFAULT;
	descDepthTexture.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	descDepthTexture.CPUAccessFlags = 0;
	descDepthTexture.MiscFlags = 0;

	hr = device->CreateTexture2D(&descDepthTexture, NULL, &DepthTexture);

	if (FAILED(hr))
		printf("Error creating depth texture.\n");


	// Create the depth render target.
	D3D11_RENDER_TARGET_VIEW_DESC depthRTVDesc;
	ZeroMemory(&depthRTVDesc, sizeof(depthRTVDesc));
	depthRTVDesc.Format = descDepthTexture.Format;
	depthRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	depthRTVDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(DepthTexture, &depthRTVDesc, &DepthRenderTargetView);

	if (FAILED(hr))
		printf("Error creating depth RT.\n");


	// Create the depth shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC depthSRVDesc;
	depthSRVDesc.Format = descDepthTexture.Format;
	depthSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthSRVDesc.Texture2D.MostDetailedMip = 0;
	depthSRVDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(DepthTexture, &depthSRVDesc, &DepthShaderRV);

	// Don't need the actual depth texture
	DepthTexture->Release();


	// Create The Shaders
}


DefferedRenderer::~DefferedRenderer()
{
	// Albedo
	AlbedoRenderTargetView->Release();
	AlbedoShaderRV->Release();

	// Normal
	NormalRenderTargetView->Release();
	NormalShaderRV->Release();

	// Depth
	DepthRenderTargetView->Release();
	DepthShaderRV->Release();

	// Shaders
	delete vertexShader;
	delete pixelShader;
}

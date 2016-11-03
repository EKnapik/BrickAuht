#include "DefferedRenderer.h"



DefferedRenderer::DefferedRenderer(Camera *camera, ID3D11DeviceContext *context, ID3D11Device* device, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView, int width, int height) :
	Renderer(camera, context, device, backBufferRTV, depthStencilView)
{
	windowHeight = height;
	windowWidth = width;

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

	hr = device->CreateRenderTargetView(AlbedoTexture, &albedoRTVDesc, &AlbedoRTV);

	if (FAILED(hr))
		printf("Error creating albedo RT.\n");


	// Create the albedo shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC albedoSRVDesc;
	albedoSRVDesc.Format = descAlbedoTexture.Format;
	albedoSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	albedoSRVDesc.Texture2D.MostDetailedMip = 0;
	albedoSRVDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(AlbedoTexture, &albedoSRVDesc, &AlbedoSRV);

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

	hr = device->CreateRenderTargetView(NormalTexture, &normalRTVDesc, &NormalRTV);

	if (FAILED(hr))
		printf("Error creating normal RT.\n");


	// Create the Normal shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC normalSRVDesc;
	normalSRVDesc.Format = descNormalTexture.Format;
	normalSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	normalSRVDesc.Texture2D.MostDetailedMip = 0;
	normalSRVDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(NormalTexture, &normalSRVDesc, &NormalSRV);

	if (FAILED(hr))
		printf("Error creating normal SRV.\n");

	// Don't need the actual normal texture
	NormalTexture->Release();

	// Depth.
	// Create the depth texture.
	D3D11_TEXTURE2D_DESC descPositionTexture;
	ID3D11Texture2D* PositionTexture;
	ZeroMemory(&descPositionTexture, sizeof(descPositionTexture));
	descPositionTexture.Width = width;
	descPositionTexture.Height = height;
	descPositionTexture.MipLevels = 1;
	descPositionTexture.ArraySize = 1;
	descPositionTexture.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	descPositionTexture.SampleDesc.Count = 1;
	descPositionTexture.SampleDesc.Quality = 0;
	descPositionTexture.Usage = D3D11_USAGE_DEFAULT;
	descPositionTexture.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	descPositionTexture.CPUAccessFlags = 0;
	descPositionTexture.MiscFlags = 0;

	hr = device->CreateTexture2D(&descPositionTexture, NULL, &PositionTexture);

	if (FAILED(hr))
		printf("Error creating depth texture.\n");


	// Create the depth render target.
	D3D11_RENDER_TARGET_VIEW_DESC PositionRTVDesc;
	ZeroMemory(&PositionRTVDesc, sizeof(PositionRTVDesc));
	PositionRTVDesc.Format = descPositionTexture.Format;
	PositionRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	PositionRTVDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(PositionTexture, &PositionRTVDesc, &PositionRTV);

	if (FAILED(hr))
		printf("Error creating depth RT.\n");


	// Create the depth shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC PositionSRVDesc;
	PositionSRVDesc.Format = descPositionTexture.Format;
	PositionSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	PositionSRVDesc.Texture2D.MostDetailedMip = 0;
	PositionSRVDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(PositionTexture, &PositionSRVDesc, &PositionSRV);

	// Don't need the actual depth texture
	PositionTexture->Release();

	// create sampler
	D3D11_SAMPLER_DESC sampleDesc = {};
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampleDesc, &simpleSampler);

	// Create addative blend state needed for light rendering
	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable		= false;
	bd.IndependentBlendEnable		= false;
	bd.RenderTarget[0].BlendEnable	= true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&bd, &blendState);

	// Create raster state for rendering lights propperly
	D3D11_RASTERIZER_DESC ligtRastDesc = {};
	ligtRastDesc.FillMode = D3D11_FILL_SOLID;
	ligtRastDesc.CullMode = D3D11_CULL_FRONT;
	ligtRastDesc.DepthClipEnable = false;
	device->CreateRasterizerState(&ligtRastDesc, &lightRastState);
}


DefferedRenderer::~DefferedRenderer()
{
	// Albedo
	AlbedoRTV->Release();
	AlbedoSRV->Release();

	// Normal
	NormalRTV->Release();
	NormalSRV->Release();

	// Depth
	PositionRTV->Release();
	PositionSRV->Release();

	simpleSampler->Release();
	blendState->Release();
	lightRastState->Release();
}


void DefferedRenderer::Render(std::vector<GameEntity*>* gameEntitys, std::vector<SceneDirectionalLight>* directionalLights,
					std::vector<ScenePointLight>* pointLights, FLOAT deltaTime, FLOAT totalTime)
{
	// Background color (Cornflower Blue)
	const float clearColor[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	const float black[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	context->ClearRenderTargetView(backBufferRTV, black);
	// clear all the render targets
	context->ClearRenderTargetView(AlbedoRTV, black);
	context->ClearRenderTargetView(NormalRTV, black);
	context->ClearRenderTargetView(PositionRTV, black);

	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	gBufferRender(gameEntitys, directionalLights, pointLights, deltaTime, totalTime);
	context->OMSetRenderTargets(1, &backBufferRTV, 0);
	pointLightRender(pointLights);
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	directionalLightRender(directionalLights);
	DrawSkyBox();
}


void DefferedRenderer::gBufferRender(std::vector<GameEntity*>* gameEntitys, std::vector<SceneDirectionalLight>* directionalLights,
					std::vector<ScenePointLight>* pointLights, FLOAT deltaTime, FLOAT totalTime)
{
	ID3D11RenderTargetView* RTViews[3] = { AlbedoRTV, NormalRTV, PositionRTV };

	context->OMSetRenderTargets(3, RTViews, depthStencilView);
	
	//OUR SCENE NEEDS AT LEAST ONE DIRECTIONAL LIGHT TO LOOK GOOD
	if (directionalLights->size() <= 0)
		throw "Scene needs at least 1 directional light";

	// RENDER NORMALLY NOW
	DrawMultipleMaterials(gameEntitys, &directionalLights->at(0));
}



/// Only Render Sphere Lights
void DefferedRenderer::pointLightRender(std::vector<ScenePointLight>* pointLights)
{
	float factors[4] = { 1,1,1,1 };
	context->OMSetBlendState(blendState, factors, 0xFFFFFFFF);
	context->RSSetState(lightRastState);

	SimpleVertexShader* vertexShader = GetVertexShader("default");
	SimplePixelShader* pixelShader = GetPixelShader("sphereLight");
	vertexShader->SetShader();
	pixelShader->SetShader();

	// Send G buffers to pixel shader
	pixelShader->SetSamplerState("basicSampler", simpleSampler);
	pixelShader->SetShaderResourceView("gAlbedo", AlbedoSRV);
	pixelShader->SetShaderResourceView("gNormal", NormalSRV);
	pixelShader->SetShaderResourceView("gPosition", PositionSRV);
	// send constant data
	vertexShader->SetMatrix4x4("view", *camera->GetView());
	vertexShader->SetMatrix4x4("projection", *camera->GetProjection());

	pixelShader->SetFloat3("cameraPosition", *camera->GetPosition());
	pixelShader->SetFloat("width", windowWidth);
	pixelShader->SetFloat("height", windowHeight);
	
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Mesh* meshTmp = GetMesh("sphere");
	ID3D11Buffer* vertTemp = meshTmp->GetVertexBuffer();
	PointLight light;
	MAT4X4 world;

	for (int i = 0; i < pointLights->size(); i++) {
		// Send light info to pixel shader
		light.Color = pointLights->at(i).Color;
		light.Position = pointLights->at(i).Position;
		// divide by 10 so attenuation looks good
		light.Radius = pointLights->at(i).Radius.x/9;
		pixelShader->SetData("pointLight", &light, sizeof(PointLight));

		GMath::SetTransposeMatrix(&world, &(GMath::CreateScaleMatrix(&pointLights->at(i).Radius) * GMath::CreateTranslationMatrix(&pointLights->at(i).Position)));
		vertexShader->SetMatrix4x4("world", world);

		pixelShader->CopyAllBufferData();
		vertexShader->CopyAllBufferData();

		context->IASetVertexBuffers(0, 1, &vertTemp, &stride, &offset);
		context->IASetIndexBuffer(meshTmp->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(meshTmp->GetIndexCount(), 0, 0);
	}

	// RESET STATES
	pixelShader->SetShaderResourceView("gAlbedo", 0);
	pixelShader->SetShaderResourceView("gNormal", 0);
	pixelShader->SetShaderResourceView("gPosition", 0);
	context->OMSetBlendState(0, factors, 0xFFFFFFFF);
	context->RSSetState(0);
	return;
}


void DefferedRenderer::directionalLightRender(std::vector<SceneDirectionalLight>* dirLights) {
	float factors[4] = { 1,1,1,1 };
	context->OMSetBlendState(blendState, factors, 0xFFFFFFFF);
	
	SimpleVertexShader* vertexShader = GetVertexShader("quad");
	SimplePixelShader* pixelShader = GetPixelShader("quad");
	vertexShader->SetShader();
	pixelShader->SetShader();

	// Send G buffers to pixel shader
	pixelShader->SetSamplerState("basicSampler", simpleSampler);
	pixelShader->SetShaderResourceView("gAlbedo", AlbedoSRV);
	pixelShader->SetShaderResourceView("gNormal", NormalSRV);
	pixelShader->SetShaderResourceView("gPosition", PositionSRV);

	pixelShader->SetFloat3("cameraPosition", *camera->GetPosition());
	vertexShader->CopyAllBufferData();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Mesh* meshTmp = GetMesh("quad");
	ID3D11Buffer* vertTemp = meshTmp->GetVertexBuffer();
	DirectionalLight light;
	MAT4X4 world;
	for (int i = 0; i < dirLights->size(); i++) {
		// Send light info to pixel shader
		light.AmbientColor = dirLights->at(i).AmbientColor;
		light.DiffuseColor = dirLights->at(i).DiffuseColor;
		light.Direction = dirLights->at(i).Direction;
		pixelShader->SetData("dirLight", &light, sizeof(DirectionalLight));
		pixelShader->CopyAllBufferData();

		context->IASetVertexBuffers(0, 1, &vertTemp, &stride, &offset);
		context->IASetIndexBuffer(meshTmp->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(meshTmp->GetIndexCount(), 0, 0);
	}

	// RESET STATES
	pixelShader->SetShaderResourceView("gAlbedo", 0);
	pixelShader->SetShaderResourceView("gNormal", 0);
	pixelShader->SetShaderResourceView("gPosition", 0);
	context->OMSetBlendState(0, factors, 0xFFFFFFFF);
	context->RSSetState(0);
	return;
}

void DefferedRenderer::DrawMultipleMaterials(std::vector<GameEntity*>* gameEntitys, SceneDirectionalLight* firstDirectionalLight)
{
	SimpleVertexShader* vertexShader = GetVertexShader("gBuffer");
	SimplePixelShader* pixelShader = GetPixelShader("gBuffer");

	if (gameEntitys->size() == 0) return;
	float factors[4] = { 1,1,1,1 };

	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable = false;
	bd.IndependentBlendEnable = false;
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState* blendState;
	device->CreateBlendState(&bd, &blendState);

	//Do shadow stuff!
	vertexShader->SetMatrix4x4("shadowView", firstDirectionalLight->ViewMatrix);
	vertexShader->SetMatrix4x4("shadowProjection", shadowDirectionalProjectionMatrix);
	pixelShader->SetShaderResourceView("ShadowMap", shadowSRV);
	pixelShader->SetSamplerState("ShadowSampler", GetSampler("shadow"));

	for (int i = 0; i < gameEntitys->size(); i++)
	{
		Material* material = GetMaterial(gameEntitys->at(i)->GetMaterial());
		vertexShader->SetShader();
		pixelShader->SetShader();

		if (material->transparency == true)
		{	
			context->OMSetBlendState(blendState, factors, 0xFFFFFFFF);
			blendMode = true;
		} else if (blendMode == true)
		{
			context->OMSetBlendState(0, factors, 0xFFFFFFFF);
			blendMode = false;
		}

		// Send texture Info
		pixelShader->SetSamplerState("basicSampler", material->GetSamplerState());
		pixelShader->SetShaderResourceView("diffuseTexture", material->GetSRV());
		//pixelShader->SetShaderResourceView("NormalMap", material->GetSRV());

		// Send Geometry
		vertexShader->SetMatrix4x4("view", *camera->GetView());
		vertexShader->SetMatrix4x4("projection", *camera->GetProjection());
		pixelShader->SetFloat3("cameraPosition", *camera->GetPosition());
		pixelShader->SetShaderResourceView("Sky", skyBox->GetSRV());

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Mesh* meshTmp;
		vertexShader->SetMatrix4x4("world", *gameEntitys->at(i)->GetWorld());
		vertexShader->CopyAllBufferData();
		pixelShader->CopyAllBufferData();

		meshTmp = GetMesh(gameEntitys->at(i)->GetMesh());
		ID3D11Buffer* vertTemp = meshTmp->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &vertTemp, &stride, &offset);
		context->IASetIndexBuffer(meshTmp->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(meshTmp->GetIndexCount(), 0, 0);
	}

	blendState->Release();
	context->OMSetBlendState(0, factors, 0xFFFFFFFF);
	context->RSSetState(0);
}




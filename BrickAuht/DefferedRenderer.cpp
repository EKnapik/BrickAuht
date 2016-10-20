#include "DefferedRenderer.h"



DefferedRenderer::DefferedRenderer(Camera *camera, ID3D11DeviceContext *context, ID3D11Device* device, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView, int width, int height) :
	Renderer(camera, context, device, backBufferRTV, depthStencilView)
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

	// Add required quad mesh
	AddMesh("quad", new Mesh(device));
	// Create The Shaders
	AddVertexShader("gBuffer", L"gBufferVertexShader.cso");
	AddPixelShader("gBuffer", L"gBufferPixelShader.cso");

	AddVertexShader("quad", L"quadVertexShader.cso");
	AddPixelShader("quad", L"quadPixelShader.cso");
	AddPixelShader("sphereLight", L"sphereLightPixelShader.cso");
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
}


void DefferedRenderer::Render(std::vector<GameEntity*>* gameEntitys, FLOAT deltaTime, FLOAT totalTime)
{
	// Background color (Cornflower Blue)
	const float clearColor[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	context->ClearRenderTargetView(backBufferRTV, clearColor);
	// clear all the render targets
	context->ClearRenderTargetView(AlbedoRTV, clearColor);
	context->ClearRenderTargetView(NormalRTV, clearColor);
	context->ClearRenderTargetView(PositionRTV, clearColor);

	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	std::vector<GameEntity*> solidEntities;
	std::vector<GameEntity*> lightEntities;
	for (int i = 0; i < gameEntitys->size(); i++)
	{
		if (gameEntitys->at(i)->GetMaterial() == "light")
		{
			lightEntities.push_back(gameEntitys->at(i));
		}
		else {
			solidEntities.push_back(gameEntitys->at(i));
		}
	}

	gBufferRender(&solidEntities);
	lightRender(&lightEntities);
}


void DefferedRenderer::gBufferRender(std::vector<GameEntity*>* gameEntitys)
{
	ID3D11RenderTargetView* RTViews[3] = { AlbedoRTV, NormalRTV, PositionRTV };

	context->OMSetRenderTargets(3, RTViews, depthStencilView);

	// RENDER NORMALLY NOW
	DrawOneMaterial(gameEntitys);
}



/// Only Render Sphere lights currently, could switch the mesh for cone lights and
/// directions lights (full quads)
void DefferedRenderer::lightRender(std::vector<GameEntity*>* gameEntitys)
{
	// Create a light for testing
	// can't destroy this!!!
	GameEntity* testLight = new GameEntity("sphere", "light");
	testLight->SetPosition(VEC3(0, 0, 5));
	testLight->SetScale(VEC3(4, 4, 4));

	////////////////////////////////////////////////////////
	context->OMSetRenderTargets(1, &backBufferRTV, 0);
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
	// send light
	PointLight light;
	light.Color = VEC4(0.8, 0.1, 0.3, 1.0);
	light.Position = testLight->GetPosition();
	// TODO
	pixelShader->SetData("pointLight", &light, sizeof(PointLight));
	pixelShader->SetFloat3("cameraPosition", *camera->GetPosition());
	pixelShader->SetFloat("width", 1280.0f);
	pixelShader->SetFloat("height", 720.0f);
	pixelShader->CopyAllBufferData();


	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Mesh* meshTmp;
	meshTmp = GetMesh("sphere");

	ID3D11Buffer* vertTemp = meshTmp->GetVertexBuffer();
	vertexShader->SetMatrix4x4("world", *testLight->GetWorld());
	vertexShader->CopyAllBufferData();
	context->IASetVertexBuffers(0, 1, &vertTemp, &stride, &offset);
	context->IASetIndexBuffer(meshTmp->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(meshTmp->GetIndexCount(), 0, 0);


	testLight->Release();
	return;

	// TODO: Send lights that sample the position data in the buffer. To render properly
	// Change gDepth to be gPosition and then I can render the sphere of influence sampling the pixel values and calculating
	// I need to pass in the light pos, intensity, color that can be achieved with game entity values
	for (int i = 0; i < gameEntitys->size(); i++) {
		vertexShader->SetMatrix4x4("world", *gameEntitys->at(i)->GetWorld());
		vertexShader->CopyAllBufferData();

		meshTmp = GetMesh(gameEntitys->at(i)->GetMesh());
		ID3D11Buffer* vertTemp = meshTmp->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &vertTemp, &stride, &offset);
		context->IASetIndexBuffer(meshTmp->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(meshTmp->GetIndexCount(), 0, 0);
	}

	
}

void DefferedRenderer::DrawOneMaterial(std::vector<GameEntity*>* gameEntitys)
{
	SimpleVertexShader* vertexShader = GetVertexShader("gBuffer");
	SimplePixelShader* pixelShader = GetPixelShader("gBuffer");
	vertexShader->SetShader();
	pixelShader->SetShader();

	if (gameEntitys->size() == 0) return;
	Material* material = GetMaterial(gameEntitys->at(0)->GetMaterial());

	// Send texture Info
	pixelShader->SetSamplerState("basicSampler", material->GetSamplerState());
	pixelShader->SetShaderResourceView("diffuseTexture", material->GetSRV());
	//pixelShader->SetShaderResourceView("NormalMap", material->GetNormMap());
	pixelShader->CopyAllBufferData();

	// Send Geometry
	vertexShader->SetMatrix4x4("view", *camera->GetView());
	vertexShader->SetMatrix4x4("projection", *camera->GetProjection());
	//pixelShader->SetFloat3("cameraPosition", *camera->GetPosition());  NOT CURRENTLY USED

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Mesh* meshTmp;
	for (int i = 0; i < gameEntitys->size(); i++) {
		vertexShader->SetMatrix4x4("world", *gameEntitys->at(i)->GetWorld());
		vertexShader->CopyAllBufferData();

		meshTmp = GetMesh(gameEntitys->at(i)->GetMesh());
		ID3D11Buffer* vertTemp = meshTmp->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &vertTemp, &stride, &offset);
		context->IASetIndexBuffer(meshTmp->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(meshTmp->GetIndexCount(), 0, 0);
	}

}


void DefferedRenderer::DrawMultipleMaterials(std::vector<GameEntity*>* gameEntitys)
{
	SimpleVertexShader* vertexShader = GetVertexShader("gBuffer");
	SimplePixelShader* pixelShader = GetPixelShader("gBuffer");

	if (gameEntitys->size() == 0) return;

	for (int i = 0; i < gameEntitys->size(); i++) {
		Material* material = GetMaterial(gameEntitys->at(i)->GetMaterial());
		//SimpleVertexShader* vertexShader = GetVertexShader(gameEntitys->at(i)->vertexShader);
		//SimplePixelShader* pixelShader = GetPixelShader(gameEntitys->at(i)->pixelShader);
		vertexShader->SetShader();
		pixelShader->SetShader();

		// Send texture Info
		pixelShader->SetSamplerState("basicSampler", material->GetSamplerState());
		pixelShader->SetShaderResourceView("diffuseTexture", material->GetSRV());
		//pixelShader->SetShaderResourceView("NormalMap", material->GetNormMap());

		// Send Lighting Info
		DirectionalLight dLight;
		dLight.AmbientColor = VEC4(0.1f, 0.1f, 0.1f, 0.0f);
		dLight.DiffuseColor = VEC4(0, 0, 1, 0);
		dLight.Direction = VEC3(1, -1, 0);
		pixelShader->SetData(
			"light",
			&dLight,
			sizeof(DirectionalLight));

		// Create a ground light so every object is lit a little bit from the ground
		DirectionalLight gLight;
		gLight.AmbientColor = VEC4(0.1f, 0.1f, 0.1f, 1.0f);
		gLight.DiffuseColor = VEC4(71.0f / 255.0f, 28.0f / 255.0f, 1.0f / 255.0f, 1.0f);
		gLight.Direction = VEC3(0, 1, 0);

		pixelShader->SetData(
			"groundLight",
			&gLight,
			sizeof(DirectionalLight));

		PointLight pLight;
		pLight.Color = VEC4(253.0f / 255.0f, 184.0f / 255.0f, 19.0f / 255.0f, 1.0f);
		pLight.Position = VEC3(0, 0, -5);

		pixelShader->SetData(
			"pointLight",
			&pLight,
			sizeof(PointLight));

		pixelShader->CopyAllBufferData();

		// Send Geometry
		vertexShader->SetMatrix4x4("view", *camera->GetView());
		vertexShader->SetMatrix4x4("projection", *camera->GetProjection());
		pixelShader->SetFloat3("cameraPosition", *camera->GetPosition());

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Mesh* meshTmp;
		vertexShader->SetMatrix4x4("world", *gameEntitys->at(i)->GetWorld());
		vertexShader->CopyAllBufferData();

		meshTmp = GetMesh(gameEntitys->at(i)->GetMesh());
		ID3D11Buffer* vertTemp = meshTmp->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &vertTemp, &stride, &offset);
		context->IASetIndexBuffer(meshTmp->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(meshTmp->GetIndexCount(), 0, 0);
	}
}